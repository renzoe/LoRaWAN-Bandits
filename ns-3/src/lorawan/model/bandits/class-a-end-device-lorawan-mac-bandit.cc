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

//#include "ns3/class-a-end-device-lorawan-mac-bandit.h"
#include "ns3/class-a-end-device-lorawan-mac-bandit.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/log.h"

#include "ns3/lora-tag.h"

#include <algorithm>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("ClassAEndDeviceLorawanMacBandit");
NS_OBJECT_ENSURE_REGISTERED (ClassAEndDeviceLorawanMacBandit);

// BEGIN Auxiliary functions
/************************
 * @brief Auxiliary function to conver tfrom SF to DR in the EU band (DR from 0 to 5)
 *
 * @param sf
 * @return DR EU region
 */
uint8_t
GetDataRateFromSfEU (uint8_t sf)
{
  return (sf-12)*-1; // cheap trick I need to convert SF to DR in EU region, does not work for DR 6 (that is also SF7), will map to DR5
}

//                    DR =   0    1   2  3   4   5
//                    SF =  12   11  10  9   8   7
double cost_for_arm[]   = {-32,-16, -8, -4, -2, -1};
//double reward_for_arm[] = { 32, 16,  8,  4,  2,  1};
double reward_for_arm[] = { 33, 18,  12,  12,  18,  33};

uint8_t
GetCostForARM (uint8_t arm)
{
 return  cost_for_arm[arm];

}

//uint8_t
//GetRewardForARM (uint8_t arm)
//{
//return 0
//}
// END Auxiliary functions

TypeId
ClassAEndDeviceLorawanMacBandit::GetTypeId (void)
{
static TypeId tid = TypeId ("ns3::ClassAEndDeviceLorawanMacBandit")
  .SetParent<ClassAEndDeviceLorawanMac> ()
  .SetGroupName ("lorawan")
  .AddConstructor<ClassAEndDeviceLorawanMacBandit> ();
return tid;
}

ClassAEndDeviceLorawanMacBandit::ClassAEndDeviceLorawanMacBandit () //:
		//ClassAEndDeviceLorawanMac() // It is already called
{
  NS_LOG_FUNCTION (this  <<  "I am a bandit" );
  this->adr_bandit_agent = Create<AdrBanditAgent> ();

}

ClassAEndDeviceLorawanMacBandit::~ClassAEndDeviceLorawanMacBandit ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

/////////////////////
// Sending methods //
/////////////////////

