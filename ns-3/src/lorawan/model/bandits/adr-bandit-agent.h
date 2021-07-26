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


/*
 * TODO :Starting to respect ns-3 coding style: https://www.nsnam.org/develop/contributing-code/coding-style/
 * */

#ifndef SRC_LORAWAN_MODEL_BANDITS_ADR_BANDIT_AGENT_H_
#define SRC_LORAWAN_MODEL_BANDITS_ADR_BANDIT_AGENT_H_

#include "ns3/object.h"
#include "ns3/bandit-policy.h"
#include <AIToolbox/Bandit/Experience.hpp>


using namespace AIToolbox::Bandit;

namespace ns3 {
namespace lorawan {

/**
 * @brief A class that implements a Bandit RL Agent targeted at Adaptive Data Rate LoRaWAN End Nodes
 *
 */
class AdrBanditAgent : public Object
{

public:
  static TypeId GetTypeId (void);

  AdrBanditAgent ();
  virtual ~AdrBanditAgent ();

  /**
   * @brief Get the Arm to use
   *
   * @return The chosen Arm
   */
  size_t  ChooseArm ();
   //size_t  chooseArmFromPolicyN(); If we have more than one policy!
  /**
   * @brief Low-level funciton to update a given arm reward
   *
   * @param armNumber
   * @param reward
   */
  void UpdateReward (size_t armNumber, double reward);


  /**
   * @brief Get the Numbers of Arms k
   *
   * @return The number of Arms (k)
   */
  size_t  GetNumberOfArms ();

  /* TODO we start with a 2-arm bandit: SF12 and SF9 */







protected:
  Experience      m_experience;
  PolicyInterface * m_aiPolicy;


  Ptr<BanditPolicy> m_policy; // Originally a Wrapper for AIToolbox::Bandit::PolicyInterface but probably we do not need it. (TODO: see)

  //void MySub (const T&);    // Method 1  (prefer this syntax)
  /*
   * https://stackoverflow.com/questions/6500313/why-should-c-programmers-minimize-use-of-new
   *   https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization
   *
   * https://en.cppreference.com/w/cpp/language/reference
   * https://www.geeksforgeeks.org/references-in-c/
   * */
  //

  //const Experience & exp_;

};

} /* namespace lorawan */
} /* namespace ns3 */

#endif /* SRC_LORAWAN_MODEL_BANDITS_ADR_BANDIT_AGENT_H_ */
