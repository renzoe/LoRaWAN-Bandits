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

#include "ns3/adr-bandit-agent.h"
#include "ns3/log.h"
#include <boost/multi_array.hpp>
//#include <Eigen/Core>
#include <eigen3/Eigen/Core>
//#include <AIToolbox/Bandit/Types.hpp>
#include <AIToolbox/Bandit/Policies/ThompsonSamplingPolicy.hpp>
#include <AIToolbox/Bandit/Experience.hpp>

using namespace AIToolbox::Bandit;


namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("ADRBanditAgent");
NS_OBJECT_ENSURE_REGISTERED (AdrBanditAgent);



TypeId
AdrBanditAgent::GetTypeId (void)
{
//TODO: see for constructor https://www.nsnam.org/docs/manual/html/attributes.html
  static TypeId tid =
      TypeId ("ns3::ADRBanditAgent").SetParent<Object> ().SetGroupName (
	  "lorawan").AddConstructor<AdrBanditAgent> ();
  return tid;
}

const int NUMER_OF_ARMS = 6;

AdrBanditAgent::AdrBanditAgent ():
    m_experience(NUMER_OF_ARMS) // https://stackoverflow.com/questions/12927169/how-can-i-initialize-c-object-member-variables-in-the-constructor
{

  NS_LOG_FUNCTION(this << "I am a ADRBanditAgent!");

  //this->m_experience = new Experience ((size_t) NUMER_OF_ARMS);

  //this->policy = Create<BanditPolicy> (*(this->experience)); // TODO work with ns-3 constructors that take input params
  //this->m_aiPolicy = new AIToolbox::Bandit::ThompsonSamplingPolicy (*(this->m_experience));
  this->m_aiPolicy = new AIToolbox::Bandit::ThompsonSamplingPolicy (m_experience);


//Bootstrap 50%
  for (int i = 0; i < NUMER_OF_ARMS; i++)
    {
      m_experience.record (i, 1);
      m_experience.record (i, 0);
    }

//  m_experience.record (0, 0);
//  m_experience.record (0, 0);
//  m_experience.record (0, 0);
//  m_experience.record (0, 1);
//  m_experience.record (1, 1);
//  m_experience.record (1, 1);
//  m_experience.record (1, 1);
//  m_experience.record (1, 0);

//  NS_LOG_FUNCTION(
//      "getRewardMatrix:" << "\n" << m_experience.getRewardMatrix());
//  NS_LOG_FUNCTION("getM2Matrix:" << "\n" << m_experience.getM2Matrix());
//
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//  NS_LOG_FUNCTION("sampleAction:" << m_aiPolicy->sampleAction());
//
//  NS_LOG_FUNCTION("getPolicy:" << m_aiPolicy->getPolicy());
//
//
//  NS_LOG_FUNCTION(m_experience.getTimesteps ());
//  NS_LOG_FUNCTION(m_experience.getVisitsTable ());

//  NS_LOG_FUNCTION ( "getRewardMatrix:" << "\n" << experience->getRewardMatrix());
//  NS_LOG_FUNCTION ( "getM2Matrix:" << "\n" <<  experience->getM2Matrix());

}

AdrBanditAgent::~AdrBanditAgent ()
{
  //delete this->m_experience;
  delete this->m_aiPolicy;
}

void
AdrBanditAgent::UpdateReward (size_t arm_number, double reward)
{
  m_experience.record (arm_number, reward);
}

size_t
AdrBanditAgent::ChooseArm ()
{
  NS_LOG_FUNCTION ( "getRewardMatrix:" << "\n" << m_experience.getRewardMatrix());
  NS_LOG_FUNCTION ( "getM2Matrix (Variance):" << "\n" <<  m_experience.getM2Matrix());
  NS_LOG_FUNCTION ( "getPolicy:" << "\n" <<  m_aiPolicy->getPolicy());




  return m_aiPolicy->sampleAction();
}

size_t
AdrBanditAgent::GetNumberOfArms ()
{
  return NUMER_OF_ARMS;
}

} /* namespace lorawan */
} /* namespace ns3 */