void
ClassAEndDeviceLorawanMacBandit::SendToPhy (Ptr<Packet> packetToSend)
{
  /////////////////////////////////////////////////////////
  // Add headers, prepare TX parameters and send the packet
  /////////////////////////////////////////////////////////

  NS_LOG_DEBUG ("PacketToSend: " << packetToSend << "\t\tBandit!!!!!!!!!!!!!!");

  // Data Rate Adaptation as in LoRaWAN specification, V1.0.2 (2016) --> ADR Backoff
  // Renzo: we disable it for bandits:
  m_enableDRAdapt = false;
  if (m_enableDRAdapt && (m_dataRate > 0)
      && (m_retxParams.retxLeft < m_maxNumbTx)
      && (m_retxParams.retxLeft % 2 == 0) )
    {
      m_txPower = 14; // Reset transmission power
      m_dataRate = m_dataRate - 1;
    }

  // Craft LoraTxParameters object
  LoraTxParameters params;

  //***************************************************************
  //Renzo BANDIT chooses next m_dataRate
  m_dataRate = this->adr_bandit_agent->ChooseArm();
  //m_dataRate = 5 ; // Debugging with SF7 to create lost frames

  NS_LOG_INFO ("Bandit chosen DR!:" << unsigned(m_dataRate));

  //renzo TODO: until implementing bettter feedback, I express that I used this arm and had a cost, I don't know if It will be rewarded (should compensate)
  NS_LOG_INFO ("Bandit chosen DR COST!:" << cost_for_arm[m_dataRate]);
  this->adr_bandit_agent->UpdateReward(m_dataRate, cost_for_arm[m_dataRate]);

  // this was applied to the header before EndDeviceLorawanMac:ApplyNecessaryOptions()
  NS_LOG_INFO ("Bandit type m_mType:" << unsigned(this->GetMType()));
  this->SetMType(ns3::lorawan::LorawanMacHeader::CONFIRMED_DATA_UP);
  NS_LOG_INFO ("Bandit type m_mType:" << unsigned(this->GetMType()));



  /* TODO fix this logic. This prevent a bug, where  mac command gets added multiple times . Proper solution: Add directly to the frame header at higher layer thank SendToPhy */
  NS_LOG_INFO("About to check if Retranssmiting an old packet:  m_retransmitting_old_packet? \t\t\t\t\t!!!!!!!!!!! : " << (m_retransmitting_old_packet));
  //m_retransmitting_old_packet = false; // this is to replicate the bug, if we always add the MacCommand it re-add to retransmittions
  if (!m_retransmitting_old_packet) /* if this is NOT a retransmission. See EndDeviceLorawanMac::DoSend (Ptr<Packet> packet) for retransmission logic */
    { /* Because if this is a restransmission we will ad N times the MAC command. This mac command logic should be done at higer layers if done properly .... */
      NS_LOG_INFO(
	  "this is NOT a Retransmission !! we add the MAC command ");
      this->AddMacCommand (
	  Create<BanditRewardReq> (unsigned (0), unsigned (100))); // This MAC command will be added to the NEXT packet! TODO: add to current packet at higher layer logic (before SendToPhy and DoSend)
      //packetToSend->frameHeader.AddCommand (command); // or  Add directly to the frame?

    }


  //(In this alpha version of bandit we magically will update the results from the network server.)
  //packetToSend
  //LoraFrameHeader frameHdr;
  //frameHdr.SetAck (true);

  //***************************************************************


  params.sf = GetSfFromDataRate (m_dataRate);
  params.headerDisabled = m_headerDisabled;
  params.codingRate = m_codingRate;
  params.bandwidthHz = GetBandwidthFromDataRate (m_dataRate);
  params.nPreamble = m_nPreambleSymbols;
  params.crcEnabled = 1;
  params.lowDataRateOptimizationEnabled = 0;

  // Wake up PHY layer and directly send the packet

  /* RENZO: Here we randomize channel/Frequency, TODO: advanced bandit may not use this randomization
   * but the frequency will be another arm/config parameter */
  Ptr<LogicalLoraChannel> txChannel = GetChannelForTx ();

  NS_LOG_DEBUG ("PacketToSend: " << packetToSend);
  m_phy->Send (packetToSend, params, txChannel->GetFrequency (), m_txPower);

  //////////////////////////////////////////////
  // Register packet transmission for duty cycle
  //////////////////////////////////////////////

  // Compute packet duration
  Time duration = m_phy->GetOnAirTime (packetToSend, params);

  // Register the sent packet into the DutyCycleHelper
  m_channelHelper.AddEvent (duration, txChannel);

  //////////////////////////////
  // Prepare for the downlink //
  //////////////////////////////

  // Switch the PHY to the channel so that it will listen here for downlink
  m_phy->GetObject<EndDeviceLoraPhy> ()->SetFrequency (txChannel->GetFrequency ());

  // Instruct the PHY on the right Spreading Factor to listen for during the window
  // create a SetReplyDataRate function?
  uint8_t replyDataRate = GetFirstReceiveWindowDataRate ();
  NS_LOG_DEBUG ("m_dataRate: " << unsigned (m_dataRate) <<
                ", m_rx1DrOffset: " << unsigned (m_rx1DrOffset) <<
                ", replyDataRate: " << unsigned (replyDataRate) << ".");

  m_phy->GetObject<EndDeviceLoraPhy> ()->SetSpreadingFactor
    (GetSfFromDataRate (replyDataRate));


}

