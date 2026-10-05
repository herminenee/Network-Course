/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    uint32_t nClients = 1;
    uint32_t nPackets = 1;
    CommandLine cmd(__FILE__);
    cmd.AddValue("nClients", "Number of clients", nClients);
    cmd.AddValue("nPackets", "Packets per client", nPackets);
    cmd.Parse(argc, argv);
    RngSeedManager::SetSeed(1234);
    NS_ABORT_MSG_IF(nClients < 1 || nClients > 5, "nClients must be 1-5");
    NS_ABORT_MSG_IF(nPackets < 1 || nPackets > 5, "nPackets must be 1-5");
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(nClients + 1);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));


    InternetStackHelper stack;

    // Disableing IPv6 because it is not necessary to show what we want to demonstrate here.
    // Note:Normal networks typically have both IPv4 and IPv6 enabled.
    stack.SetIpv6StackInstall(false);

    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

	for (uint32_t i = 1; i <= nClients; ++i)
     {
    NodeContainer link(nodes.Get(i), nodes.Get(0));
		NetDeviceContainer devices = pointToPoint.Install(link);
		address.Assign(devices);
		address.NewNetwork();	
     }
    UdpEchoServerHelper echoServer(9);
	echoServer.SetAttribute("Port", UintegerValue(15));

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));
    serverApps.Start(Seconds(1));
    serverApps.Stop(Seconds(20));

    UdpEchoClientHelper echoClient(Ipv4Address("10.1.1.2"), 15);
    echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    Ptr<UniformRandomVariable> startTime = CreateObject<UniformRandomVariable>();
    for (uint32_t i = 1; i <= nClients; ++i)
 {
    ApplicationContainer clientApps = echoClient.Install(nodes.Get(i));
    clientApps.Start(Seconds(startTime->GetValue(2, 7)));
    clientApps.Stop(Seconds(20));
}
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Simulator::Stop(Seconds(20));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
