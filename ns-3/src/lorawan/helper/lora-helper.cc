/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
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
 * Author: Davide Magrin <magrinda@dei.unipd.it>
 */

#include "ns3/lora-helper.h"
#include "ns3/log.h"
#include "ns3/loratap-pcap-header.h"

#include <fstream>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("LoraHelper");

  LoraHelper::LoraHelper () :
    m_lastPhyPerformanceUpdate (Seconds (0)),
    m_lastGlobalPerformanceUpdate (Seconds (0))
  {
  }

  LoraHelper::~LoraHelper ()
  {
  }

  NetDeviceContainer
  LoraHelper::Install ( const LoraPhyHelper &phyHelper,
                        const LorawanMacHelper &macHelper,
                        NodeContainer c) const
  {
    NS_LOG_FUNCTION_NOARGS ();

    NetDeviceContainer devices;

    // Go over the various nodes in which to install the NetDevice
    for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
      {
        Ptr<Node> node = *i;

        // Create the LoraNetDevice
        Ptr<LoraNetDevice> device = CreateObject<LoraNetDevice> ();

        // Create the PHY
        Ptr<LoraPhy> phy = phyHelper.Create (node, device);
        NS_ASSERT (phy != 0);
        device->SetPhy (phy);
        NS_LOG_DEBUG ("Done creating the PHY");

        // Connect Trace Sources if necessary
        if (m_packetTracker)
          {
            if (phyHelper.GetDeviceType () ==
                TypeId::LookupByName ("ns3::SimpleEndDeviceLoraPhy"))
              {
                phy->TraceConnectWithoutContext ("StartSending",
                                                 MakeCallback
                                                 (&LoraPacketTracker::TransmissionCallback,
                                                  m_packetTracker));
              }
            else if (phyHelper.GetDeviceType () ==
                     TypeId::LookupByName ("ns3::SimpleGatewayLoraPhy"))
            {
              phy->TraceConnectWithoutContext ("StartSending",
                                               MakeCallback
                                               (&LoraPacketTracker::TransmissionCallback,
                                                m_packetTracker));
              phy->TraceConnectWithoutContext ("ReceivedPacket",
                                               MakeCallback
                                                 (&LoraPacketTracker::PacketReceptionCallback,
                                                 m_packetTracker));
              phy->TraceConnectWithoutContext ("LostPacketBecauseInterference",
                                               MakeCallback
                                                 (&LoraPacketTracker::InterferenceCallback,
                                                 m_packetTracker));
              phy->TraceConnectWithoutContext ("LostPacketBecauseNoMoreReceivers",
                                               MakeCallback
                                                 (&LoraPacketTracker::NoMoreReceiversCallback,
                                                 m_packetTracker));
              phy->TraceConnectWithoutContext ("LostPacketBecauseUnderSensitivity",
                                               MakeCallback
                                                 (&LoraPacketTracker::UnderSensitivityCallback,
                                                 m_packetTracker));
              phy->TraceConnectWithoutContext ("NoReceptionBecauseTransmitting",
                                               MakeCallback
                                                 (&LoraPacketTracker::LostBecauseTxCallback,
                                                 m_packetTracker));
            }
        }

      // Create the MAC
      Ptr<LorawanMac> mac = macHelper.Create (node, device);
      NS_ASSERT (mac != 0);
      mac->SetPhy (phy);
      NS_LOG_DEBUG ("Done creating the MAC");
      device->SetMac (mac);

      if (m_packetTracker)
        {
          if (phyHelper.GetDeviceType () ==
              TypeId::LookupByName ("ns3::SimpleEndDeviceLoraPhy"))
            {
              mac->TraceConnectWithoutContext ("SentNewPacket",
                                               MakeCallback
                                                 (&LoraPacketTracker::MacTransmissionCallback,
                                                 m_packetTracker));

              mac->TraceConnectWithoutContext ("RequiredTransmissions",
                                               MakeCallback
                                                 (&LoraPacketTracker::RequiredTransmissionsCallback,
                                                 m_packetTracker));
            }
          else if (phyHelper.GetDeviceType () ==
                   TypeId::LookupByName ("ns3::SimpleGatewayLoraPhy"))
            {
              mac->TraceConnectWithoutContext ("SentNewPacket",
                                               MakeCallback
                                               (&LoraPacketTracker::MacTransmissionCallback,
                                                m_packetTracker));

              mac->TraceConnectWithoutContext ("ReceivedPacket",
                                               MakeCallback
                                               (&LoraPacketTracker::MacGwReceptionCallback,
                                                m_packetTracker));
            }
        }

      node->AddDevice (device);
      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ()->GetPosition ());
    }
  return devices;
}

NetDeviceContainer
LoraHelper::Install ( const LoraPhyHelper &phy,
                      const LorawanMacHelper &mac,
                      Ptr<Node> node) const
{
  return Install (phy, mac, NodeContainer (node));
}

