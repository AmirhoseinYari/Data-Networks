// FROM HERE :
// https://github.com/microsoft/Tocino/blob/master/src/bridge/examples/csma-bridge-one-hop.cc
// Dijkstra :
// https://www.tutorialspoint.com/cplusplus-program-for-dijkstra-s-shortest-path-algorithm
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
#include "ns3/ipv4-global-routing-helper.h" //
#include "ns3/udp-echo-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/on-off-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include <string>
#include <bits/stdc++.h>
#include <stdio.h>

 
using namespace ns3;
using namespace std;
 
NS_LOG_COMPONENT_DEFINE ("CsmaBridgeOneHopExample");

// Creating shortcut for an integer pair and Dijkstra function
typedef  pair<int, int> iPair;
vector< pair<double, pair<int, int> >> dijkstra(double G[6][6],int n,int startnode);

int main() {
  double G[6][6]={{0    ,50   , 50   ,186.6,0    , 0     },
                  {50   ,0    , 50   ,0    ,100  , 145.5 },
                  {50   ,50   , 0    ,193.2,145.5, 186.6 },
                  {186.6,0    , 193.2,0    ,50   , 50    },
                  {0    ,100  , 145.5,50   ,0    , 145.5 },
                  {0    ,145.5, 186.6,50   ,145.5, 0     }
                  };
  int n=6;
  int u=0;
  vector< pair <double, pair<int, int> >> out = dijkstra(G,n,u);

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
  csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (0.2))); //10m = 0.2 us
 
  // Create the csma links, from each terminal to the bridge
  // This will create six network devices; we'll keep track separately
  // of the devices on and off the bridge respectively, for later configuration
  NetDeviceContainer LanDevicesIp;
  NetDeviceContainer LanDevices1;
  NetDeviceContainer * BridgeDevices = new NetDeviceContainer[6];
  // It is easier to iterate the nodes in C++ if we put them into a container
  NodeContainer Lan1;
  Lan1.Add(c1);
  Lan1.Add(c2);
  for (int i = 0; i < 2; i++){
      // install a csma channel between the ith lan1 node and the bridge1 node
      NetDeviceContainer link = csma.Install (NodeContainer (Lan1.Get (i), N1));
      LanDevices1.Add (link.Get (0));
      BridgeDevices[0].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }

  // Repeat for other bridges ///////////////
  NetDeviceContainer LanDevices3;
  NodeContainer Lan3;
  Lan3.Add(c3);
  Lan3.Add(c4);
  for (int i = 0; i < 2; i++){
      NetDeviceContainer link = csma.Install (NodeContainer (Lan3.Get (i), N3));
      LanDevices3.Add (link.Get (0));
      BridgeDevices[2].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }

  NetDeviceContainer LanDevices4;
  NodeContainer Lan4;
  Lan4.Add(c5);
  Lan4.Add(c6);
  for (int i = 0; i < 2; i++){
      NetDeviceContainer link = csma.Install (NodeContainer (Lan4.Get (i), N4));
      LanDevices4.Add (link.Get (0));
      BridgeDevices[3].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }

  NetDeviceContainer LanDevices6;
  NodeContainer Lan6;
  Lan6.Add(c7);
  Lan6.Add(c8);
  for (int i = 0; i < 2; i++){
      NetDeviceContainer link = csma.Install (NodeContainer (Lan6.Get (i), N6));
      LanDevices6.Add (link.Get (0));
      BridgeDevices[5].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }

  // Adding MST Linkes
  NodeContainer BrAll;
  BrAll.Add(N1);
  BrAll.Add(N2);
  BrAll.Add(N3);
  BrAll.Add(N4);
  BrAll.Add(N5);
  BrAll.Add(N6);
  
  for(unsigned int i = 0;i<out.size();i++){
    NetDeviceContainer BridgeLinks;
    NetDeviceContainer BridgeLinks1;
    int x = out[i].second.first - 1;
    int y = out[i].second.second - 1;
    double w = out[i].first/50.0;
    //std::cout << "Link from " << x << " to " << y << "  " << w << " us" << endl;
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", DataRateValue (1000000)); //1Mbps
    csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (w)));
    NetDeviceContainer link = csma.Install (NodeContainer (BrAll.Get(x), BrAll.Get(y)));
    BridgeDevices[x].Add (link.Get (0));
    BridgeDevices[y].Add (link.Get (1));
  }
  // instalation of bridge interfaces
  BridgeHelper bridge; 
  for (int i=0;i<6;i++){
    bridge.Install(BrAll.Get(i),BridgeDevices[i]);
  }
 
  // Add internet stack to the Ci nodes
  NodeContainer LanAll;
  LanAll.Add(c1);
  LanAll.Add(c2);
  LanAll.Add(c3);
  LanAll.Add(c4);
  LanAll.Add(c5);
  LanAll.Add(c6);
  LanAll.Add(c7);
  LanAll.Add(c8);
  InternetStackHelper internet;
  internet.Install (LanAll);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.10.18.00", "255.255.255.0"); //98109718 -> 18
  Ipv4InterfaceContainer nodeInterfaces;
  nodeInterfaces = ipv4.Assign(LanDevicesIp);
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  // Create an OnOff application to send UDP datagrams from node zero to node 1.
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)

  // C1 -> C6
  OnOffHelper onoff1 ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.10.18.6"), port)));
  onoff1.SetConstantRate (DataRate ("10000kb/s"));
  onoff1.SetAttribute("StartTime", TimeValue(Seconds(0.0)));
 
  ApplicationContainer app1 = onoff1.Install (c1);
  // Start the application
  app1.Start (Seconds (0.0));
  app1.Stop (Seconds (10.0));
  
  // C2 -> C7
  OnOffHelper onoff2 ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.10.18.7"), port)));
  onoff2.SetConstantRate (DataRate ("10000kb/s"));
  onoff2.SetAttribute("StartTime", TimeValue(Seconds(0.0)));
 
  ApplicationContainer app2 = onoff2.Install (c2);
  // Start the application
  app2.Start (Seconds (0.0));
  app2.Stop (Seconds (10.0));
  
 
  // Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
 
  //set mobility, position, ... nodes and Bridges
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(-0.0),
                                "MinY", DoubleValue(5.0),
                                "DeltaX", DoubleValue(10.0),
                                "DeltaY", DoubleValue(0.0),
                                "GridWidth", UintegerValue(80.0),
                                "LayoutType", StringValue("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // Mobility for All nodes
  for (int i=0;i<8;i++){
    positionAlloc->Add (Vector (10.0*i-(i/4)*40,30.0*(i/4),0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (LanAll);

  // Mobility for Bridges
  MobilityHelper mobility2;
  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(-0.0),
                                "MinY", DoubleValue(5.0),
                                "DeltaX", DoubleValue(10.0),
                                "DeltaY", DoubleValue(0.0),
                                "GridWidth", UintegerValue(80.0),
                                "LayoutType", StringValue("RowFirst"));

  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();

  for (int i=0;i<6;i++){
    positionAlloc2->Add (Vector (10.0*i,10.0,0.0));
  }
  mobility2.SetPositionAllocator (positionAlloc2);
  mobility2.Install (BrAll);

  //NetAnim for debugging
  AnimationInterface anim ("L2_Part_b.xml");

  // Now, do the actual simulation.
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();

  // Print per flow statistics 
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      //   Simulator::Stops at "second 10".
      if (i->first > 0)
        {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 10.0 / 1000 / 1000  << " Mbps\n";
          std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 10.0 / 1000 / 1000  << " Mbps\n";
          std::cout << "  Delay: " << i->second.delaySum.GetSeconds() / i->second.rxPackets << " s\n";
        }
    }
  
  Simulator::Destroy ();
  NS_LOG_INFO ("Done."); 
   return 0;
}

vector< pair<double, pair<int, int> >> dijkstra(double G[6][6],int n,int startnode) {
   double cost[6][6],distance[6],pred[6];
   int visited[6],count,mindistance,nextnode,i,j;
   vector< pair<double, pair<int, int> >> out; // vector of the tree
   for(i=0;i<n;i++)
      for(j=0;j<n;j++)
   if(G[i][j]==0)
      cost[i][j]=9999; //inf
   else
      cost[i][j]=G[i][j];
   for(i=0;i<n;i++) {
      distance[i]=cost[startnode][i];
      pred[i]=startnode;
      visited[i]=0;
   }
   distance[startnode]=0;
   visited[startnode]=1;
   count=1;
   while(count<n-1) {
      mindistance=9999; //inf
      for(i=0;i<n;i++)
         if(distance[i]<mindistance&&!visited[i]) {
         mindistance=distance[i];
         nextnode=i;
      }
      visited[nextnode]=1;
      for(i=0;i<n;i++)
         if(!visited[i])
      if(mindistance+cost[nextnode][i]<distance[i]) {
         distance[i]=mindistance+cost[nextnode][i];
         pred[i]=nextnode;
      }
      count++;
   }
   for(i=0;i<n;i++)
   if(i!=startnode) {
    j = pred[i];
    //cout << "Link from " << i+1 << " to " << j+1 << "  " << G[i][j] << " m" << endl;
    out.push_back({G[i][j], {i+1, j+1}}); // nodes of the tree
   }
   return out;
}