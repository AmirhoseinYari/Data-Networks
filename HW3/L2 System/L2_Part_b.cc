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
#include<bits/stdc++.h>
 
using namespace ns3;
using namespace std;
 
NS_LOG_COMPONENT_DEFINE ("CsmaBridgeOneHopExample");

// Creating shortcut for an integer pair
typedef  pair<int, int> iPair;
  
// Structure to represent a graph
struct Graph
{
    int V, E;
    vector< pair<int, iPair> > edges;
  
    // Constructor
    Graph(int V, int E)
    {
        this->V = V;
        this->E = E;
    }
  
    // Utility function to add an edge
    void addEdge(int u, int v, int w)
    {
        edges.push_back({w, {u, v}});
    }
  
    // Function to find MST using Kruskal's
    // MST algorithm
    vector<pair<double, pair<int, int>>> kruskalMST();
};
  
// To represent Disjoint Sets
struct DisjointSets
{
    int *parent, *rnk;
    int n;
  
    // Constructor.
    DisjointSets(int n)
    {
        // Allocate memory
        this->n = n;
        parent = new int[n+1];
        rnk = new int[n+1];
  
        // Initially, all vertices are in
        // different sets and have rank 0.
        for (int i = 0; i <= n; i++)
        {
            rnk[i] = 0;
  
            //every element is parent of itself
            parent[i] = i;
        }
    }
  
    // Find the parent of a node 'u'
    // Path Compression
    int find(int u)
    {
        /* Make the parent of the nodes in the path
           from u--> parent[u] point to parent[u] */
        if (u != parent[u])
            parent[u] = find(parent[u]);
        return parent[u];
    }
  
    // Union by rank
    void merge(int x, int y)
    {
        x = find(x), y = find(y);
  
        /* Make tree with smaller height
           a subtree of the other tree  */
        if (rnk[x] > rnk[y])
            parent[y] = x;
        else // If rnk[x] <= rnk[y]
            parent[x] = y;
  
        if (rnk[x] == rnk[y])
            rnk[y]++;
    }
};
  
 /* Functions returns vector of the MST*/ 
  
vector< pair<double, pair<int, int> >> Graph::kruskalMST()
{
    double mst_wt = 0; // Initialize result
    vector< pair<double, pair<int, int> >> out; // vector of the tree

    // Sort edges in increasing order on basis of cost
    sort(edges.begin(), edges.end());
  
    // Create disjoint sets
    DisjointSets ds(V);
  
    // Iterate through all sorted edges
    vector< pair<int, iPair> >::iterator it;
    for (it=edges.begin(); it!=edges.end(); it++)
    {
        int u = it->second.first;
        int v = it->second.second;
        double w = it->first;
  
        int set_u = ds.find(u);
        int set_v = ds.find(v);
  
        // Check if the selected edge is creating
        // a cycle or not (Cycle is created if u
        // and v belong to same set)
        if (set_u != set_v)
        {
            // Current edge will be in the MST
            // so print it
            //cout << u << " ---- " << v << endl;
            out.push_back({w, {u, v}}); // nodes of the tree
  
            // Update MST weight
            mst_wt += it->first;
  
            // Merge two sets
            ds.merge(set_u, set_v);
        }
    }
  
    return out; //vector, links of tree
}
 