void
LoraHelper::EnablePacketTracking ()
{
  NS_LOG_FUNCTION (this);

  // Create the packet tracker
  m_packetTracker = new LoraPacketTracker ();
}

LoraPacketTracker&
LoraHelper::GetPacketTracker (void)
{
  NS_LOG_FUNCTION (this);

  return *m_packetTracker;
}

void
LoraHelper::EnableSimulationTimePrinting (Time interval)
{
  m_oldtime = std::time (0);
  Simulator::Schedule (Seconds (0), &LoraHelper::DoPrintSimulationTime, this,
                       interval);
}

void
LoraHelper::EnablePeriodicDeviceStatusPrinting (NodeContainer endDevices,
                                                NodeContainer gateways,
                                                std::string filename,
                                                Time interval)
{
  NS_LOG_FUNCTION (this);

  DoPrintDeviceStatus (endDevices, gateways, filename);

  // Schedule periodic printing
  Simulator::Schedule (interval,
                       &LoraHelper::EnablePeriodicDeviceStatusPrinting, this,
                       endDevices, gateways, filename, interval);
}

void
LoraHelper::DoPrintDeviceStatus (NodeContainer endDevices, NodeContainer gateways,
                                 std::string filename)
{
  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  Time currentTime = Simulator::Now();
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Ptr<NetDevice> netDevice = object->GetDevice (0);
      Ptr<LoraNetDevice> loraNetDevice = netDevice->GetObject<LoraNetDevice> ();
      NS_ASSERT (loraNetDevice != 0);
      Ptr<ClassAEndDeviceLorawanMac> mac = loraNetDevice->GetMac ()->GetObject<ClassAEndDeviceLorawanMac> ();
      int dr = int(mac->GetDataRate ());
      double txPower = mac->GetTransmissionPower ();
      Vector pos = position->GetPosition ();
      outputFile << currentTime.GetSeconds () << " "
                 << object->GetId () <<  " "
                 << pos.x << " " << pos.y << " " << dr << " "
                 << unsigned(txPower) << std::endl;
    }

// [renzo] Here we can print the info for the gateways, but I disabled for having only Nodes in this file
//   for (NodeContainer::Iterator j = gateways.Begin (); j != gateways.End (); ++j)
//     {
//       Ptr<Node> object = *j;
//       Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
//       Vector pos = position->GetPosition ();
//       outputFile << currentTime.GetSeconds () << " "
//                  << object->GetId () <<  " "
//                  << pos.x << " " << pos.y << " " << "-1 -1" << std::endl;
//     }
  outputFile.close ();
}

void
LoraHelper::DoPrintDeviceStatus (NodeContainer gateways, std::string filename)
{
  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  Time currentTime = Simulator::Now();
  // [renzo] Here we can print the info for the nodes (we use it for gateways..)
   for (NodeContainer::Iterator j = gateways.Begin (); j != gateways.End (); ++j)
     {
       Ptr<Node> object = *j;
       Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
       Vector pos = position->GetPosition ();
       outputFile << currentTime.GetSeconds () << " "
                  << object->GetId () <<  " "
                  << pos.x << " " << pos.y << " " << "-1 -1" << std::endl;
     }
  outputFile.close ();
}


void
LoraHelper::EnablePeriodicPhyPerformancePrinting (NodeContainer gateways,
                                                  std::string filename,
                                                  Time interval)
{
  NS_LOG_FUNCTION (this);

  DoPrintPhyPerformance (gateways, filename);

  Simulator::Schedule (interval,
                       &LoraHelper::EnablePeriodicPhyPerformancePrinting,
                       this,
                       gateways, filename, interval);
}

void
LoraHelper::DoPrintPhyPerformance (NodeContainer gateways,
                                   std::string filename)
{
  NS_LOG_FUNCTION (this);

  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  for (auto it = gateways.Begin (); it != gateways.End (); ++it)
    {
      int systemId = (*it)->GetId ();
      outputFile << Simulator::Now ().GetSeconds () << " " <<
        std::to_string(systemId) << " " <<
        m_packetTracker->PrintPhyPacketsPerGw(m_lastPhyPerformanceUpdate,
                                              Simulator::Now (),
                                              systemId) << std::endl;
    }

  m_lastPhyPerformanceUpdate = Simulator::Now ();

  outputFile.close();
}

void
LoraHelper::EnablePeriodicGlobalPerformancePrinting (std::string filename,
                                                     Time interval)
{
  NS_LOG_FUNCTION (this << filename << interval);

  DoPrintGlobalPerformance (filename);

  Simulator::Schedule (interval,
                       &LoraHelper::EnablePeriodicGlobalPerformancePrinting,
                       this,
                       filename, interval);
}

