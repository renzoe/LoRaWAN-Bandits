/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021
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
 * Author: Renzo Navas <renzo.navas@inria.fr>
 */

#ifndef CLASS_A_END_DEVICE_LORAWAN_MAC_BANDIT_H
#define CLASS_A_END_DEVICE_LORAWAN_MAC_BANDIT_H


#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/adr-bandit-agent.h"


namespace ns3 {
namespace lorawan {

/**
 * Class representing the MAC layer of a Class A LoRaWAN device, with added Bandit behaviour ADR, suported at MAC
 * https://stackoverflow.com/questions/29408896/the-protected-and-private-member-variables-in-c-inheritance
 */
class ClassAEndDeviceLorawanMacBandit : public ClassAEndDeviceLorawanMac
{
public:
  static TypeId GetTypeId (void);

  ClassAEndDeviceLorawanMacBandit (); // TODO inherit the constructors
  virtual ~ClassAEndDeviceLorawanMacBandit (); // TODO inherit the destructor



  /////////////////////
  // Sending methods //
  /////////////////////
  /**
  * Add headers and send a packet with the sending function of the physical layer.
  *
  * \param packet the packet to send
  */
  virtual void SendToPhy (Ptr<Packet> packet);

  //////////////////////////
  //  Receiving methods   //
  //////////////////////////

  /**
   * Receive a packet.
   *
   * This method is typically registered as a callback in the underlying PHY
   * layer so that it's called when a packet is going up the stack.
   *
   * \param packet the received packet.
   */
  virtual void Receive (Ptr<Packet const> packet);

  /////////////////////////
  // Inherited from  EndDeviceLorawanMac, but we want to override (I made it "virtual" so I can override it)
  /////////////////////////
  /**
   * Perform the actions that need to be taken when receiving a LinkAdrReq command.
   * For the bandit case: we will ignore the command, as we define our own ADR policy.
   *
   * \param dataRate The data rate value of the command.
   * \param txPower The transmission power value of the command.
   * \param enabledChannels A list of the enabled channels.
   * \param repetitions The number of repetitions prescribed by the command.
   */
   virtual void OnLinkAdrReq (uint8_t dataRate, uint8_t txPower,
                     std::list<int> enabledChannels, int repetitions);

  /////////////////////////
  // NEW Bandit MAC command methods //
  /////////////////////////

  /**
   * Perform the actions that need to be taken when receiving a BanditRewardAns MAC
   * command based on the Device's Class Type. (Here we process it)
   *
   * \param banditRewardAns The <BanditRewardAns> MAC command but not casted yet (<MacCommand>) to avoid dependencies
   */
  virtual void OnBanditRewardAns (Ptr<MacCommand> banditRewardAns);

protected:

      Ptr<AdrBanditAgent> adr_bandit_agent;
      void BanditDelayedFeedbackUpdate (const Ptr<Packet> &packetCopy);


private:



}; /* ClassAEndDeviceLorawanMacBandit */
} /* namespace lorawan */
} /* namespace ns3 */
#endif /* CLASS_A_END_DEVICE_LORAWAN_MAC_BANDIT_H */
