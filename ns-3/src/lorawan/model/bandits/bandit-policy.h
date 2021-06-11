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

#ifndef SRC_LORAWAN_MODEL_BANDITS_BANDIT_POLICY_H_
#define SRC_LORAWAN_MODEL_BANDITS_BANDIT_POLICY_H_

#include "ns3/object.h"
#include <AIToolbox/Bandit/Types.hpp>
#include <AIToolbox/Bandit/Policies/PolicyInterface.hpp>
#include <AIToolbox/Bandit/Policies/ThompsonSamplingPolicy.hpp>


using namespace AIToolbox::Bandit;

namespace ns3 {
namespace lorawan {

class BanditPolicy : public Object
{
public:
  static TypeId
  GetTypeId (void);

  BanditPolicy (const AIToolbox::Bandit::Experience &exp);
  virtual
  ~BanditPolicy ();

  // chooseArm 	  --> arm
  // chooseNArms(N) --> arm1, arm1, arm2

  // updateArm(arm, reward)
  // 		updateArm (vector (arm, reward))

  size_t
  sampleAction ();
  AIToolbox::Vector
  getPolicy () const;

protected:
  // vector of arms, and associated stuff (reward discounts, etc)
  AIToolbox::Bandit::ThompsonSamplingPolicy*  m_aiPolicy;
  //AIToolbox::Bandit::PolicyInterface* m_aiPolicy;

};

} /* namespace lorawan */
} /* namespace ns3 */

#endif /* SRC_LORAWAN_MODEL_BANDITS_BANDIT_POLICY_H_ */
