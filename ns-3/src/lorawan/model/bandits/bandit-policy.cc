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

#include "bandit-policy.h"
#include "ns3/log.h"


namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("BanditPolicy");
NS_OBJECT_ENSURE_REGISTERED (BanditPolicy);

TypeId
BanditPolicy::GetTypeId (void)
{
//TODO: see for constructor https://www.nsnam.org/docs/manual/html/attributes.html
  static TypeId tid =
      TypeId ("ns3::BanditPolicy").SetParent<Object> ().SetGroupName (
	  "lorawan")
	  //.AddConstructor<BanditPolicy> ()
	  //.AddAttribute ( // TODO!!
	  ;
  return tid;
}

BanditPolicy::BanditPolicy (const AIToolbox::Bandit::Experience &exp)
{
  NS_LOG_FUNCTION(this << "I am a BanditPolicy!");
  this->m_aiPolicy = new AIToolbox::Bandit::ThompsonSamplingPolicy (exp);
}

BanditPolicy::~BanditPolicy ()
{
  delete m_aiPolicy;
}

size_t
BanditPolicy::sampleAction ()
{

  return m_aiPolicy->sampleAction ();
}

AIToolbox::Vector
BanditPolicy::getPolicy () const
{
  return m_aiPolicy->getPolicy ();

}

} /* namespace lorawan */
} /* namespace ns3 */
