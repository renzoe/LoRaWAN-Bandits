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

#ifndef SRC_LORAWAN_MODEL_BANDITS_BANDIT_CONSTANTS_H_
#define SRC_LORAWAN_MODEL_BANDITS_BANDIT_CONSTANTS_H_

namespace ns3 {
namespace lorawan {

/*
 * From: https://www.learncpp.com/cpp-tutorial/sharing-global-constants-across-multiple-files-using-inline-variables/
 * For this to work we need C++17
 * (We already need it for AIToolbox and other static inline in bandit-delayed-reward-intelligence.h
 *
 */
namespace banditConstants
{

  inline constexpr int framesForBoostraping  = 15   ; // The number of frames before the bandit starts asking for feedback
  inline constexpr double pAskingForFeedback = 0.05 ; // p of asking for feedback (Bernoulli)

  /*                                      Rewards = {SF12, SF11, SF10, SF9 , SF8  , SF7 }   */
  inline constexpr double rewardsTOARatioFor32B[] = { 1  , 1.8 , 4.0 , 7.3 , 13.6 , 25.2} ; // Time on air ratio for 32Bytes (https://loratiiks.nk/#/airtime)
  inline constexpr double rewardsEnergySimple[]   = { 1  , 2   , 4   , 8   , 16   , 32  } ; // Naif energy pondered reward
  inline constexpr double rewardsPurePDR[]        = { 1  , 1   , 1   , 1   , 1    , 1   } ; // Naif pure PDR

  inline constexpr double rewardsFibonacci[]      = { 1  , 2   , 3   , 5   , 8    , 13  } ; // Fibonacci sequence



  inline constexpr const double * rewardsDefinition     =  rewardsFibonacci ; /* we need constexpr and const https://stackoverflow.com/questions/14116003/difference-between-constexpr-and-const */



} /* namespace banditConstants */


} /* namespace lorawan */
} /* namespace ns3 */

#endif /* SRC_LORAWAN_MODEL_BANDITS_BANDIT_CONSTANTS_H_ */
