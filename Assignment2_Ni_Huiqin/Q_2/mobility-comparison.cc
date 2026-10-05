#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MobilityComparison");

int
main(int argc, char* argv[])
{
    uint32_t numNodes = 100;
    std::string mobility = "walk";
    double duration = 400.0;
    double minSpeed = 4.0;
    double maxSpeed = 6.0;
    double pause = 2.0;

    CommandLine cmd(__FILE__);
    cmd.AddValue("numNodes", "Number of nodes", numNodes);
    cmd.AddValue("mobility", "walk or waypoint", mobility);
    cmd.AddValue("duration", "Simulation duration", duration);
    cmd.AddValue("minSpeed", "Minimum speed", minSpeed);
    cmd.AddValue("maxSpeed", "Maximum speed", maxSpeed);
    cmd.AddValue("pause", "Waypoint pause time", pause);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(123456789);

    NodeContainer nodes;
    nodes.Create(numNodes);

    Ptr<RandomRectanglePositionAllocator> positionAllocator =
        CreateObject<RandomRectanglePositionAllocator>();

    positionAllocator->SetAttribute(
        "X",
        StringValue("ns3::UniformRandomVariable[Min=0.0|Max=80.0]"));

    positionAllocator->SetAttribute(
        "Y",
        StringValue("ns3::UniformRandomVariable[Min=0.0|Max=80.0]"));

    MobilityHelper mobilityHelper;
    mobilityHelper.SetPositionAllocator(positionAllocator);

    std::ostringstream speedStream;
    speedStream << "ns3::UniformRandomVariable[Min=" << minSpeed
                << "|Max=" << maxSpeed << "]";

    if (mobility == "walk")
    {
        mobilityHelper.SetMobilityModel(
            "ns3::RandomWalk2dMobilityModel",
            "Mode",
            EnumValue(RandomWalk2dMobilityModel::MODE_TIME),
            "Time",
            TimeValue(Seconds(2.0)),
            "Speed",
            StringValue(speedStream.str()),
            "Bounds",
            RectangleValue(Rectangle(0.0, 80.0, 0.0, 80.0)));
    }

    else if (mobility == "waypoint")
    {
        std::ostringstream pauseStream;
        pauseStream << "ns3::ConstantRandomVariable[Constant="
                    << pause << "]";

        mobilityHelper.SetMobilityModel(
            "ns3::RandomWaypointMobilityModel",
            "Speed",
            StringValue(speedStream.str()),
            "Pause",
            StringValue(pauseStream.str()),
            "PositionAllocator",
            PointerValue(positionAllocator));
    }
    else
    {
        NS_FATAL_ERROR("mobility must be walk or waypoint");
    }

    mobilityHelper.Install(nodes);

    Simulator::Stop(Seconds(duration));
    Simulator::Run();

    std::cout << "Node,X,Y,DistanceFromCenter" << std::endl;
    double totalDistance = 0.0;

    for (uint32_t i = 0; i < numNodes; ++i)
    {
        Ptr<MobilityModel> model =
            nodes.Get(i)->GetObject<MobilityModel>();

        Vector position = model->GetPosition();

        double dx = position.x - 40.0;
        double dy = position.y - 40.0;
        double distance = std::sqrt(dx * dx + dy * dy);

        totalDistance += distance;

        std::cout << i << ","
                  << std::fixed << std::setprecision(4)
                  << position.x << ","
                  << position.y << ","
                  << distance << std::endl;
    }

    std::cout << "AverageDistanceFromCenter,"
              << std::fixed << std::setprecision(4)
              << totalDistance / numNodes << std::endl;

    Simulator::Destroy();
    return 0;
}
