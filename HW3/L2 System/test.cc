// FROM HERE :
// https://github.com/microsoft/Tocino/blob/master/src/bridge/examples/csma-bridge-one-hop.cc
 
#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

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
#include "ns3/netanim-module.h"
 
using namespace ns3;
 
NS_LOG_COMPONENT_DEFINE ("CsmaBridgeOneHopExample");
 
int
main (int argc, char *argv[])
{
  //
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  //
#if 0
  LogComponentEnable ("CsmaBridgeOneHopExample", LOG_LEVEL_INFO);
#endif
 
  //
  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
  //
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
 
  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
 
  Ptr<Node> c1 = CreateObject<Node> ();
  Ptr<Node> c2 = CreateObject<Node> ();
  Ptr<Node> c3 = CreateObject<Node> ();
  Ptr<Node> c4 = CreateObject<Node> ();
  Ptr<Node> c5 = CreateObject<Node> ();
  Ptr<Node> c6 = CreateObject<Node> ();
  Ptr<Node> c7 = CreateObject<Node> ();
  Ptr<Node> c8 = CreateObject<Node> ();
 
  Ptr<Node> N1 = CreateObject<Node> ();
  Ptr<Node> N2 = CreateObject<Node> ();
  Ptr<Node> N3 = CreateObject<Node> ();
  Ptr<Node> N4 = CreateObject<Node> ();
  Ptr<Node> N5 = CreateObject<Node> ();
  Ptr<Node> N6 = CreateObject<Node> ();
 
  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (1000000)); //1Mbps
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2))); //1u
 
  // Create the csma links, from each terminal to the bridge
  // This will create six network devices; we'll keep track separately
  // of the devices on and off the bridge respectively, for later configuration
  NetDeviceContainer LanDevices1;
  NetDeviceContainer BridgeDevices1;
  // It is easier to iterate the nodes in C++ if we put them into a container
  NodeContainer Lan1 (c1, c2);
  for (int i = 0; i < 2; i++)
    {
      // install a csma channel between the ith lan1 node and the bridge1 node
      NetDeviceContainer link = csma.Install (NodeContainer (Lan1.Get (i), N1));
      LanDevices1.Add (link.Get (0));
      BridgeDevices1.Add (link.Get (1));
    }
  
  //
  // Now, Create the bridge netdevice, which will do the packet switching.  The
  // bridge lives on the node bridge1 and bridges together the topBridgeDevices
  // which are the three CSMA net devices on the node in the diagram above.
  //
  BridgeHelper bridge;
  bridge.Install (N1, BridgeDevices1);
 
  // Add internet stack to the router nodes
  //NodeContainer routerNodes (n0, n1, n2, n3, n4);
  //InternetStackHelper internet;
  //internet.Install (routerNodes);

  // Repeat for other bridges ///////////////
  NetDeviceContainer LanDevices3;
  NetDeviceContainer BridgeDevices3;
  NodeContainer Lan3 (c3, c4);
  for (int i = 0; i < 2; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (Lan3.Get (i), N3));
      LanDevices3.Add (link.Get (0));
      BridgeDevices3.Add (link.Get (1));
    }
  bridge.Install (N3, BridgeDevices3);

  NetDeviceContainer LanDevices4;
  NetDeviceContainer BridgeDevices4;
  NodeContainer Lan4 (c5, c6);
  for (int i = 0; i < 2; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (Lan4.Get (i), N4));
      LanDevices4.Add (link.Get (0));
      BridgeDevices4.Add (link.Get (1));
    }
  bridge.Install (N4, BridgeDevices4);

  NetDeviceContainer LanDevices6;
  NetDeviceContainer BridgeDevices6;
  NodeContainer Lan6 (c7, c8);
  for (int i = 0; i < 2; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (Lan6.Get (i), N6));
      LanDevices6.Add (link.Get (0));
      BridgeDevices6.Add (link.Get (1));
    }
  bridge.Install (N6, BridgeDevices6);

 
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.10.18.00", "255.255.255.0");
  ipv4.Assign (LanDevices1);
  ipv4.Assign (LanDevices3);
  ipv4.Assign (LanDevices4);
  ipv4.Assign (LanDevices6);
 
  //
  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.  We excuse the bridge nodes from having to serve as
  // routers, since they don't even have internet stacks on them.
  //
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  //
  // Create an OnOff application to send UDP datagrams from node zero to node 1.
  //
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)
 
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.10.18.6"), port)));
  onoff.SetConstantRate (DataRate ("1000kb/s"));
 
  ApplicationContainer app = onoff.Install (c6);
  // Start the application
  app.Start (Seconds (1.0));
  app.Stop (Seconds (10.0));
 
  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer sink1 = sink.Install (c1);
  sink1.Start (Seconds (1.0));
  sink1.Stop (Seconds (10.0));
 
  //
  // Create a similar flow from n3 to n0, starting at time 1.1 seconds
  //
  onoff.SetAttribute ("Remote",
                      AddressValue (InetSocketAddress (Ipv4Address ("10.10.18.7"), port)));
  ApplicationContainer app2 = onoff.Install (n7);
  app2.Start (Seconds (1.1));
  app2.Stop (Seconds (10.0));
 
  ApplicationContainer sink2 = sink.Install (n2);
  sink2.Start (Seconds (1.1));
  sink2.Stop (Seconds (10.0));
 
  NS_LOG_INFO ("Configure Tracing.");
 
  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "csma-bridge-one-hop.tr"
  //
  AsciiTraceHelper ascii;
  //csma.EnableAsciiAll (ascii.CreateFileStream ("csma-bridge-one-hop.tr"));
 
  //
  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     csma-bridge-one-hop-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  //
  //csma.EnablePcapAll ("csma-bridge-one-hop", false);
  AnimationInterface anim (L2_Part_b.xml);
  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}