/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021 INRIA
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
#include "ns3/bandit-constants.h"
//#include "ns3/end-device-status.h" // for ReceivedPacketList
#include <random> // For the bernoulli distribution


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

  /**
   * @brief Determines if the bandit needs stats/feedback
   *
   * @return true if if it needs, false otherwise
   */
  bool isBanditNeedsStats () const;


  Ptr<BanditRewardReq> GetRewardsMacCommandReq(uint16_t currentFrame);

  void UpdateRewardsAns(Ptr<BanditRewardAns> delayedRewardsAns);

  /**
   * @brief This function is to decouple the logic from  UpdateRewardsAns (depending on MAC command)
   * Here we focus only in calculated delayed reward per arm, and we could try different rewards functions.
   *
   *
   */
  void ConsolidateRewardsIntoBandit();


  /**
   * @brief This function stores the used arms (even if we do not have feedback yet).
   * It also sets the internal boolean m_banditNeedsStats that determines if we need bandit feedback or not.
   * The strategy of the feedback request frequency is determined here.
   *
   * @param armNumber The arm that was played
   * @param frameCnt The frame count of the packet that was sent using this arm configuration
   */
  void UpdateUsedArm(size_t armNumber, int frameCnt);

  void CleanArmsStats();



//protected:
  Ptr<AdrBanditAgent> m_adrBanditAgent; // Pointer to the agent to update the Rewards



  bool m_waitingForStats = false;
  std::string printArmsAndRewardsVector();

protected:

  /**
   * @brief This function determines the strategy to ask for delayed feedback.
   * E.g., first N frames never ask, and then only ask with p=0.1
   *
   * @param frameCnt the Frame number, the strategy will depend on the frame.
   */
  void setBanditNeedStats (int frameCnt);

  bool m_banditNeedsStats = true;



  // Todo: Clean the logic  of  this FrmCounters ..
  int m_frmCntMinWithoutStats=1;
  int m_frmCntMaxWithoutStats=0;

  int m_requestedMaxFrmCntReward=0;

  //int m_frmCntDeltaMin=0;
  //int m_currentFrmCntReward=0;


  static const int HARDCODED_NUMBER_ARMS = 6 ; // TODO: fix this with to dynamically allocate the "m_armsAndRewardsVector" attribute on InitBanditAgentAndArms(Ptr<AdrBanditAgent> adrBanditAgent)


  typedef std::tuple <int, int, double, double> arm_stats; // <packets sent, packets rcv, Packet Delivery Ratio (raw reward), reward scaling factor>
  std::vector<arm_stats> m_armsAndRewardsVector;

  // Static/shared members (Maybe not a good idea... maybe a global static object is better?)
  // https://stackoverflow.com/questions/3409428/putting-class-static-members-definition-into-cpp-file-technical-limitation
  // https://stackoverflow.com/questions/46874055/why-is-inline-required-on-static-inline-variables
  static inline std::default_random_engine generator; /* This makes the simulation deterministic if all inputs are the same TODO: see if we want better randomness use other RNG */
  static inline std::bernoulli_distribution bernoulliDistribution{ banditConstants::pAskingForFeedback }; // p of asking for STATS


};


}
}

#endif /* SRC_LORAWAN_MODEL_BANDITS_SRC_BANDIT_DELAYED_REWARD_HELPER__H */