void
LoraHelper::DoPrintGlobalPerformance (std::string filename)
{
  NS_LOG_FUNCTION (this);

  const char * c = filename.c_str ();
  std::ofstream outputFile;
  if (Simulator::Now () == Seconds (0))
    {
      // Delete contents of the file as it is opened
      outputFile.open (c, std::ofstream::out | std::ofstream::trunc);
    }
  else
    {
      // Only append to the file
      outputFile.open (c, std::ofstream::out | std::ofstream::app);
    }

  outputFile << Simulator::Now ().GetSeconds () << " " <<
    m_packetTracker->CountMacPacketsGlobally (m_lastGlobalPerformanceUpdate,
                                              Simulator::Now ()) <<
    std::endl;

  m_lastGlobalPerformanceUpdate = Simulator::Now ();

  outputFile.close();
}

void
LoraHelper::DoPrintSimulationTime (Time interval)
{
  // NS_LOG_INFO ("Time: " << Simulator::Now().GetHours());
  std::cout << "Simulated time: " << Simulator::Now ().GetHours () << " hours" << std::endl;
  std::cout << "Real time from last call: " << std::time (0) - m_oldtime << " seconds" << std::endl;
  m_oldtime = std::time (0);
  Simulator::Schedule (interval, &LoraHelper::DoPrintSimulationTime, this, interval);
}



/*
 *
 * RENZO NOTES:
 *
 * We have to work on this PcapSniffLoRaTx and PcapSniffLoRaRx functions , maybe have only one. But Tx and Rx gives versatility.
 * (we can refactor common code and still have two different functions.
 *
 *  Note: Also, I do not like this static functions, but maybe proper class functions. But when I tried to do that it raised some errors.
 *  	I believe some "magic" is happening here to hook  the PcapFileWrapper with this functions without context...
 *
 *  Ideally I do not want all the metatada as input parameters,
 *  As will be mentioned metadata should bet contained in a new Tag we have to define (NOT the LoraTag, because it does not have all we need )
 *  (Wit out tag all the code will be much cleaner, and MAC layer will have PHY information)
 *
 *   TODO: create this new Tag made to contain the metadata needed for "LoratapPcapHeader", and populate in the trace sources of the devices.
 * */
/**
 * @brief Write a packet in a PCAP file
 * @param file the output file
 * @param packet the packet
 * @param (TODO: metadata from the packet)
 */
static void
PcapSniffLoRaTx(
		Ptr<PcapFileWrapper> file,
		Ptr<const Packet> packet,
		double m_frequency, /* LoRa frequency (Hz) */
		uint8_t m_sf, /* LoRa SF (sf_t) [7, 8, 9, 10, 11, 12] */
		double m_packet_rssi, /* LoRa packet RSSI,*/
		int m_snr /* LoRa SNR*/
		)
{
    Ptr<Packet> packetCopy = packet -> Copy ();

    LoraTag packetLoRaTag; // We are missing Power and SNR information. --> Suggestion Define our own Packet Tag appropiate for LoraTapPcaHeader
    LoratapPcapHeader loratapHeader;


    packetCopy->RemovePacketTag(packetLoRaTag); // https://www.nsnam.org/docs/models/html/packets.html?highlight=packet
    packetLoRaTag.SetFrequency(m_frequency);
    packetLoRaTag.SetSpreadingFactor(m_sf);

    loratapHeader.FillHeader (packetLoRaTag);
    // This "FillHeader" is not enough because LoraTag does not have all the information, for not I manually set what is missing:
    loratapHeader.SetMPacketRssi(unsigned(int(139.5 + m_packet_rssi))); //139.5 insted of 139 to approximate to nearest int);
    loratapHeader.SetMSnr(m_snr);

    packetCopy -> AddHeader (loratapHeader);
	file->Write (Simulator::Now (), packetCopy);
}


