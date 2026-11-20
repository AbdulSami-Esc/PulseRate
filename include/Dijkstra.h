#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdexcept>
#include "DynamicArray.h"
#include "MinHeap.h"
#include "Graph.h"

const int INF = 1000000000;

struct RouteResult {
    bool found;
    int totalTime;
    DynamicArray<int> path;
};

// traces backwards from target to source using prev and reverses the order
inline DynamicArray<int> buildPath(const DynamicArray<int>& prev, int source, int target) {
    DynamicArray<int> path;
    int cur = target;
    while (cur != -1) {
        path.push_back(cur);
        cur = prev[cur];
    }
    path.reverse();
    return path;
}

// finds the fastest route between source and target using Dijkstra's algorithm
inline RouteResult shortestPath(const Graph& graph, int source, int target,
                                bool stopAtTarget = true) {
    int n = graph.nodeCount();

    if (source < 0 || source >= n || target < 0 || target >= n) {
        throw std::out_of_range("Node id out of range");
    }

    DynamicArray<int> dist;
    DynamicArray<int> prev;
    DynamicArray<bool> visited;

    for (int i = 0; i < n; i++) {
        dist.push_back(INF);
        prev.push_back(-1);
        visited.push_back(false);
    }
    dist[source] = 0;

    MinHeap pq;
    pq.push(0, source);

    while (!pq.empty()) {
        auto item = pq.pop();
        int u = item.id;

        if (visited[u]) {
            continue;
        }
        visited[u] = true;

        if (stopAtTarget && u == target) {
            break;
        }

        const DynamicArray<Edge>& edges = graph.neighbors(u);
        for (int i = 0; i < edges.size(); i++) {
            Edge e = edges[i];
            if (e.blocked) {
                continue;
            }
            int newDist = dist[u] + e.weight;
            if (newDist < dist[e.to]) {
                dist[e.to] = newDist;
                prev[e.to] = u;
                pq.push(newDist, e.to);
            }
        }
    }

    RouteResult result;
    if (dist[target] == INF) {
        result.found = false;
        result.totalTime = -1;
    } else {
        result.found = true;
        result.totalTime = dist[target];
        result.path = buildPath(prev, source, target);
    }

    return result;
}

#endif