int
main (int argc, char **argv)
{
  int V = 6, E = 12;
  Graph g(V, E);
  
  //  making the graph
  g.addEdge(1, 2, 50);
  g.addEdge(1, 3, 50);
  g.addEdge(1, 4, 186.6);

  g.addEdge(2, 3, 50);
  g.addEdge(2, 5, 100);
  g.addEdge(2, 6, 145.5);

  g.addEdge(3, 4, 193.2);
  g.addEdge(3, 5, 145.5);
  g.addEdge(3, 6, 186.6);

  g.addEdge(4, 5, 50);
  g.addEdge(4, 6, 50);

  g.addEdge(5, 6, 50);

  //cout << "Edges of MST are \n";
  vector< pair <double, pair<int, int> >> out = g.kruskalMST();
    
  std::cout << "the links are : "<<"\n";
  for(unsigned int i = 0;i<out.size();i++){
      std::cout << out[i].second.first << " -- " << out[i].first << " -- " << out[i].second.second  << endl;
  }

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
  CommandLine cmd;
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
  NetDeviceContainer LanDevicesIp;
  NetDeviceContainer LanDevices1;
  NetDeviceContainer * BridgeDevices = new NetDeviceContainer[6];
  // It is easier to iterate the nodes in C++ if we put them into a container
  NodeContainer Lan1;
  Lan1.Add(c1);
  Lan1.Add(c2);
  for (int i = 0; i < 2; i++)
    {
      // install a csma channel between the ith lan1 node and the bridge1 node
      NetDeviceContainer link = csma.Install (NodeContainer (Lan1.Get (i), N1));
      LanDevices1.Add (link.Get (0));
      BridgeDevices[0].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }
  
  //
  // Now, Create the bridge netdevice, which will do the packet switching.  The
  // bridge lives on the node bridge1 and bridges together the topBridgeDevices
  // which are the three CSMA net devices on the node in the diagram above.
  //
  BridgeHelper bridge; 

  // Repeat for other bridges ///////////////
  NetDeviceContainer LanDevices3;
  //NetDeviceContainer BridgeDevices3;
  NodeContainer Lan3;
  Lan3.Add(c3);
  Lan3.Add(c4);
  for (int i = 0; i < 2; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (Lan3.Get (i), N3));
      LanDevices3.Add (link.Get (0));
      BridgeDevices[2].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }

  NetDeviceContainer LanDevices4;
  NodeContainer Lan4;
  Lan4.Add(c5);
  Lan4.Add(c6);
  for (int i = 0; i < 2; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (Lan4.Get (i), N4));
      LanDevices4.Add (link.Get (0));
      BridgeDevices[3].Add (link.Get (1));
      LanDevicesIp.Add (link.Get (0));
    }

  NetDeviceContainer LanDevices6;
  NodeContainer Lan6;
  Lan6.Add(c7);
  Lan6.Add(c8);
  for (int i = 0; i < 2; i++)
    {
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
    std::cout << "Link from " << x << " to " << y << "  " << w << " us" << endl;
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", DataRateValue (1000000)); //1Mbps
    csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (w)));
    NetDeviceContainer link = csma.Install (NodeContainer (BrAll.Get(x), BrAll.Get(y)));
    BridgeDevices[x].Add (link.Get (0));
    BridgeDevices[y].Add (link.Get (1));
  }

  bridge.Install(BrAll.Get(0),BridgeDevices[0]);
  bridge.Install(BrAll.Get(1),BridgeDevices[1]);
  bridge.Install(BrAll.Get(2),BridgeDevices[2]);
  bridge.Install(BrAll.Get(3),BridgeDevices[3]);
  bridge.Install(BrAll.Get(4),BridgeDevices[4]);
  bridge.Install(BrAll.Get(5),BridgeDevices[5]);


 
  // Add internet stack to the router nodes
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
  ipv4.SetBase ("10.10.18.00", "255.255.255.0");
  Ipv4InterfaceContainer nodeInterfaces;
  nodeInterfaces = ipv4.Assign(LanDevicesIp);
  //ipv4.Assign (Lan1);

 
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
  // C1 -> C6
  OnOffHelper onoff1 ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.10.18.6"), port)));
  onoff1.SetConstantRate (DataRate ("1000kb/s"));
  onoff1.SetAttribute("StartTime", TimeValue(Seconds(0.0)));
 
  ApplicationContainer app1 = onoff1.Install (c1);
  // Start the application
  app1.Start (Seconds (0.0));
  app1.Stop (Seconds (10.0));
  
  // C2 -> C7
  OnOffHelper onoff2 ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("10.10.18.7"), port)));
  onoff2.SetConstantRate (DataRate ("1000kb/s"));
  onoff2.SetAttribute("StartTime", TimeValue(Seconds(0.0)));
 
  ApplicationContainer app2 = onoff2.Install (c2);
  // Start the application
  app2.Start (Seconds (0.0));
  app2.Stop (Seconds (10.0));
  
 
  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  std::cout << "debug"<<endl;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  NS_LOG_INFO ("Configure Tracing.");
  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "csma-bridge-one-hop.tr"
  //
  AsciiTraceHelper ascii;
  //csma.EnableAsciiAll (ascii.CreateFileStream ("csma-bridge-one-hop.tr"));
 
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
    positionAlloc->Add (Vector (10.0*i-(i/4)*40,10.0*(i/4),0.0));
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
    positionAlloc2->Add (Vector (10.0*i,20.0,0.0));
  }
  mobility2.SetPositionAllocator (positionAlloc2);
  mobility2.Install (BrAll);

  //
  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     csma-bridge-one-hop-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  //
  //csma.EnablePcapAll ("csma-bridge-one-hop", false);
  AnimationInterface anim ("L2_Part_b.xml");
  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();

  // 10. Print per flow statistics 
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      // first 2 FlowIds are for ECHO apps, we don't want to display them
      //
      // Duration for throughput measurement is 9.0 seconds, since
      //   StartTime of the OnOffApplication is at about "second 1"
      // and
      //   Simulator::Stops at "second 10".
      if (i->first > 0)
        {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
          std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
        }
    }
  
  Simulator::Destroy ();
  NS_LOG_INFO ("Done."); 
}