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

#ifndef SRC_LORAWAN_MODEL_BANDITS_NETWORK_CONTROLLER_COMPONENT_BANDIT_H_
#define SRC_LORAWAN_MODEL_BANDITS_NETWORK_CONTROLLER_COMPONENT_BANDIT_H_


#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/network-status.h"
#include "ns3/network-controller-components.h"

namespace ns3 {
namespace lorawan {

////////////////////////////////////////
// BanditRewardReq commands management //
////////////////////////////////////////

class NetworkControllerComponentBandit : public ns3::lorawan::NetworkControllerComponent
{
public:
  static TypeId GetTypeId (void);

  //Constructor
  NetworkControllerComponentBandit ();

  //Destructor
  virtual ~NetworkControllerComponentBandit ();

  void OnReceivedPacket (Ptr<const Packet> packet,
                         Ptr<EndDeviceStatus> status,
                         Ptr<NetworkStatus> networkStatus);

  void BeforeSendingReply (Ptr<EndDeviceStatus> status,
                           Ptr<NetworkStatus> networkStatus);

  void OnFailedReply (Ptr<EndDeviceStatus> status,
                      Ptr<NetworkStatus> networkStatus);
private:

};


}
}

#endif /* SRC_LORAWAN_MODEL_BANDITS_NETWORK_CONTROLLER_COMPONENT_BANDIT_H_ */
