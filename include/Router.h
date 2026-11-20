#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include "Graph.h"
#include "HashTable.h"
#include "Dijkstra.h"

class Router {
private:
    const Graph& graph;
    const HashTable& names;

public:
    Router(const Graph& g, const HashTable& n) : graph(g), names(n) {}

    // looks up room names and finds the shortest path between them
    RouteResult route(const std::string& from, const std::string& to) const {
        int fromId = -1;
        int toId = -1;

        if (!names.get(from, fromId) || !names.get(to, toId)) {
            RouteResult res;
            res.found = false;
            res.totalTime = -1;
            return res;
        }

        return shortestPath(graph, fromId, toId);
    }

    // formats a route result into a readable string with node names and time
    std::string describe(const RouteResult& result) const {
        if (!result.found) {
            return "No route found";
        }

        std::string out = "";
        for (int i = 0; i < result.path.size(); i++) {
            if (i > 0) {
                out += " -> ";
            }
            int id = result.path[i];
            out += graph.getNode(id).name;
        }

        out += " (" + std::to_string(result.totalTime) + "s)";
        return out;
    }
};

#endif