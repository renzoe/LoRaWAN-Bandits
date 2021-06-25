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
  this->m_adrBanditAgent = Create<AdrBanditAgent> ();




  //[Renzo] I am doing a shorcut to have the pointer in the  m_adrBanditRewardHelper TODO: proper constructor/encapsulation
  Ptr<BanditDelayedRewardIntelligence> tmp = Create<BanditDelayedRewardIntelligence> ();

  tmp->m_adrBanditAgent = this->m_adrBanditAgent;
  this->m_banditDelayedRewardIntelligence = tmp;

  //(this->m_banditDelayedRewardHelper)->m_adrBanditAgent = this->m_adrBanditAgent; // Does not work for some reason..




}

ClassAEndDeviceLorawanMacBandit::~ClassAEndDeviceLorawanMacBandit ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

/////////////////////
// Sending methods //
/////////////////////

void
ClassAEndDeviceLorawanMacBandit::DoSendBeforeApplyNecessaryOptions (Ptr<Packet> packet)
{
  //This function only called one new packets, not on re
  // Colored Terminal: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  NS_LOG_INFO("\033[1;31m");

  NS_LOG_INFO("DoSendBeforeApplyNecessaryOptions");

  //***************************************************************
    //[Renzo] BANDIT chooses next m_dataRate
    m_dataRate = this->m_adrBanditAgent->ChooseArm();
    //m_dataRate = 4 ; // Debugging with SF7 to create lost frames

    m_banditDelayedRewardIntelligence->UpdateUsedArm(m_dataRate, this->m_currentFCnt);


    NS_LOG_INFO ("Bandit chosen DR!:" << unsigned(m_dataRate));
    //renzo TODO: until implementing better feedback, I express that I used this arm, and this has a cost; I don't know if It will be rewarded (should compensate)

    //NS_LOG_INFO ("Bandit chosen DR COST!:" << cost_for_arm[m_dataRate]);
    //this->m_adrBanditAgent->UpdateReward(m_dataRate, cost_for_arm[m_dataRate]);

    // The message tuple was applied to the header before EndDeviceLorawanMac:ApplyNecessaryOptions(), I force here a CONFIRMED_DATA_UP message. This code was for Bandit without custom MAC command implementation
    /*NS_LOG_INFO ("Bandit type m_mType:" << unsigned(this->GetMType()));
    this->SetMType(ns3::lorawan::LorawanMacHeader::CONFIRMED_DATA_UP);
    NS_LOG_INFO ("Bandit type m_mType:" << unsigned(this->GetMType()));*/



    /* TODO fix this logic --> FIXED . This prevented a bug, where  a MAC command gets added multiple times . Proper solution: Add directly to the frame header at higher logic chain to ::SendToPhy  (::Send or ::DoSend) --> DONE in DoSend*/
    NS_LOG_INFO("About to check if Retransmitting an old packet:  m_retransmitting_old_packet?: " << (m_retransmitting_old_packet));
    //m_retransmitting_old_packet = false; // this is to replicate the bug, if we always add the MacCommand it will re-add to retransmissions eventually making the frame too big.

    if (!m_retransmitting_old_packet) /* if this is NOT a retransmission. See EndDeviceLorawanMac::DoSend (Ptr<Packet> packet) for retransmission logic */
      { /* Because if this is a retransmission we will ad N times the MAC command.*/

        NS_LOG_INFO( "this is NOT a Retransmission !! we add the MAC command ");

        uint16_t currentFrame = (this->m_currentFCnt); // unsigned (0)

        if(m_banditDelayedRewardIntelligence->isGetRewardsMacCommandReqNeeded())
          {
  	  Ptr<BanditRewardReq> req = m_banditDelayedRewardIntelligence->GetRewardsMacCommandReq (currentFrame);
  	  this->AddMacCommand (req);
          }

      }


    //***************************************************************



  NS_LOG_INFO("\033[0m");
}

