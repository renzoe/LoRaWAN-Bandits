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

  // Colored Terminal: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  NS_LOG_FUNCTION ("\033[1;33m");
  NS_LOG_FUNCTION ("\033[1;33m"<<"Packets received by this device (" << status->GetReceivedPacketList ().size () << ") !!!! "<< "\033[0m");
  NS_LOG_FUNCTION ("\033[1;33m"<<"Last Packet Frame Count:  (" << status->GetLastReceivedPacketInfo().fCnt << ") OMG !!!!!!!!!!!!!!!!!!!"<< "\033[0m");
  NS_LOG_FUNCTION ("\033[0m");

  // See: void AdrComponent::BeforeSendingReply, I inspired from there the packet/reply threatment.

  Ptr<Packet> myPacket = status->GetLastPacketReceivedFromDevice ()->Copy ();

  LorawanMacHeader mHdr;
  LoraFrameHeader fHdr;
  fHdr.SetAsUplink (); // [Renzo] why they do this?
  myPacket->RemoveHeader (mHdr);
  myPacket->RemoveHeader (fHdr);


  // Look for the BanditRewardReq , and create a BanditRewardAns
  // See  void   EndDeviceLorawanMac::ParseCommands (LoraFrameHeader frameHeader) , I inspired from there
  std::list<Ptr<MacCommand> > commands =  fHdr.GetCommands ();
  std::list<Ptr<MacCommand> >::iterator it;
  for (it = commands.begin (); it != commands.end (); it++)
    {
      NS_LOG_DEBUG ("Iterating over the MAC commands...");
      enum MacCommandType type = (*it)->GetCommandType ();
      switch (type)
        {
        case (BANDIT_REWARD_REQ):
          {
            NS_LOG_DEBUG ("Detected a BanditRewardReq command.");

            // Cast the command
            Ptr<BanditRewardReq> banditRewardReq = (*it)->GetObject<BanditRewardReq> ();
            // Call the appropriate function to take action   //OnBanditRewardReq (banditRewardReq);
            /*uint16_t from = banditRewardReq->GetFrameCountFrom(); //std::bitset<16> (m_fCnt_from)
            uint8_t  to   = banditRewardReq->GetFrameCountTo();
            NS_LOG_FUNCTION ("MAC BanditRewardReq , Frame From" << from << " .... to : " << unsigned(to) );*/

            EndDeviceStatus::ReceivedPacketList rcvPacketsList = status->GetReceivedPacketList ();


            Ptr<BanditRewardAns> banditRewardAns = GetBanditRewardAns(banditRewardReq, rcvPacketsList);

            status->m_reply.frameHeader.AddCommand(banditRewardAns) ;
	    status->m_reply.frameHeader.SetAsDownlink ();
	    status->m_reply.macHeader.SetMType (LorawanMacHeader::UNCONFIRMED_DATA_DOWN);

	    status->m_reply.needsReply = true; /* [Renzo] This is needed to force this downlink packet, if not  a Downlink is not sent ! */


            break;
          }
	default:
	  {
	    //NS_LOG_DEBUG("NO  BANDIT_REWARD_REQ present, we do nothing");
	    break;
	  }
	}
    }
}

void
NetworkControllerComponentBandit::OnFailedReply (Ptr<EndDeviceStatus> status,
                                  Ptr<NetworkStatus> networkStatus)
{
  NS_LOG_FUNCTION (this->GetTypeId () << networkStatus);
}


Ptr<BanditRewardAns>
NetworkControllerComponentBandit::GetBanditRewardAns (
    Ptr<BanditRewardReq> banditRewardReq,
    EndDeviceStatus::ReceivedPacketList packetList)
{

  uint16_t frmCntMaxAbs   = banditRewardReq->GetFrameCountMax(); //std::bitset<16> (m_fCnt_from)
  uint8_t  frmCntDeltaMin = banditRewardReq->GetFrameCountDeltaMin();

  uint16_t frmCntMinAbs =  unsigned(frmCntMaxAbs) - unsigned(frmCntDeltaMin);

 // Colored Terminal: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  NS_LOG_FUNCTION ("\033[1;33m");
  NS_LOG_FUNCTION ("MAC BanditRewardReq , Frame frmCntMinAbs" << frmCntMinAbs << " .... to frmCntDeltaMin : " << unsigned(frmCntDeltaMin) << " .... to frmCntMaxAbs : " << unsigned(frmCntMaxAbs));

  //  typedef std::list<std::pair<Ptr<Packet const>, ReceivedPacketInfo> >   ReceivedPacketList; //EndDeviceStatus::
  //Iteration inspired by "AdrComponent::GetMinSNR"

  uint8_t dr_rcv_packets[6] = {0,0,0,0,0,0};


  //Take elements from the list starting at the end
  auto it = packetList.rbegin ();
  uint16_t frmCurrentIt = (it->second.fCnt); // The max Fcount .

  if (frmCntMinAbs == 0)
    frmCntMinAbs = 1; // FCnt = 0 does not exists, it starts at 1.

  while (frmCntMinAbs <= frmCurrentIt)
    {
      NS_LOG_FUNCTION("frmCurrentIt: " << frmCurrentIt);

      if ((frmCntMinAbs <= frmCurrentIt) &&  (frmCurrentIt <= frmCntMaxAbs)) // (frmCntMinAbs <= frmCurrentIt) redundant because its in the while condition
	{
	  int SfToDR = (12 - unsigned (it->second.sf)); // number 12 convers from SF to DR ... TODO use proper function
	  dr_rcv_packets[SfToDR]++;

	  NS_LOG_FUNCTION(
	      "dr_rcv_packets["<< SfToDR << "]: " << unsigned(dr_rcv_packets[SfToDR]));
	}

      if (frmCntMinAbs == frmCurrentIt) break;

      ++it;

      frmCurrentIt = (it->second.fCnt);
      NS_LOG_FUNCTION("frmNEXTCurrentIt: " << frmCurrentIt << "   frmCntMinAbs: " << frmCntMinAbs);

      if (it == packetList.rend ()) // [RENZO] Very important to iterate properly.. this whole function could use a refactor.
	{
	  NS_LOG_FUNCTION("We prevented the crazy bug!!! \a ");
	  break;
	} // A bug could happen if lost frames happened at the beginning, and the frame count was high: We ended up in an infinite loop at




    } // The logic I use could be better... I had infinite loop issues with the number Zero, also because it is "unsigned" and we are not using a proper "counter", but real values of  fCnt.
      // All this logic could be done cleaner (also, in current implementation issues can happen in out of order messages -early exit loop ...-).

  NS_LOG_FUNCTION ("\033[0m");



  return CreateObject<BanditRewardAns> (dr_rcv_packets[0],dr_rcv_packets[1],
					dr_rcv_packets[2],dr_rcv_packets[3],
					dr_rcv_packets[4],dr_rcv_packets[5]);
}


}
}
