/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021 Renzo E. Navas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Renzo E. Navas <renzo.navas@inria.fr>
 */

#include "bandit-delayed-reward-intelligence.h"

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("BanditDelayedRewardIntelligence");
NS_OBJECT_ENSURE_REGISTERED (BanditDelayedRewardIntelligence);

TypeId BanditDelayedRewardIntelligence::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BanditDelayedRewardIntelligence")
    .SetGroupName ("lorawan")
    .AddConstructor<BanditDelayedRewardIntelligence> ()
    ;
  return tid;
}
BanditDelayedRewardIntelligence::BanditDelayedRewardIntelligence ()
{
  // TODO Auto-generated constructor stub
  for (int i = 0; i < HARDCODED_NUMBER_ARMS; i++)
    {
      // The exploration will depend a lot on the bootstrapping of the bandit, see AdrBanditAgent::AdrBanditAgent ():.
      double armReward = pow(2, i)/pow(2, HARDCODED_NUMBER_ARMS-1); // pow(2, i);//pow(2, i)/pow(2, HARDCODED_NUMBER_ARMS-1); // armReward= 1 -> equal weight, will prioritize raw PDR.

      //this->m_armsAndRewardsVector.push_back (arm_stats (0, 0, 0.0, pow(2, i)/pow(2, HARDCODED_NUMBER_ARMS-1)));
      this->m_armsAndRewardsVector.push_back (arm_stats (0, 0, 0.0, armReward  ));
    }

}

BanditDelayedRewardIntelligence::~BanditDelayedRewardIntelligence ()
{
  // TODO Auto-generated destructor stub
}


void
BanditDelayedRewardIntelligence::InitBanditAgentAndArms (
    Ptr<AdrBanditAgent> adrBanditAgent)
{

  // This function is not called, move all to a proper Constructor
  this->m_adrBanditAgent = adrBanditAgent;
  this->m_banditNeedsStats = true;
  this->m_waitingForStats = false;


  //this->CleanArmsStats();
  //TODO: initialize arms use reward vector

  NS_LOG_INFO("\033[1;31m");
  NS_LOG_INFO("m_armsAndRewardsVector.size()" << m_armsAndRewardsVector.size());
  //m_armsAndRewardsVector.size();
 NS_LOG_INFO("\033[0m");
}



void
BanditDelayedRewardIntelligence::UpdateUsedArm (size_t armNumber,  int frameCnt)
{
  ++std::get<0>(m_armsAndRewardsVector[armNumber]);

  if (frameCnt > m_frmCntMaxWithoutStats) m_frmCntMaxWithoutStats = frameCnt;

  // here we could also put the logic to set the m_needStats boolean
}

Ptr<BanditRewardReq>
BanditDelayedRewardIntelligence::GetRewardsMacCommandReq (uint16_t currentFrame)
{

  m_requestedMaxFrmCntReward = currentFrame; // Important to keep track, because in case of re-sent message m_requestedMaxFrmCntReward will be < m_frmCntMaxWithoutStats

  uint8_t frameDelta =  currentFrame - m_frmCntMinWithoutStats;
  // Colored Terminal: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  NS_LOG_INFO("\033[1;31m");

  NS_LOG_INFO("m_armsAndRewardsVector.size(): " << m_armsAndRewardsVector.size());
  NS_LOG_INFO("printArmsAndRewardsVector: "<< printArmsAndRewardsVector());
  NS_LOG_INFO("GetRewardsMacCommandReq. frameDelta = " << unsigned(frameDelta) << "  currentFrame: " << currentFrame);

  Ptr<BanditRewardReq> req = CreateObject<BanditRewardReq> (currentFrame, frameDelta); //CreateObject<BanditRewardReq> vs Create<BanditRewardReq>?
  m_waitingForStats= true;

  NS_LOG_INFO("\033[0m");
  return req;
}

