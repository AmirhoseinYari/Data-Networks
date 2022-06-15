#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/bridge-module.h"
#include "ns3/trace-helper.h" ////
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3;

void experiment(){
// 1. Create Nodes
NodeContainer terminals;
terminals.Create(8);


NodeContainer bridges;
bridges.Create(6);

CsmaHelper csma_10; // 10 meters CSMA link
CsmaHelper csma_50; // 50 meters CSMA link
CsmaHelper csma_100; // 100 meters CSMA link

csma_10.SetChannelAttribute("DataRate", DataRateValue(1000000));
csma_10.SetChannelAttribute("Delay", TimeValue(MicroSeconds(0.2)));

csma_50.SetChannelAttribute("DataRate", DataRateValue(1000000));
csma_50.SetChannelAttribute("Delay", TimeValue(MicroSeconds(1)));

csma_100.SetChannelAttribute("DataRate", DataRateValue(1000000));
csma_100.SetChannelAttribute("Delay", TimeValue(MicroSeconds(2)));


// 2. Create links betweeen Nodes and bridges

NetDeviceContainer terminalDevices;
NetDeviceContainer bridge_N1_Devices;
NetDeviceContainer bridge_N2_Devices;
NetDeviceContainer bridge_N3_Devices;
NetDeviceContainer bridge_N4_Devices;
NetDeviceContainer bridge_N5_Devices;
NetDeviceContainer bridge_N6_Devices;


NetDeviceContainer link_C1_N1 = csma_10.Install(NodeContainer(terminals.Get(0), bridges.Get(0)));
NetDeviceContainer link_C2_N1 = csma_10.Install(NodeContainer(terminals.Get(1), bridges.Get(0)));
NetDeviceContainer link_N1_N2 = csma_50.Install(NodeContainer(bridges.Get(0), bridges.Get(1)));
NetDeviceContainer link_N1_N3 = csma_50.Install(NodeContainer(bridges.Get(0), bridges.Get(2)));

terminalDevices.Add(link_C1_N1.Get(0));
bridge_N1_Devices.Add(link_C1_N1.Get(1));

terminalDevices.Add(link_C2_N1.Get(0));
bridge_N1_Devices.Add(link_C2_N1.Get(1));

bridge_N1_Devices.Add(link_N1_N2.Get(0));
bridge_N2_Devices.Add(link_N1_N2.Get(1));

bridge_N1_Devices.Add(link_N1_N3.Get(0));
bridge_N3_Devices.Add(link_N1_N3.Get(1));

NetDeviceContainer link_N2_N5 = csma_100.Install(NodeContainer(bridges.Get(1), bridges.Get(4)));
bridge_N2_Devices.Add(link_N2_N5.Get(0));
bridge_N5_Devices.Add(link_N2_N5.Get(1));

NetDeviceContainer link_C3_N3 = csma_10.Install(NodeContainer(terminals.Get(2), bridges.Get(2)));
NetDeviceContainer link_C4_N3 = csma_10.Install(NodeContainer(terminals.Get(3), bridges.Get(2)));
terminalDevices.Add(link_C3_N3.Get(0));
bridge_N3_Devices.Add(link_C3_N3.Get(1));
terminalDevices.Add(link_C4_N3.Get(0));
bridge_N3_Devices.Add(link_C4_N3.Get(1));


NetDeviceContainer link_C5_N4 = csma_10.Install(NodeContainer(terminals.Get(4), bridges.Get(3)));
NetDeviceContainer link_C6_N4 = csma_10.Install(NodeContainer(terminals.Get(5), bridges.Get(3)));
NetDeviceContainer link_N4_N5 = csma_50.Install(NodeContainer(bridges.Get(3), bridges.Get(4)));
NetDeviceContainer link_N4_N6 = csma_50.Install(NodeContainer(bridges.Get(3), bridges.Get(5)));

terminalDevices.Add(link_C5_N4.Get(0));
bridge_N4_Devices.Add(link_C5_N4.Get(1));
terminalDevices.Add(link_C6_N4.Get(0));
bridge_N4_Devices.Add(link_C6_N4.Get(1));
bridge_N4_Devices.Add(link_N4_N5.Get(0));
bridge_N5_Devices.Add(link_N4_N5.Get(1));
bridge_N4_Devices.Add(link_N4_N6.Get(0));
bridge_N6_Devices.Add(link_N4_N6.Get(1));

NetDeviceContainer link_C7_N6 = csma_10.Install(NodeContainer(terminals.Get(6), bridges.Get(5)));
NetDeviceContainer link_C8_N6 = csma_10.Install(NodeContainer(terminals.Get(7), bridges.Get(5)));
terminalDevices.Add(link_C7_N6.Get(0));
bridge_N6_Devices.Add(link_C7_N6.Get(1));
terminalDevices.Add(link_C8_N6.Get(0));
bridge_N6_Devices.Add(link_C8_N6.Get(1));


// 3. Install bridge device on bridges and attach its ports
BridgeHelper bridge;
bridge.Install (bridges.Get(0), bridge_N1_Devices);
bridge.Install (bridges.Get(1), bridge_N2_Devices);
bridge.Install (bridges.Get(2), bridge_N3_Devices);
bridge.Install (bridges.Get(3), bridge_N4_Devices);
bridge.Install (bridges.Get(4), bridge_N5_Devices);
bridge.Install (bridges.Get(5), bridge_N6_Devices);

// 4. Add internet stack to the terminal nodes

InternetStackHelper internet;
internet.Install(terminals);

// 5. Add ip addresses

Ipv4AddressHelper ipv4;
ipv4.SetBase("10.10.69.0", "255.255.255.0");
ipv4.Assign(terminalDevices);
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


// 6. Create flows

ApplicationContainer app;

uint16_t port = 9;
int simulation_time = 10;
int start_time = 0;

// flow1 C1 ------> C6
OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.10.69.6"), port)));
onoff1.SetConstantRate (DataRate ("1000kb/s"));
onoff1.SetAttribute ("StartTime", TimeValue (Seconds (start_time)));///////
app.Add (onoff1.Install(terminals.Get (0)));

// flow2 C2 ------> C7
OnOffHelper onoff2 ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.10.69.7"), port)));
onoff2.SetConstantRate (DataRate ("1000kb/s"));
onoff2.SetAttribute ("StartTime", TimeValue (Seconds (start_time)));/////
app.Add (onoff2.Install(terminals.Get (1)));


// Add packet sinkers here

// 8. Install FlowMonitor on all nodes
FlowMonitorHelper flowmon;
Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

// 9. Run simulation for 10 seconds
Simulator::Stop (Seconds (simulation_time));
Simulator::Run ();

// 10. Print per flow statistics
monitor->CheckForLostPackets ();
Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
{
// Simulator::Stops at "second 10".
if (i->first > 0)
{
Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
double duration = simulation_time - start_time;
double total_delay = i->second.delaySum.GetMilliSeconds();
std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
std::cout << " Tx Packets: " << i->second.txPackets << "\n";
std::cout << " Tx Bytes: " << i->second.txBytes << "\n";
std::cout << " TxOffered: " << i->second.txBytes * 8.0 / duration / 1000 / 1000 << " Mbps\n";
std::cout << " Rx Packets: " << i->second.rxPackets << "\n";
std::cout << " Rx Bytes: " << i->second.rxBytes << "\n";
std::cout << " Throughput: " << i->second.rxBytes * 8.0 / duration / 1000 / 1000 << " Mbps\n";
std::cout << " Avg Delay: " << total_delay / i->second.rxPackets / 1000 << " seconds\n";
}

}

// 11. Cleanup
Simulator::Destroy ();
}


int main(){
experiment();
return 0;
}