static void
PcapSniffLoRaRx(Ptr<PcapFileWrapper> file,	Ptr<const Packet> packet)
{
    Ptr<Packet> packetCopy = packet -> Copy ();

    LoraTag packetLoRaTag; // We are missing Power and SNR information. --> Suggestion Define our own Packet Tag appropiate for LoraTapPcaHeader
    LoratapPcapHeader loratapHeader;

    packetCopy->RemovePacketTag(packetLoRaTag); // https://www.nsnam.org/docs/models/html/packets.html?highlight=packet
    loratapHeader.FillHeader (packetLoRaTag);

    // This "FillHeader" is not enough because LoraTag does not have all the information, for not I manually set what is missing:
    packetCopy -> AddHeader (loratapHeader);
	file->Write (Simulator::Now (), packetCopy);
}


 
void 
LoraHelper::EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename)
{
 //
 // All of the Pcap enable functions vector through here including the ones
 // that are wandering through all of devices on perhaps all of the nodes in
 // the system.  We can only deal with devices of type LoraNetDevice.
 //
 Ptr<LoraNetDevice> device = nd->GetObject<LoraNetDevice> ();
 if (device == 0)
   {
     NS_LOG_INFO ("LoraHelper::EnablePcapInternal(): Device " << device << " not of type ns3::LoraNetDevice");
     return;
   }

 Ptr<LoraPhy> phy = device->GetPhy();
 NS_ABORT_MSG_IF (phy == 0, "LoraHelper::EnablePcapInternal(): Phy layer in LoraNetDevice must be set");
 Ptr<LorawanMac> mac = device->GetMac();
 NS_ABORT_MSG_IF (mac == 0, "LoraHelper::EnablePcapInternal(): Mac layer in LoraNetDevice must be set");


 PcapHelper pcapHelper;

 std::string filename;
 if (explicitFilename)
   {
     filename = prefix;
   }
 else
   {
     filename = pcapHelper.GetFilenameFromDevice (prefix, device);
     NS_LOG_INFO ("LoraHelper::EnablePcapInternal(): Prefix of fime: " << prefix << ".");
   }

 Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, 
                                                    PcapHelper::DLT_LORATAP);


 //pcapHelper.HookDefaultSink<LoraNetDevice> (device, "PromiscSniffer", file); // Old first approach. This works for GWs for TX for sure, for RX (? did not check). For ED did not worked.

 /* Examples how they feed/hook the pcapFile from other modules helpers:
  *		 device->GetMac ()->TraceConnectWithoutContext ("PromiscSniffer", MakeBoundCallback (&PcapSniffLrWpan, file)); // https://gitlab.com/nsnam/ns-3-dev/-/blob/master/src/lr-wpan/helper/lr-wpan-helper.cc#L319
  *		 This approach I believe is better to not do a static function call:
  *		 phy->TraceConnectWithoutContext ("MonitorSnifferTx", MakeBoundCallback (&WifiPhyHelper::PcapSniffTxEvent, file)); //https://gitlab.com/nsnam/ns-3-dev/-/blob/master/src/wifi/helper/wifi-helper.cc#L612
  *		 phy->TraceConnectWithoutContext ("MonitorSnifferRx", MakeBoundCallback (&WifiPhyHelper::PcapSniffRxEvent, file));
 */



 /* ********************************************
  * 				TX Sniff
  * ********************************************
  *
  * A Sent packet is captured at PHY layer. (At MAC layer we can not guarantee the TX; e.g., PHY might not send).
  * Involved Classes:
  *    * [ALL]	LoraPhy --> ("SnifferTx" Trace Source)
  *    * [ED] 	SimpleEndDeviceLoraPhy::Send
  *    * [GW] 	SimpleGatewayLoraPhy::Send (TODO test)
  *    * [NS] 	TODO
  *
  * */
  phy->TraceConnectWithoutContext ("SnifferTx", MakeBoundCallback (&PcapSniffLoRaTx, file));
  // Comment, currently PcapSniffLoRaTx has a lot of additional parameters. We should define our custom packet Tag to simplify.


  /*********************************************
  * 				RX Sniff
  * ********************************************
  * Received Packet sniffing strategy depends on promiscuous mode activated or not
  * */

 if (promiscuous == true)
    {
	  /*
	   * Involved classes:
	   * 	* [ALL]	LoraPhy --> ("SnifferRx" Trace Source)
	   * 	* [ED] 	SimpleEndDeviceLoraPhy::EndReceive (TODO test --have not tested--)
	   * 	* [GW] 	SimpleGatewayLoraPhy::EndReceive  (Test STATUS: Some issues with Packet RSSI dBm, no SNR yet )
	   * 	* [NS] 	TODO
	   *
	   */
	 phy->TraceConnectWithoutContext ("SnifferRx", MakeBoundCallback (&PcapSniffLoRaRx, file));
  }
  else
  { // non-promiscuous. Has to be done at MAC layer (At PHY we do not have the notion of unicast)

	  /*
	   * Involved classes:
	   * 	* [ALL]	LorawanMac --> ("SnifferRx" Trace Source)
	   * 	* [ED] 	ClassAEndDeviceLorawanMac::Receive (TODO test --have not tested--)
	   * 	* [GW] 	GatewayLorawanMac::Receive (Test STATUS: Some issues with Packet RSSI dBm, no SNR yet )
	   * 	* [NS] 	TODO
	   *
	   * TODO: currently a Packet at MAC does not have the PHY metadata (or if it has it, is by mere chance/luck in the LoRaTag),
	   * 	 we have to define our own Tag to contain this metadata explicitly.
	   */
	  mac->TraceConnectWithoutContext ("SnifferRx", MakeBoundCallback (&PcapSniffLoRaRx, file));

  }

}



}
}
