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

  NS_LOG_FUNCTION ("Packets received by this device (" << status->GetReceivedPacketList ().size () << ") !!!! " );
  NS_LOG_FUNCTION ("Last Packet Frame Count:  (" << status->GetLastReceivedPacketInfo().fCnt << ") OMG !!!!!!!!!!!!!!!!!!!" );

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

  uint8_t  frmToDelta   = banditRewardReq->GetFrameCountTo();
  uint16_t frmFromAbs = banditRewardReq->GetFrameCountFrom(); //std::bitset<16> (m_fCnt_from)
  uint16_t frmToAbs =  unsigned(frmFromAbs) + unsigned(frmToDelta); // Overflow Can happen..

  NS_LOG_FUNCTION ("MAC BanditRewardReq , Frame frmFromAbs" << frmFromAbs << " .... to frmToDelta : " << unsigned(frmToDelta) << " .... to frmToAbs : " << unsigned(frmToAbs));

  //  typedef std::list<std::pair<Ptr<Packet const>, ReceivedPacketInfo> >   ReceivedPacketList; //EndDeviceStatus::
  //Iteration inspired by "AdrComponent::GetMinSNR"

  uint8_t dr_rcv_packets[6] = {0,0,0,0,0,0};


  //Take elements from the list starting at the end
  auto it = packetList.rbegin ();
  uint16_t frmCurrentIt = (it->second.fCnt); // The max Fcount

  if (frmFromAbs == 0)
    frmFromAbs = 1; // FCnt = 0 does not exists, it starts at 1.

  while (frmFromAbs <= frmCurrentIt)
    {
      NS_LOG_FUNCTION("frmCurrentIt: " << frmCurrentIt);

      if ((frmFromAbs <= frmCurrentIt) &&  (frmCurrentIt <= frmToAbs))
	{
	  int SfToDR = (12 - unsigned (it->second.sf));
	  dr_rcv_packets[SfToDR]++;

	  NS_LOG_FUNCTION(
	      "dr_rcv_packets["<< SfToDR << "]: " << unsigned(dr_rcv_packets[SfToDR]));
	}

      if (frmFromAbs == frmCurrentIt) break;

      ++it;
      frmCurrentIt = (it->second.fCnt);
      NS_LOG_FUNCTION("frmNEXTCurrentIt: " << frmCurrentIt << "   frmFromAbs: " << frmFromAbs);


    }




  return CreateObject<BanditRewardAns> (dr_rcv_packets[0],dr_rcv_packets[1],
					dr_rcv_packets[2],dr_rcv_packets[3],
					dr_rcv_packets[4],dr_rcv_packets[5]);
}


}
}
