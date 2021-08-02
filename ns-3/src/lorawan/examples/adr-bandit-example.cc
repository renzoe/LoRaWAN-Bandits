/*
 * This program creates a simple network which uses an ADR algorithm to set up
 * the Spreading Factors of the devices in the Network.
 */

#include "ns3/point-to-point-module.h"
#include "ns3/forwarder-helper.h"
#include "ns3/network-server-helper.h"
#include "ns3/lora-channel.h"
#include "ns3/mobility-helper.h"
#include "ns3/lora-phy-helper.h"
#include "ns3/lorawan-mac-helper.h"
#include "ns3/lora-helper.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/periodic-sender.h"
#include "ns3/periodic-sender-helper.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/command-line.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/lora-device-address-generator.h"
#include "ns3/random-variable-stream.h"
#include "ns3/config.h"
#include "ns3/rectangle.h"
#include "ns3/hex-grid-position-allocator.h"

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("AdrBanditExample");

// Trace sources that are called when a node changes its DR or TX power
void OnDataRateChange (uint8_t oldDr, uint8_t newDr)
{
  NS_LOG_DEBUG ("DR" << unsigned(oldDr) << " -> DR" << unsigned(newDr));
}
void OnTxPowerChange (double oldTxPower, double newTxPower)
{
  NS_LOG_DEBUG (oldTxPower << " dBm -> " << newTxPower << " dBm");
}

