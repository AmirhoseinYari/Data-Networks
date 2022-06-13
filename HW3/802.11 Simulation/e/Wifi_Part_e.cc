#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/core-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include <string>

using namespace ns3;

/// Run single 10 seconds experiment
void experiment (bool enableCtsRts, std::string wifiManager, int n)
{
  // 0. Enable or disable CTS/RTS
  UintegerValue ctsThr = (enableCtsRts ? UintegerValue (100) : UintegerValue (2200));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);

  // 1. Create n+n nodes first n is A second is B
  NodeContainer nodes;
  nodes.Create (2*n);

  // 2. Place nodes somehow, this is required by every wireless simulation
  for (uint8_t i = 0; i < 2*n; ++i)
    {
      nodes.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
    }

  // 3. Create propagation loss matrix
  Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel> ();
  lossModel->SetDefaultLoss (120); // set default loss to 120 dB (no link)
  for (int i = 0; i<n ; i++){
    lossModel->SetLoss (nodes.Get (i)->GetObject<MobilityModel> (), nodes.Get (i+n)->GetObject<MobilityModel> (), 40); //Ai, Bi, 40dB
  }
  for (int i=0 ; i<n ; i++){ //Every Bi node is connected
    for (int j=i+1 ; j<n ; j++){
        lossModel->SetLoss (nodes.Get (i+n)->GetObject<MobilityModel> (), nodes.Get (j+n)->GetObject<MobilityModel> (), 50); //Bi, Bj, 50dB
    }
  }

  // 4. Create & setup wifi channel
  Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
  wifiChannel->SetPropagationLossModel (lossModel);
  wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

  // 5. Install wireless devices
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::" + wifiManager + "WifiManager");
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel);
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac"); // use ad-hoc MAC
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("19.71.8.0", "255.255.255.0"); //98109718 --> 19.71.8.0
  ipv4.Assign (devices);

  // 7. Install applications: n CBR streams each saturating the channel
  ApplicationContainer cbrApps;
  uint16_t cbrPort = 12345;

  for (int i=0;i<n;i++){
    std::string s = "19.71.8."+std::to_string(i+1);
    const char * s_ip = s.c_str();
    //std::cout << s_ip <<"\n";
    OnOffHelper onOff ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address (s_ip), cbrPort));//Ai
    onOff.SetAttribute ("PacketSize", UintegerValue (1400));
    onOff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOff.SetAttribute ("DataRate", StringValue ("3000000bps"));
    onOff.SetAttribute ("StartTime", TimeValue (Seconds (1.000000)));
    cbrApps.Add (onOff.Install (nodes.Get (i+n))); //Bi
  }
  
  /** \internal
   * We also use separate UDP applications that will send a single
   * packet before the CBR flows start.
   * This is a workaround for the lack of perfect ARP, see \bugid{187}
   */
  uint16_t  echoPort = 9;
  for (int i=0;i<n;i++){
    std::string s = "19.71.8."+std::to_string(i+1);
    const char * s_ip = s.c_str();
    //std::cout << s_ip <<"\n";
    UdpEchoClientHelper echoC (Ipv4Address (s_ip), echoPort);//Ai
    echoC.SetAttribute ("MaxPackets", UintegerValue (1));
    echoC.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
    echoC.SetAttribute ("PacketSize", UintegerValue (10));

    ApplicationContainer pingA;
    // again using different start times to workaround Bug 388 and Bug 912
    echoC.SetAttribute ("StartTime", TimeValue (Seconds (0.001)));
    pingA.Add (echoC.Install (nodes.Get (i+n))); //Bi
  }

  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  //set mobility, position, ... nodes
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(-0.0),
                                "MinY", DoubleValue(5.0),
                                "DeltaX", DoubleValue(10.0),
                                "DeltaY", DoubleValue(0.0),
                                "GridWidth", UintegerValue(50.0),
                                "LayoutType", StringValue("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  //Mobility for A nodes
  for (int i=0;i<n;i++){
    positionAlloc->Add (Vector (10.0*i,0.0,0.0));//Ai
  }
  //Mobility for B nodes
  for (int i=0;i<n;i++){
    positionAlloc->Add (Vector (10.0*i,10.0,0.0));//Bi
  }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (nodes);

  // 9. Run simulation for 10 seconds
  Simulator::Stop (Seconds (10));
  Simulator::Run ();

  // 10. Print per flow statistics 
  double throu = 0; //for avraging
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      // first n FlowIds are for ECHO apps, we don't want to display them
      //
      // Duration for throughput measurement is 9.0 seconds, since
      //   StartTime of the OnOffApplication is at about "second 1"
      // and
      //   Simulator::Stops at "second 10".
      if (i->first > (unsigned int)n)
        {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
          std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
          throu += i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000;
        }
    }
  //print avrage throughput
  std::cout << "Avrage Throughput for (n = " << n << ") : " << throu/n << " Mbps\n" ;
  // 11. Cleanup
  Simulator::Destroy ();
}

int main (int argc, char **argv)
{
  int n;
  std::string wifiManager ("Arf");
  CommandLine cmd;
  cmd.AddValue("n", "This is n", n);
  cmd.AddValue ("wifiManager", "Set wifi rate manager (Aarf, Aarfcd, Amrr, Arf, Cara, Ideal, Minstrel, Onoe, Rraa)", wifiManager);
  cmd.Parse (argc, argv);
  std::cout << "n is = " << n <<"\n";
  std::cout << "Hidden station experiment with RTS/CTS disabled:\n" << std::flush;
  experiment (false, wifiManager, n);
  std::cout << "------------------------------------------------\n";
  std::cout << "Hidden station experiment with RTS/CTS enabled:\n";
  experiment (true, wifiManager, n);

  return 0;
}