/**
 * @brief Class to implement the bandit delayed feedback learning
 *
 * @param packetCopy TODO: for now we only extract SF info an assume a reward for this DL SF
 */
void
ClassAEndDeviceLorawanMacBandit::BanditDelayedFeedbackUpdate (
    const Ptr<Packet> &packetCopy)
{
  // RENZO Also... update bandit

  LoraTag packetLoRaTag; // We are missing Power and SNR information. --> Suggestion Define our own Packet Tag appropiate for LoraTapPcaHeader
  packetCopy->RemovePacketTag (packetLoRaTag); // https://www.nsnam.org/docs/models/html/packets.html?highlight=packet
  uint8_t lTSF = packetLoRaTag.GetSpreadingFactor ();

//  NS_LOG_INFO(
//      "\n\t\t\tThe SF of received msg will feed bandit: '"<< unsigned(lTSF)<<"' !!!!!!!!!");
  //We need to translate SF to DR, the inverse of GetSfFromDataRate (m_dataRate);
  uint8_t lDR = GetDataRateFromSfEU (lTSF);
  NS_LOG_LOGIC(
      "\n\t\t\tThe DR of received msg will feed bandit: '"<< unsigned(lDR)<<"' !!!!!!!!!");

  NS_LOG_INFO ("Bandit chosen DR REWARD!:" << reward_for_arm[lDR]);
  this->adr_bandit_agent->UpdateReward (lDR, reward_for_arm[lDR]);

}

//////////////////////////
//  Receiving methods   //
//////////////////////////
void
ClassAEndDeviceLorawanMacBandit::Receive (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Work on a copy of the packet
  Ptr<Packet> packetCopy = packet->Copy ();

  // Remove the Mac Header to get some information
  LorawanMacHeader mHdr;
  packetCopy->RemoveHeader (mHdr);


  //NS_LOG_DEBUG ("Mac Header: " << mHdr);

  // Only keep analyzing the packet if it's downlink
  if (!mHdr.IsUplink ())
    {
      NS_LOG_INFO ("Found a downlink packet.");

      // Remove the Frame Header
      LoraFrameHeader fHdr;
      fHdr.SetAsDownlink ();
      packetCopy->RemoveHeader (fHdr);

      //Renzo Disable TODO
      //NS_LOG_DEBUG ("Frame Header: " << fHdr);

      // Determine whether this packet is for us
      bool messageForUs = (m_address == fHdr.GetAddress ());

      if (messageForUs)
        {
          NS_LOG_INFO ("The message is for us!");

          // If it exists, cancel the second receive window event
          // THIS WILL BE GetReceiveWindow()
          Simulator::Cancel (m_secondReceiveWindow);


          // Parse the MAC commands
          ParseCommands (fHdr);

          // TODO Pass the packet up to the NetDevice


          // Call the trace source
          m_receivedPacket (packet);

          // RENZO Also... update bandit with the delayed Feedback
	  BanditDelayedFeedbackUpdate (packetCopy);

          // Call the trace source for Pcap Sniffing
          m_receivedPacketRxSnifferTrace(packet);
        }
      else
        {
          NS_LOG_DEBUG ("The message is intended for another recipient.");

          // In this case, we are either receiving in the first receive window
          // and finishing reception inside the second one, or receiving a
          // packet in the second receive window and finding out, after the
          // fact, that the packet is not for us. In either case, if we no
          // longer have any retransmissions left, we declare failure.
          if (m_retxParams.waitingAck && m_secondReceiveWindow.IsExpired ())
            {
              if (m_retxParams.retxLeft == 0)
                {
                  uint8_t txs = m_maxNumbTx - (m_retxParams.retxLeft);
                  m_requiredTxCallback (txs, false, m_retxParams.firstAttempt, m_retxParams.packet);
                  NS_LOG_DEBUG ("Failure: no more retransmissions left. Used " << unsigned(txs) << " transmissions.");

                  // Reset retransmission parameters
                  resetRetransmissionParameters ();
                }
              else       // Reschedule
                {
                  this->Send (m_retxParams.packet);
                  NS_LOG_INFO ("We have " << unsigned(m_retxParams.retxLeft) << " retransmissions left: rescheduling transmission.");
                }
            }
        }
    }
  else if (m_retxParams.waitingAck && m_secondReceiveWindow.IsExpired ())
    {
      NS_LOG_INFO ("The packet we are receiving is in uplink.");
      if (m_retxParams.retxLeft > 0)
        {
          this->Send (m_retxParams.packet);
          NS_LOG_INFO ("We have " << unsigned(m_retxParams.retxLeft) << " retransmissions left: rescheduling transmission.");
        }
      else
        {
          uint8_t txs = m_maxNumbTx - (m_retxParams.retxLeft);
          m_requiredTxCallback (txs, false, m_retxParams.firstAttempt, m_retxParams.packet);
          NS_LOG_DEBUG ("Failure: no more retransmissions left. Used " << unsigned(txs) << " transmissions.");

          // Reset retransmission parameters
          resetRetransmissionParameters ();
        }
    }

  m_phy->GetObject<EndDeviceLoraPhy> ()->SwitchToSleep ();
}



