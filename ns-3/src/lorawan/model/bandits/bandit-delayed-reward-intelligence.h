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

#ifndef SRC_LORAWAN_MODEL_BANDITS_SRC_BANDIT_DELAYED_REWARD_HELPER__H
#define SRC_LORAWAN_MODEL_BANDITS_SRC_BANDIT_DELAYED_REWARD_HELPER__H


#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/adr-bandit-agent.h"
#include "ns3/mac-command.h"
//#include "ns3/network-controller-components.h"
//#include "ns3/end-device-status.h" // for ReceivedPacketList

namespace ns3 {
namespace lorawan {



/**
 * @brief A class to help withe Bandit Delayed Rewards calculation
 *
 */
class BanditDelayedRewardIntelligence : public Object
{
public:
  static TypeId GetTypeId (void);

  //Constructor
  BanditDelayedRewardIntelligence ();
  //BanditDelayedRewardIntelligence (Ptr<AdrBanditAgent> adrBanditAgent);

  //Destructor
  virtual ~BanditDelayedRewardIntelligence ();



  void InitBanditAgentAndArms(Ptr<AdrBanditAgent> adrBanditAgent); /* Could be on constructor */




  void setBanditNeedsStats (bool mBanditNeedsStats = true);
  bool isBanditNeedsStats () const;


  bool isGetRewardsMacCommandReqNeeded () const;

  Ptr<BanditRewardReq> GetRewardsMacCommandReq(uint16_t currentFrame);

  void UpdateRewardsAns(Ptr<BanditRewardAns> delayedRewardsAns);

  /**
   * @brief This function is to decouple the logic from  UpdateRewardsAns (depending on MAC command)
   * Here we focus only in calculated delayed reward per arm, and we could try different rewards functions.
   *
   *
   */
  void ConsolidateRewardsIntoBandit();


  void UpdateUsedArm(size_t armNumber, int frameCnt);

  void CleanArmsStats();



//protected:
  Ptr<AdrBanditAgent> m_adrBanditAgent; // Pointer to the agent to update the Rewards



  bool m_waitingForStats = false;
  std::string printArmsAndRewardsVector();

protected:

  bool m_banditNeedsStats = true;



  // Todo: Clean the logic  of  this FrmCounters ..
  int m_frmCntMinWithoutStats=1;
  int m_frmCntMaxWithoutStats=0;

  int m_requestedMaxFrmCntReward=0;
  //int m_frmCntDeltaMin=0;


  //int m_currentFrmCntReward=0;


  static const int HARDCODED_NUMBER_ARMS = 6 ; // TODO: fix this with time to dinamically allocate the "m_armsAndRewards" atributo on InitBanditAgentAndArms(Ptr<AdrBanditAgent> adrBanditAgent)


  int m_armsAndRewards[HARDCODED_NUMBER_ARMS][3] = {}; // https://stackoverflow.com/questions/3948290/what-is-the-safe-way-to-fill-multidimensional-array-using-stdfill
  //std::vector<unsigned long> m_usedArms;

  typedef std::tuple <int, int, double> arm_stats; // <packets sent, packets rcv, "reward">
  std::vector<arm_stats> m_armsAndRewardsVector;


};


}
}

#endif /* SRC_LORAWAN_MODEL_BANDITS_SRC_BANDIT_DELAYED_REWARD_HELPER__H */
