#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("mini-hw");

int main(int argc, char *argv[])
{
    uint64_t student_number = 96000000;
    CommandLine cmd;
    cmd.AddValue ("StudentNumber", "Enter your student number", student_number);
    cmd.Parse (argc, argv);

    NS_ASSERT_MSG (student_number > 94000000, "Weird Student Number!!!");
    uint64_t rate = student_number - 94000000;

    LogComponentEnable ("mini-hw", LogLevel::LOG_LEVEL_INFO);
    // Uncomment the following line to see the packet logs
    //LogComponentEnable ("PacketSink", LogLevel::LOG_LEVEL_INFO);

    NS_LOG_INFO ("Creating Nodes...");
    Ptr<Node> n0 = CreateObject<Node> ();
    Ptr<Node> n1 = CreateObject<Node> ();
    NodeContainer nodes = NodeContainer (n0, n1);

    NS_LOG_INFO ("Creating a p2p link between the two nodes...");    
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (rate)));
    p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
    NetDeviceContainer devices = p2p.Install (nodes);

    NS_LOG_INFO ("Installing internet stack...");
    InternetStackHelper internet;
    Ipv4GlobalRoutingHelper globalRoutingHelper;
    internet.SetRoutingHelper (globalRoutingHelper);
    internet.Install(nodes);

    NS_LOG_INFO ("Assigning IP...");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.0.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(devices);

    NS_LOG_INFO ("Calculating Routes...");
    globalRoutingHelper.PopulateRoutingTables ();

    NS_LOG_INFO ("Adding mobility model to the nodes...");
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (-0.0),
                                 "MinY", DoubleValue (5.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (0.0),
                                 "GridWidth", UintegerValue (50.0),
                                 "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);

    // Initializing packet sniffing and animations
    p2p.EnablePcapAll ("mini-hw");
    AnimationInterface anim ("mini-hw.xml");

    // Some constants
    uint32_t port = 4500;
    uint32_t packetSize = 512;

    NS_LOG_INFO ("Installing sender application...");
    BulkSendHelper bulk ("ns3::TcpSocketFactory", InetSocketAddress (i.GetAddress (1), port));
    bulk.SetAttribute ("SendSize", UintegerValue (packetSize));
    ApplicationContainer senderApps = bulk.Install (n0);

    NS_LOG_INFO ("Installing receiver application...");
    PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer receiverApps = sink.Install (n1);

    // Starting applications
    senderApps.Start (Seconds (0.0));
    receiverApps.Start (Seconds (0.0));

    NS_LOG_INFO ("Running the simulation...");
    Simulator::Stop (Seconds (2.0));
    Simulator::Run ();
    NS_LOG_INFO ("Done!");

    Simulator::Destroy ();

    Ptr<PacketSink> pktSink = DynamicCast<PacketSink> (receiverApps.Get(0));
    std::cout << std::endl << "*** Summary (User: " << getenv("USER") << ") ***" << std::endl;
    std::cout << "Total received packets: " << pktSink->GetTotalRx () / packetSize << std::endl;
    std::cout <<  "Throughput: " << pktSink->GetTotalRx () * 8 / 1000000.0 / 2.0 << "Mbps" << std::endl;
    
    return 0;
}
