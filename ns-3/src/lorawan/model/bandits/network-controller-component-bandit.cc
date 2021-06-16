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

#include "network-controller-component-bandit.h"

namespace ns3 {
namespace lorawan {

////////////////////////////////////////
// BanditRewardReq commands management //
////////////////////////////////////////

NS_LOG_COMPONENT_DEFINE ("NetworkControllerComponentBandit");

NS_OBJECT_ENSURE_REGISTERED (NetworkControllerComponentBandit);

TypeId NetworkControllerComponentBandit::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NetworkControllerComponentBandit")
    .SetGroupName ("lorawan")
    .AddConstructor<NetworkControllerComponentBandit> ()
    .SetParent<NetworkControllerComponent> ()
    ;
  return tid;
}
NetworkControllerComponentBandit::NetworkControllerComponentBandit ()
{
  // TODO Auto-generated constructor stub

}

NetworkControllerComponentBandit::~NetworkControllerComponentBandit ()
{
  // TODO Auto-generated destructor stub
}


void
NetworkControllerComponentBandit::OnReceivedPacket (
    Ptr<const Packet> packet, Ptr<EndDeviceStatus> status,
    Ptr<NetworkStatus> networkStatus)
{
  NS_LOG_FUNCTION(this->GetTypeId () << packet << networkStatus);

  // We will only act just before reply, when all Gateways will have received
  // the packet, since we need their respective received power.
}

void
NetworkControllerComponentBandit::BeforeSendingReply (Ptr<EndDeviceStatus> status,
                                  Ptr<NetworkStatus> networkStatus)
{
  NS_LOG_FUNCTION (this << status << networkStatus);

  NS_LOG_FUNCTION ("Packets received by this device (" << status->GetReceivedPacketList ().size () << ") OMG !!" );



}

void
NetworkControllerComponentBandit::OnFailedReply (Ptr<EndDeviceStatus> status,
                                  Ptr<NetworkStatus> networkStatus)
{
  NS_LOG_FUNCTION (this->GetTypeId () << networkStatus);
}



}
}