int main (int argc, char *argv[])
{

  bool verbose = false;
  bool adrEnabled = true;
  bool initializeSF = false;
  int nDevices = 400;
  int nPeriods = 20;

  int nGateways = 1;
  double radius = 6400;
  //double radius = 2400;


  double maxRandomLoss = 10;
  double minSpeed = 2;
  double maxSpeed = 16;
  std::string adrType = "ns3::AdrComponent"; /* [Renzo] Here we can use different ADR implementations (at NS) */

  CommandLine cmd;
  cmd.AddValue ("verbose", "Whether to print output or not", verbose);
  cmd.AddValue ("MultipleGwCombiningMethod",
                "ns3::AdrComponent::MultipleGwCombiningMethod");
  cmd.AddValue ("MultiplePacketsCombiningMethod",
                "ns3::AdrComponent::MultiplePacketsCombiningMethod");
  cmd.AddValue ("HistoryRange", "ns3::AdrComponent::HistoryRange");
  cmd.AddValue ("MType", "ns3::EndDeviceLorawanMac::MType");
  cmd.AddValue ("EDDRAdaptation", "ns3::EndDeviceLorawanMac::EnableEDDataRateAdaptation");
  cmd.AddValue ("ChangeTransmissionPower",
                "ns3::AdrComponent::ChangeTransmissionPower");
   cmd.AddValue ("AdrEnabled", "Whether to enable ADR", adrEnabled);
   cmd.AddValue ("nDevices", "Number of devices to simulate", nDevices);
   cmd.AddValue ("PeriodsToSimulate", "Number of periods to simulate", nPeriods);
   cmd.AddValue ("radius", "The radius of the area to simulate", radius);
   cmd.AddValue ("maxRandomLoss",
                 "Maximum amount in dB of the random loss component",
                 maxRandomLoss);
   cmd.AddValue ("initializeSF",
                 "Whether to initialize the SFs",
                 initializeSF);
   cmd.AddValue ("MinSpeed",
                 "Minimum speed for mobile devices",
                 minSpeed);
   cmd.AddValue ("MaxSpeed",
                 "Maximum speed for mobile devices",
                 maxSpeed);
   cmd.AddValue ("MaxTransmissions",
                 "ns3::EndDeviceLorawanMac::MaxTransmissions");
   cmd.Parse (argc, argv);


   // Logging
   //////////

   LogComponentEnable ("AdrBanditExample", LOG_LEVEL_ALL);

   //LogComponentEnable ("LoraHelper", LOG_LEVEL_ALL);
   // LogComponentEnable ("LoraPacketTracker", LOG_LEVEL_ALL);
//    LogComponentEnable ("NetworkServer", LOG_LEVEL_ALL);

//    LogComponentEnable ("NetworkController", LOG_LEVEL_ALL);
   // LogComponentEnable ("NetworkScheduler", LOG_LEVEL_ALL);
   // LogComponentEnable ("NetworkStatus", LOG_LEVEL_ALL);


   // LogComponentEnable ("EndDeviceStatus", LOG_LEVEL_ALL);

   // LogComponentEnable ("AdrComponent", LOG_LEVEL_ALL);
    //LogComponentEnable("ClassAEndDeviceLorawanMac", LOG_LEVEL_ALL);


//   LogComponentEnable("ClassAEndDeviceLorawanMacBandit", LOG_LEVEL_ALL);
   //LogComponentEnable("ClassAEndDeviceLorawanMacBandit", LOG_LEVEL_LOGIC);


 //   LogComponentEnable("BanditPolicy", LOG_LEVEL_ALL);
 //   LogComponentEnable("ADRBanditAgent", LOG_LEVEL_ALL);

   //LogComponentEnable ("EndDeviceLorawanMac", LOG_LEVEL_ALL);

   // LogComponentEnable ("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
   //LogComponentEnable ("MacCommand", LOG_LEVEL_ALL);
   // LogComponentEnable ("AdrExploraSf", LOG_LEVEL_ALL);
   // LogComponentEnable ("AdrExploraAt", LOG_LEVEL_ALL);



 //  LogComponentEnable ("SimpleGatewayLoraPhy", LOG_LEVEL_ALL);


    /*Debugging new MAC command */
//    LogComponentEnable ("SimpleEndDeviceLoraPhy", LOG_LEVEL_ALL);
//    LogComponentEnable ("EndDeviceLoraPhy", LOG_LEVEL_ALL);
//    //LogComponentEnable ("LoraFrameHeader", LOG_LEVEL_ALL);
//    LogComponentEnable ("EndDeviceLorawanMac", LOG_LEVEL_ALL);
//
//
//

//   LogComponentEnable ("NetworkControllerComponentBandit", LOG_LEVEL_ALL); // We can debug how the NS answers to the MAC BanditRewardReq and prepares the Reward in the BanditRewardAns
//   LogComponentEnable ("BanditDelayedRewardIntelligence", LOG_LEVEL_ALL); // We can debug (among many things) how the bandits creates the MAC BanditRewardReq
//   LogComponentEnable("ClassAEndDeviceLorawanMacBandit", LOG_LEVEL_ALL); // We can debug when the bandit receives the MAC BanditRewardAns


//
//
//
//    LogComponentEnable ("PeriodicSender", LOG_LEVEL_ALL);

  //    LogComponentEnable ("LoraInterferenceHelper", LOG_LEVEL_ALL); // To test interference Matrix






   LogComponentEnableAll (LOG_PREFIX_FUNC);
   LogComponentEnableAll (LOG_PREFIX_NODE);
   LogComponentEnableAll (LOG_PREFIX_TIME);



   // Set the EDs to require Data Rate control from the NS
   Config::SetDefault ("ns3::EndDeviceLorawanMac::DRControl", BooleanValue (true));

   // Create a simple wireless channel
   ///////////////////////////////////
   /// //

   Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
   loss->SetPathLossExponent (3.76);
   loss->SetReference (1, 7.7);

   // [RN] Discussion about this values (7.7) https://gitter.im/ns-3-lorawan/Lobby?at=5c1e3fe72863d8612b71b730

   Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
   x->SetAttribute ("Min", DoubleValue (0.0));
   x->SetAttribute ("Max", DoubleValue (maxRandomLoss));

   Ptr<RandomPropagationLossModel> randomLoss = CreateObject<RandomPropagationLossModel> ();
   randomLoss->SetAttribute ("Variable", PointerValue (x));

   loss->SetNext (randomLoss);

   Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

   Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

   // Helpers
   //////////

   // End Device mobility
   MobilityHelper mobilityEd, mobilityGw;
   mobilityEd.SetPositionAllocator ("ns3::UniformDiscPositionAllocator", "rho", DoubleValue (radius),
                                  "X", DoubleValue (0.0), "Y", DoubleValue (0.0));


   // // Gateway mobility
   Ptr<ListPositionAllocator> positionAllocGw = CreateObject<ListPositionAllocator> ();
   positionAllocGw->Add (Vector (0.0, 0.0, 15.0)); // a gateway in the center of the cartesian cordinates!
   // positionAllocGw->Add (Vector (-5000.0, -5000.0, 15.0));
   // positionAllocGw->Add (Vector (-5000.0, 5000.0, 15.0));
   // positionAllocGw->Add (Vector (5000.0, -5000.0, 15.0));
   // positionAllocGw->Add (Vector (5000.0, 5000.0, 15.0));
    mobilityGw.SetPositionAllocator (positionAllocGw);
    mobilityGw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

   //mobilityGw.SetPositionAllocator ("ns3::UniformDiscPositionAllocator", "rho", DoubleValue (radius),
   //                               "X", DoubleValue (0.0), "Y", DoubleValue (0.0));// [RN] Super Wrong! I was putting the GW at random!!

   //mobilityGw.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

   // Create the LoraPhyHelper
   LoraPhyHelper phyHelper = LoraPhyHelper ();
   phyHelper.SetChannel (channel);

   // Create the LorawanMacHelper
   LorawanMacHelper macHelper = LorawanMacHelper ();

   // Create the LoraHelper
   LoraHelper helper = LoraHelper ();
   helper.EnablePacketTracking ();

   ////////////////
   // Create GWs //
   ////////////////

   NodeContainer gateways;
   gateways.Create (nGateways);
   mobilityGw.Install (gateways);

   // Create the LoraNetDevices of the gateways
   phyHelper.SetDeviceType (LoraPhyHelper::GW);
   macHelper.SetDeviceType (LorawanMacHelper::GW);
   helper.Install (phyHelper, macHelper, gateways);

   // Create EDs
   /////////////

   NodeContainer endDevices;
   endDevices.Create (nDevices);

   // Install mobility model on fixed nodes (all nodes are fixed on this modified example)
   mobilityEd.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

   for (int i = 0; i < nDevices; ++i) // i < (int) endDevices.GetN ()
   {
      mobilityEd.Install (endDevices.Get (i));
   }

  // Make it so that nodes are at a certain height > 0
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      Vector position = mobility->GetPosition ();
      position.z = 1.2;
      mobility->SetPosition (position);
    }

  // Create a LoraDeviceAddressGenerator
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen = CreateObject<LoraDeviceAddressGenerator> (nwkId,nwkAddr);

  // Create the LoraNetDevices of the end devices
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LorawanMacHelper::ED_A); // We create normal ADR nodes
  //macHelper.SetDeviceType (LorawanMacHelper::ED_A_ADR_BANDIT); // We create ADR Bandits nodes :)
  macHelper.SetAddressGenerator (addrGen);
  macHelper.SetRegion (LorawanMacHelper::EU);
  helper.Install (phyHelper, macHelper, endDevices);

  // Install applications in EDs
  int appPeriodSeconds = 1200;      // One packet every 20 minutes
  //int appPeriodSeconds = 300;      // One packet every 5 minutes (300 s)

  /*Renzo: BEWARE! When the max size is excedeed (eg by a MAC command) there simulation does not work and does not print proper debug*/

  int packetSizeBytes = 32;   /* 49, 50 --> When we piggyback MAC answers at SF=12 cause problems: (LoraTap has a header of 15 Bytes)
  	 EndDeviceLorawanMac:DoSend(): Attempting to send a packet larger than the maximum allowed size at this DataRate (DR0). Transmission canceled. */

  PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
  appHelper.SetPacketSize (packetSizeBytes);
  appHelper.SetPeriod (Seconds (appPeriodSeconds));
  ApplicationContainer appContainer = appHelper.Install (endDevices);

  // Do not set spreading factors up: we will wait for the NS to do this
  if (initializeSF)
    {
      macHelper.SetSpreadingFactorsUp (endDevices, gateways, channel);
    }

  ////////////
  // Create NS
  ////////////

  NodeContainer networkServers;
  networkServers.Create (1);

  // Install the NetworkServer application on the network server
  NetworkServerHelper networkServerHelper;
  networkServerHelper.SetGateways (gateways);
  networkServerHelper.SetEndDevices (endDevices);
  networkServerHelper.EnableAdr (adrEnabled);
  networkServerHelper.SetAdr (adrType);
  networkServerHelper.Install (networkServers);

  // Install the Forwarder application on the gateways
  ForwarderHelper forwarderHelper;
  forwarderHelper.Install (gateways);

  // Connect our traces ([Renzo] Disable for big simulations to go faster)
  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/0/$ns3::LoraNetDevice/Mac/$ns3::EndDeviceLorawanMac/TxPower",
  //                               MakeCallback (&OnTxPowerChange));
  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/0/$ns3::LoraNetDevice/Mac/$ns3::EndDeviceLorawanMac/DataRate",
  //                               MakeCallback (&OnDataRateChange));


  // Activate printing of ED MAC parameters
  Time stateSamplePeriod = Seconds (1200);
  helper.EnablePeriodicDeviceStatusPrinting (endDevices, gateways, "nodeData.txt", stateSamplePeriod);
  helper.EnablePeriodicPhyPerformancePrinting (gateways, "phyPerformance.txt", stateSamplePeriod);
  helper.EnablePeriodicGlobalPerformancePrinting ("globalPerformance.txt", stateSamplePeriod);


  // phyPerformance: SENT  RECEIVED   INTERFERED NO_MORE_RECEIVERS  UNDER_SENSITIVITY  LOST_BECAUSE_TX
  // nodeData : currentTime.GetSeconds () , object->GetId () ;  pos.x  ;  pos.y ;  dr (data rate) ;   unsigned(txPower)



  //[Renzo] PCAP Helper
  // https://gitlab.com/nsnam/ns-3-dev/-/blob/master/src/lr-wpan/helper/lr-wpan-helper.cc#L319
  // https://gitlab.com/nsnam/ns-3-dev/-/blob/master/src/wifi/helper/wifi-helper.cc#L612
  //helper.EnablePcapAll(std::string ("ADR-Bandit-Example"), true);

  // [Renzo] Disable for big simulations to go faster (but not that much):
  //helper.EnablePcap(std::string ("ADR-Bandit-Example-ED"), endDevices , false);
  //helper.EnablePcap(std::string ("ADR-Bandit-Example-GW"), gateways , true);
  //helper.EnablePcap(std::string ("ADR-Bandit-Example-NS"), networkServers , true);



  /*Renzo Notes:
   * ADR component: ./waf --run "src/lorawan/examples/adr-bandit-example  --nDevices=1 --HistoryRange=19 --PeriodsToSimulate=5"
   * Only decreases 3 setps, but with modifs could do 4 steps at once. (Resume: it takes two messageq to decease 4 steps total)  */



  LoraPacketTracker& tracker = helper.GetPacketTracker ();

  // Start simulation
  Time simulationTime = Seconds (1200 * nPeriods);
  Simulator::Stop (simulationTime);
  Simulator::Run ();
  Simulator::Destroy ();

  std::cout << tracker.CountMacPacketsGlobally(Seconds (1200 * (nPeriods - 2)),
                                               Seconds (1200 * (nPeriods - 1))) << std::endl;

  return 0;
}
