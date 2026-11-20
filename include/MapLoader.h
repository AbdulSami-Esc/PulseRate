#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <string>
#include <fstream>
#include <cstdlib>
#include "DynamicArray.h"
#include "HashTable.h"
#include "Graph.h"

// splits a comma-separated line into fields while stripping \r
inline DynamicArray<std::string> splitCSV(const std::string& line) {
    DynamicArray<std::string> parts;
    std::string cur = "";

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == ',') {
            parts.push_back(cur);
            cur = "";
        } else if (c != '\r') {
            cur += c;
        }
    }
    parts.push_back(cur);
    return parts;
}

// reads nodes and edges CSV files to populate the graph and lookup table
inline bool loadMap(const std::string& nodesFile,
                    const std::string& edgesFile,
                    Graph& graph,
                    HashTable& names,
                    std::string& error) {

    std::ifstream nf(nodesFile);
    if (!nf.is_open()) {
        error = "Cannot open " + nodesFile;
        return false;
    }

    std::string line;
    std::getline(nf, line);

    while (std::getline(nf, line)) {
        if (line.empty() || line == "\r") {
            continue;
        }

        DynamicArray<std::string> parts = splitCSV(line);
        if (parts.size() != 4) {
            error = "Invalid node format in " + nodesFile;
            return false;
        }

        int id = std::atoi(parts[0].c_str());
        if (id != graph.nodeCount()) {
            error = "Node IDs must be sequential starting from 0";
            return false;
        }

        if (names.contains(parts[1])) {
            error = "Duplicate node name: " + parts[1];
            return false;
        }

        int floor = std::atoi(parts[3].c_str());
        graph.addNode(parts[1], parts[2], floor);
        names.put(parts[1], id);
    }

    std::ifstream ef(edgesFile);
    if (!ef.is_open()) {
        error = "Cannot open " + edgesFile;
        return false;
    }

    std::getline(ef, line);

    while (std::getline(ef, line)) {
        if (line.empty() || line == "\r") {
            continue;
        }

        DynamicArray<std::string> parts = splitCSV(line);
        if (parts.size() != 4) {
            error = "Invalid edge format in " + edgesFile;
            return false;
        }

        int from = std::atoi(parts[0].c_str());
        int to = std::atoi(parts[1].c_str());
        int sec = std::atoi(parts[2].c_str());
        int count = graph.nodeCount();

        if (from < 0 || from >= count || to < 0 || to >= count || sec <= 0) {
            error = "Invalid edge node ID or weight in " + edgesFile;
            return false;
        }

        bool blocked = (parts[3] == "blocked");
        graph.addCorridor(from, to, sec, blocked);
    }

    return true;
}

#endif