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
      this->m_armsAndRewardsVector.push_back (arm_stats (0, 0, 0.0));
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


  m_waitingForStats= false; /*TODO: remove, curerntly not using it?*/
  //if (m_requestedMaxFrmCntReward == currentFCnt) ...

  std::vector<int> drStatistics = delayedRewardsAns->GetDataRateStatistics();

  //drStatistics.size() == m_adrBanditAgent->GetNumberOfArms() == m_armsAndRewardsVector.size()

  for (size_t i = 0; i < m_adrBanditAgent->GetNumberOfArms() ; i++)
    {
      std::get<1>(m_armsAndRewardsVector[i]) += drStatistics[i]; // The Packet Delivery Ratio of that Arm
      //std::get<2>(m_armsAndRewardsVector[i]) = drStatistics[i]/std::get<0>(m_armsAndRewardsVector[i]); // The "Packet Delivery Ratio" of that "Arm" WARNING Division by 0 !!!!!!!
    }
  //this->m_adrBanditAgent->UpdateReward(m_dataRate, cost_for_arm[m_dataRate]);

  m_frmCntMinWithoutStats = m_requestedMaxFrmCntReward+1;
  //this->CleanArmsStats(); // After updating the bandit, we clean the m_armsAndRewards stats object; This lines gives a Segmentation fault at runtime!!!

  ConsolidateRewardsIntoBandit ();


}

void
BanditDelayedRewardIntelligence::ConsolidateRewardsIntoBandit ()
{


  for (size_t i = 0; i < m_adrBanditAgent->GetNumberOfArms() ; i++)
    {
      if(std::get<0>(m_armsAndRewardsVector[i]) == 0) continue; // The arm was not used! (no reward to update)

      double reward = std::get<2>(m_armsAndRewardsVector[i]) = std::get<1>(m_armsAndRewardsVector[i])/std::get<0>(m_armsAndRewardsVector[i]); // The Packet Delivery Ratio of that Arm
      //std::get<2>(m_armsAndRewardsVector[i]) = drStatistics[i]/std::get<0>(m_armsAndRewardsVector[i]); // The "Packet Delivery Ratio" of that "Arm" WARNING Division by 0 !!!!!!!
      NS_LOG_INFO("ConsolidateRewardsIntoBandit. Arm DR=: " <<i<<" , Reward: "<< reward);
      m_adrBanditAgent->UpdateReward(i, reward);
    }

  NS_LOG_FUNCTION("printArmsAndRewardsVector():\n"<< printArmsAndRewardsVector());

  CleanArmsStats ();

  //TODO: Call function to CleanArmsStats from the vector!!!

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
  // Here we will have the bootstrap logic agressive in the beginning (<64 frames), less frequent later
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

  for (unsigned int i = 0; i < m_armsAndRewardsVector.size(); i++)
    {
      ss << "(" << std::get<0>(m_armsAndRewardsVector[i]) << " , " << std::get<1>(m_armsAndRewardsVector[i]) <<  " , " << std::get<2>(m_armsAndRewardsVector[i]) << " )\n";
    }

  return ss.str();

}

}
}