void
BanditDelayedRewardIntelligence::UpdateRewardsAns (
    Ptr<BanditRewardAns> delayedRewardsAns)
{


  m_waitingForStats= false; /*TODO: remove, currently not using it for the Bandit Intelligence decisions/logic ... */


  std::vector<int> drStatistics = delayedRewardsAns->GetDataRateStatistics();

  //drStatistics.size() == m_adrBanditAgent->GetNumberOfArms() == m_armsAndRewardsVector.size() ; <-- this has to be true.. if not we are in problems!

  for (size_t i = 0; i < m_adrBanditAgent->GetNumberOfArms() ; i++)
    {
      std::get<1>(m_armsAndRewardsVector[i]) += drStatistics[i]; // The Received Packets using that Arm Parameters
    }

  m_frmCntMinWithoutStats = m_requestedMaxFrmCntReward+1;

  //We update the Bandit in a different function, to de-couple the logic even better. This will update and clean the stats.
  ConsolidateRewardsIntoBandit ();




}

void
BanditDelayedRewardIntelligence::ConsolidateRewardsIntoBandit ()
{


  for (size_t currentArm = 0; currentArm < m_adrBanditAgent->GetNumberOfArms() ; currentArm++)
    {
      int timesArmUsed      = std::get<0>(m_armsAndRewardsVector[currentArm]);
      if( timesArmUsed == 0) continue; // The arm was not used! (no rewards to update). We avoid div/0


      int timesArmWorked    = std::get<1>(m_armsAndRewardsVector[currentArm]);
      int timesArmWorkedNOT = timesArmUsed - timesArmWorked;

      // std::get<2>(m_armsAndRewardsVector[currentArm]) --> Currently not used, it  was meant to store a PDR-based reward
      double armWorkedReward    = std::get<3>(m_armsAndRewardsVector[currentArm]);
      double armWorkedNOTReward = 0;

      NS_LOG_INFO("timesArmWorked: "    << timesArmWorked    <<" , armWorkedReward: "<< armWorkedReward);
      NS_LOG_INFO("timesArmWorkedNOT: " << timesArmWorkedNOT <<" , armWorkedNOTReward: "<< armWorkedNOTReward);

      //double reward = std::get<2>(m_armsAndRewardsVector[currentArm]) = std::get<1>(m_armsAndRewardsVector[currentArm])/std::get<0>(m_armsAndRewardsVector[i]); // The Packet Delivery Ratio (PDR) of that Arm

      // Every "pull" of this arm will correspond to a  m_adrBanditAgent->UpdateReward(i, reward);
      for(int i = 0; i < timesArmWorked   ; i++) m_adrBanditAgent->UpdateReward(currentArm, armWorkedReward   );
      for(int i = 0; i < timesArmWorkedNOT; i++) m_adrBanditAgent->UpdateReward(currentArm, armWorkedNOTReward);

    }

  NS_LOG_FUNCTION("printArmsAndRewardsVector():\n"<< printArmsAndRewardsVector());

  CleanArmsStats ();

}


void
BanditDelayedRewardIntelligence::CleanArmsStats ()
{
  for (unsigned int i = 0; i < m_armsAndRewardsVector.size(); i++)
    {
      std::get<0>(m_armsAndRewardsVector[i]) = std::get<1>(m_armsAndRewardsVector[i]) =  std::get<2>(m_armsAndRewardsVector[i]) = 0;
    }
}


bool
BanditDelayedRewardIntelligence::isGetRewardsMacCommandReqNeeded () const
{
  // Here we will have the bootstrap logic: agressive in the beginning (<64 frames), less frequent later
  return m_banditNeedsStats;
}


bool
BanditDelayedRewardIntelligence::isBanditNeedsStats () const
{
  return m_banditNeedsStats;
}

void
BanditDelayedRewardIntelligence::setBanditNeedsStats (
    bool mBanditNeedsStats)
{
  m_banditNeedsStats = mBanditNeedsStats;
}



std::string
BanditDelayedRewardIntelligence::printArmsAndRewardsVector ()
{
  std::stringstream ss;

  ss<<"\n";
  ss<<"(Sent\t, Rcvd\t, PDR\t, Weight \t)\n";

  for (unsigned int i = 0; i < m_armsAndRewardsVector.size(); i++)
    {
      ss << "("    << std::get<0>(m_armsAndRewardsVector[i]) << "\t, " << std::get<1>(m_armsAndRewardsVector[i])
	 <<  "\t, " << std::get<2>(m_armsAndRewardsVector[i]) << "\t, " << std::get<3>(m_armsAndRewardsVector[i])<< " )\n";
    }

  return ss.str();

}

}
}
