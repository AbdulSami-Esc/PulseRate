#include <iostream>
#include <string>
#include "../include/MapLoader.h"
#include "../include/Trie.h"
#include "../include/RobotFleet.h"
#include "../include/DispatchQueue.h"
#include "../include/Dispatcher.h"
#include "../include/Simulator.h"
#include "../include/CLI.h"

// Full path to the data folder (forward slashes work on Windows too)
const std::string DATA_DIR = "D:/PulseRate/data";

int main(int argc, char* argv[]) {
    // optional: .\pulseroute <other_folder> overrides the default
    std::string dir = (argc > 1) ? argv[1] : DATA_DIR;

    Graph graph;
    HashTable names;
    std::string err;
    std::string nodesPath = dir + "/nodes.csv";
    std::string edgesPath = dir + "/edges.csv";

    if (!loadMap(nodesPath, edgesPath, graph, names, err)) {
        std::cout << "Map error: " << err << "\n";
        std::cout << "Tried: " << nodesPath << "\n";
        return 1;
    }
    std::cout << "Loaded " << graph.nodeCount() << " rooms and "
              << graph.edgeCount() << " corridor directions from " << dir << "\n";

    Trie trie;
    for (int i = 0; i < graph.nodeCount(); i++) trie.insert(graph.getNode(i).name);

    RobotFleet fleet(graph);
    int start = 0;
    names.get("Pharmacy", start);
    fleet.addRobot(start);
    fleet.addRobot(start);

    DispatchQueue queue;
    Dispatcher dispatcher(queue, fleet);
    Simulator sim(queue, dispatcher, fleet);

    CLI cli(graph, names, trie, queue, fleet, sim);
    cli.run();
    return 0;
}