/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nPackets = 1;
    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("nPackets", "Packets to send", nPackets);
    cmd.Parse(argc, argv);
    RngSeedManager::SetSeed(1234);
NS_ABORT_MSG_IF(nPackets < 1 || nPackets > 20, "nPackets must be 1-20");

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma = nCsma == 0 ? 1 : nCsma;

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));
    csmaNodes.Create(nCsma);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    InternetStackHelper stack;

    // Disabling IPv6 because it is not necessary to show what we want to demonstrate here.
    // Note: Normal networks typically have both IPv4 and IPv6 enabled.
    stack.SetIpv6StackInstall(false);

    stack.Install(p2pNodes.Get(0));
    stack.Install(csmaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    NodeContainer newServer;
    newServer.Create(1);
    NodeContainer newLink(csmaNodes.Get(nCsma), newServer.Get(0));
    NetDeviceContainer newDevices = pointToPoint.Install(newLink);
    stack.Install(newServer);
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer newInterfaces = address.Assign(newDevices);
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(newServer.Get(0));
    serverApps.Start(Seconds(1));
    serverApps.Stop(Seconds(25));
    UdpEchoClientHelper echoClient(newInterfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2));
    clientApps.Stop(Seconds(25));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pointToPoint.EnablePcapAll("second");
    csma.EnablePcap("second", csmaDevices.Get(1), true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
