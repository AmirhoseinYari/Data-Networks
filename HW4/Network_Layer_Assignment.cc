// This is base structure of the code needed for your network layer assignment. Obviously the code is not complete and you have to do the rest. Please pay attention to the comments.

// Initialization
// At first you have to include the modules you need as follows:
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/animation-interface.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;
int main (int argc, char *argv[])
{
	// Initialization
	// The next line enables you showing some messages while running the code:
	
	Time::SetResolution (Time::NS);
	NS_LOG_COMPONENT_DEFINE ("Network_Layer_Assignment");
	
	// At the beginning of the main function, you have to configure some parameters as follows:
	
	uint32_t PacketSize = 512; // bytes
	std::string DataRate ("1Mbps");
	uint16_t num_Nodes = 9;
	uint16_t UDPport = 9;
	bool tracing = false;
	
	// While using NS3, you can use "CommandLine" object to override any of the default parameters, including the above parameters, at run-time:
	
	CommandLine cmd;
	cmd.AddValue ("PacketSize", "size of application packet sent", PacketSize);
	cmd.AddValue ("DataRate", "rate of pacekts sent", DataRate);
	cmd.AddValue ("tracing", "turn on ascii and pcap tracing", tracing);
	cmd.Parse (argc, argv);
	
	// The following lines override the default values of the OnOffApplication used in data generation:
	
	Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue(PacketSize));
	Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (DataRate));
	Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents",BooleanValue(true));
	
	// Finally, you should enable Packet Meta data for animator and the name of file for animation output:
	
	ns3::PacketMetadata::Enable();
	std::string animFile = "Network_Layer_Assignment_Animation.xml" ;

	// Network Topology
	// In the next step, you have to create the network topology. Network nodes are stored in a container class called "NodeContainer":
	
	NodeContainer nodes;
	nodes.Create (num_Nodes);
	
	// Create Links
	// Now, you should group nodes to make links between them.
	// YOUR CODE STARTS HERE
	// We added the first link. You have to add other links yourself.
	NodeContainer AB = NodeContainer (nodes.Get(0) , nodes.Get(1));
	// YOUR CODE ENDS HERE
	
	// Now you should assign bandwidth and delay to each link.
	
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
	p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
	
	// Install p2p on each link
	// Now you have to install p2p on each link.
	// YOUR CODE STARTS HERE
	// We installed the first p2p. You have to install the others yourself.
	NetDeviceContainer dAB = p2p.Install (AB);
	// YOUR CODE ENDS HERE

	// Network Layer Parameters
	// In this part, you will configure the network parameters. First, you have to install IPv4 on the network via using InternetStackHelper. This helper uses static routing as its first priority algorithm for routing; therefore, we redefine the priority: ####

	NS_LOG_INFO("Setting routing protocols");
	Ipv4StaticRoutingHelper staticRouting;
	Ipv4GlobalRoutingHelper globalRouting;
	Ipv4ListRoutingHelper list;
	list.Add(staticRouting,0);
	list.Add(globalRouting,10);
	
	// Install network stacks on the nodes
	InternetStackHelper internet;
	internet.SetRoutingHelper(list);
	internet.Install(nodes);
	
	// Now, you should assign IP to each node for routing and also assign the metric of each link to it.
	// YOUR CODE STARTS HERE
	// We have done this assignments for the link between "A" and "B". You have to do the rest youreself.
	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer iAB = ipv4.Assign (dAB);
	iAB.SetMetric(0,29);
	iAB.SetMetric(1,29);
	// YOUR CODE ENDS HERE
	
	// Finally, you have to add the following code to initialize routing database and set up the routing tables in the nodes:
	
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	// Building Flows
	// After doing the preceding part, now it is time to determine the application with which the nodes will function. You should install UDP application on node "A" and "E". First, use the following code:

	PacketSinkHelper UDPsink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), UDPport));
	ApplicationContainer App;
	NodeContainer SourceNode = NodeContainer (nodes.Get (0));
	NodeContainer SinkNode = NodeContainer (nodes.Get (4)); // A = 0, B = 1, C = 2, D = 3, E = 4, ...
	
	// To Create a UDP packet sink to receive these packets use the following code:
	
	App = UDPsink.Install (SinkNode);
	App.Start (Seconds (0.0));
	App.Stop (Seconds (10.0));
	Address E_Address(InetSocketAddress(iEF.GetAddress (0), UDPport));
	
	// To Create a UDP packet source to send these packets use the following code:
	
	OnOffHelper UDPsource ("ns3::UdpSocketFactory", E_Address);
	UDPsource.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	UDPsource.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	UDPsource.Install(SourceNode);
	App.Start (Seconds (1.0));
	App.Stop (Seconds (10.0));

	
	// Analysis
	// You should determine your nodes places for NetAnim. Do it with the following code:
	AnimationInterface anim (animFile);
	// YOUR CODE STARTS HERE
	// We defined first two nodes on the animation. You have to define the rest yourself.
	Ptr <Node> n = nodes.Get (0);
	anim.SetConstantPosition (n, 0, 20);
	n = nodes.Get(1);
	anim.SetConstantPosition(n,10,10);
	// YOUR CODE ENDS HERE
	
	// You can use the following code to generate the xml file used in netAnim.
	if (tracing == true)
	{
	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll (ascii.CreateFileStream ("Network_Layer_Assignment.tr"));
	p2p.EnablePcapAll ("Network_Layer_Assignment");
	}
	
	// To print routing tables you have to add the following code:
	Ptr <OutputStreamWrapper> stream1 = Create<OutputStreamWrapper> ("Routing_Table_before_SetDown", std::ios::out);
	Ipv4GlobalRoutingHelper helper2;
	helper2.PrintRoutingTableAllAt(Seconds(2.0),stream1);

	// This part can be used to shutdown links
	Ptr<Node> node1=nodes.Get(8);
	Ptr<Ipv4> ipv41=node1->GetObject<Ipv4>();
	Simulator::Schedule(Seconds(3),&Ipv4::SetDown,ipv41,1);
	Simulator::Schedule(Seconds(3),&Ipv4::SetDown,ipv41,2);
	Simulator::Schedule(Seconds(3),&Ipv4::SetDown,ipv41,3);
	Simulator::Schedule(Seconds(3),&Ipv4::SetDown,ipv41,4);
	Simulator::Schedule(Seconds(3),&Ipv4::SetDown,ipv41,5);
	Simulator::Schedule(Seconds(3),&Ipv4::SetDown,ipv41,6);
	
	// Printing the routing table after SetDown
	Ptr<OutputStreamWrapper> stream2 = Create<OutputStreamWrapper> ("Routing_Table_after_SetDown", std::ios::out);
	helper2.PrintRoutingTableAllAt(Seconds(4.0),stream2);
	
	// You should just add the following two lines to your code before running it:
	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	
	// After everything is done, you have to destroy the simulation using the following command:
	//Simulator::Destroy();

	
	return 0;

}