/////////////////////////
// MAC command methods //
/////////////////////////

void
ClassAEndDeviceLorawanMacBandit::OnLinkAdrReq (uint8_t dataRate, uint8_t txPower,
                                std::list<int> enabledChannels, int repetitions)
{
  NS_LOG_FUNCTION (this << unsigned (dataRate) << unsigned (txPower) <<
                   repetitions);
  NS_LOG_FUNCTION ("\t\t OnLinkAdrReq! But I am a bandit and I do not Care !!!!");

  // Three bools for three requirements before setting things up
  bool channelMaskOk = true;
  bool dataRateOk = true;
  bool txPowerOk = true;

  channelMaskOk = dataRateOk = txPowerOk = false;

  // Craft a LinkAdrAns MAC command as a response
  ///////////////////////////////////////////////
  m_macCommandList.push_back (CreateObject<LinkAdrAns> (txPowerOk, dataRateOk,
                                                        channelMaskOk));


}

void
ClassAEndDeviceLorawanMacBandit::OnRxBanditStatsRes (Ptr<RxParamSetupReq> rxParamSetupReq)
{
  NS_LOG_FUNCTION (this << rxParamSetupReq);

//  bool offsetOk = true;
//  bool dataRateOk = true;
//
//  uint8_t rx1DrOffset = rxParamSetupReq->GetRx1DrOffset ();
//  uint8_t rx2DataRate = rxParamSetupReq->GetRx2DataRate ();
//  double frequency = rxParamSetupReq->GetFrequency ();
//
//  NS_LOG_FUNCTION (this << unsigned (rx1DrOffset) << unsigned (rx2DataRate) <<
//                   frequency);
//
//  // Check that the desired offset is valid
//  if ( !(0 <= rx1DrOffset && rx1DrOffset <= 5))
//    {
//      offsetOk = false;
//    }
//
//  // Check that the desired data rate is valid
//  if (GetSfFromDataRate (rx2DataRate) == 0
//      || GetBandwidthFromDataRate (rx2DataRate) == 0)
//    {
//      dataRateOk = false;
//    }
//
//  // For now, don't check for validity of frequency
//  m_secondReceiveWindowDataRate = rx2DataRate;
//  m_rx1DrOffset = rx1DrOffset;
//  m_secondReceiveWindowFrequency = frequency;
//
//  // Craft a RxParamSetupAns as response
//  NS_LOG_INFO ("Adding RxParamSetupAns reply");
//  m_macCommandList.push_back (CreateObject<RxParamSetupAns> (offsetOk,
//                                                             dataRateOk, true));

}




} /* namespace lorawan */
} /* namespace ns3 */