void
ClassAEndDeviceLorawanMacBandit::SendToPhy (Ptr<Packet> packetToSend)
{
  /////////////////////////////////////////////////////////
  // Add headers, prepare TX parameters and send the packet
  /////////////////////////////////////////////////////////

  NS_LOG_DEBUG ("PacketToSend: " << packetToSend << "\t\tBandit!!!!!!!!!!!!!!");

  // Data Rate Adaptation as in LoRaWAN specification, V1.0.2 (2016) --> ADR Backoff
  m_enableDRAdapt = false; // Renzo: we disable it for bandits!

  if (m_enableDRAdapt && (m_dataRate > 0)
      && (m_retxParams.retxLeft < m_maxNumbTx)
      && (m_retxParams.retxLeft % 2 == 0) )
    {
      m_txPower = 14; // Reset transmission power
      m_dataRate = m_dataRate - 1;
    }

  // Craft LoraTxParameters object
  LoraTxParameters params;

  //********Begin Renzo ******************************
  //m_dataRate is already chosen by the bandit logic on DoSendBeforeApplyNecessaryOptions.
  // Before, I added the logic here but the MAC command can not be added (..cleanly) to the current packetToSend
  //********End Renzo******************************

  params.sf = GetSfFromDataRate (m_dataRate);
  params.headerDisabled = m_headerDisabled;
  params.codingRate = m_codingRate;
  params.bandwidthHz = GetBandwidthFromDataRate (m_dataRate);
  params.nPreamble = m_nPreambleSymbols;
  params.crcEnabled = 1;
  params.lowDataRateOptimizationEnabled = 0;

  // Wake up PHY layer and directly send the packet

  /* RENZO: Here we randomize channel/Frequency, TODO: advanced bandit may not use this randomization and chose its own values
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
ClassAEndDeviceLorawanMacBandit::BanditDelayedFeedbackUpdateOLD (
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
  this->m_adrBanditAgent->UpdateReward (lDR, reward_for_arm[lDR]);

}

void
ClassAEndDeviceLorawanMacBandit::BanditDelayedFeedbackUpdate (
    Ptr<BanditRewardAns> delayedRewards)
{

  // Colored Terminal: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  NS_LOG_FUNCTION("\033[1;32m");
  NS_LOG_FUNCTION("delayedRewards->GetDataRateStatistics(): "<<delayedRewards->GetDataRateStatistics());

  //std::vector<int> drStatistics = delayedRewards->GetDataRateStatistics();
  m_banditDelayedRewardIntelligence->UpdateRewardsAns(delayedRewards); // (this->m_currentFCnt)


  //NS_LOG_FUNCTION("m_banditDelayedRewardIntelligence->printArmsAndRewardsVector():\n"<< m_banditDelayedRewardIntelligence->printArmsAndRewardsVector());



  NS_LOG_FUNCTION("\033[0m");
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


          // Parse the MAC commands (will include a call to OnBanditRewardAns if the MAC is present)
          ParseCommands (fHdr);


          // TODO Pass the packet up to the NetDevice


          // Call the trace source
          m_receivedPacket (packet);

          // RENZO Also... update bandit with the delayed Feedback (Old logic was here, now we have the MAC command parsed)
	  //BanditDelayedFeedbackUpdateOLD (packetCopy);

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
  NS_LOG_FUNCTION ("\t\t OnLinkAdrReq! But I am a bandit and I do *not* Care !!!!");

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
ClassAEndDeviceLorawanMacBandit::OnBanditRewardAns (Ptr<MacCommand> banditRewardAns)
{
  NS_LOG_FUNCTION (this << banditRewardAns);



  Ptr<BanditRewardAns> delayedRewards = DynamicCast<BanditRewardAns>(banditRewardAns); // I could have used GetObject<> (); See "downcasting" https://www.nsnam.org/docs/manual/html/object-model.html

  NS_LOG_FUNCTION ("OnBanditRewardAns! I like this MAC command, It has my rewards :D !!!  " << delayedRewards->GetDataRateStatistics());

  //m_banditDelayedRewardIntelligence->UpdateRewardsAns (delayedRewards); // Put the logic on function BanditDelayedFeedbackUpdate(delayedRewards);:
  BanditDelayedFeedbackUpdate(delayedRewards);



}




} /* namespace lorawan */
} /* namespace ns3 */
