#ifndef GRAPH_H
#define GRAPH_H
#include <string>
#include <iostream>
#include <stdexcept>
#include "DynamicArray.h"

struct Node {
    int id;
    std::string name;
    std::string type;
    int floor;
};

struct Edge {
    int to;
    int baseWeight;
    int weight;
    bool blocked;
    double risk;
};

class Graph {
private:
    DynamicArray<Node> nodes;
    DynamicArray< DynamicArray<Edge> > adj;
    int numEdges;

public:
    Graph() : numEdges(0) {}

    int addNode(const std::string& name, const std::string& type, int floor) {
        int id = nodes.size();
        Node n = {id, name, type, floor};
        nodes.push_back(n);
        adj.push_back(DynamicArray<Edge>());
        return id;
    }

    // adds a single directed edge after checking node bounds
    void addEdge(int from, int to, int seconds, bool blocked = false) {
        if (from < 0 || from >= nodes.size() || to < 0 || to >= nodes.size()) {
            throw std::out_of_range("Node id out of range");
        }
        Edge e = {to, seconds, seconds, blocked};
        adj[from].push_back(Edge{to, seconds, seconds, blocked, 0.0});
        numEdges++;
    }

    void addCorridor(int a, int b, int seconds, bool blocked = false) {
        addEdge(a, b, seconds, blocked);
        addEdge(b, a, seconds, blocked);
    }

    int nodeCount() const { return nodes.size(); }
    int edgeCount() const { return numEdges; }

    const Node& getNode(int id) const {
        if (id < 0 || id >= nodes.size()) {
            throw std::out_of_range("Node id out of range");
        }
        return nodes[id];
    }

    const DynamicArray<Edge>& neighbors(int id) const {
        if (id < 0 || id >= nodes.size()) {
            throw std::out_of_range("Node id out of range");
        }
        return adj[id];
    }

    DynamicArray<Edge>& neighbors(int id) {
        if (id < 0 || id >= nodes.size()) {
            throw std::out_of_range("Node id out of range");
        }
        return adj[id];
    }

    // searches for a directed edge from -> to and returns its pointer
    Edge* findEdge(int from, int to) {
        if (from < 0 || from >= nodes.size()) {
            return nullptr;
        }
        for (int i = 0; i < adj[from].size(); i++) {
            if (adj[from][i].to == to) {
                return &adj[from][i];
            }
        }
        return nullptr;
    }

    // blocks or unblocks both directions of a corridor
    bool setBlocked(int a, int b, bool blocked) {
        Edge* e1 = findEdge(a, b);
        Edge* e2 = findEdge(b, a);
        if (e1 == nullptr || e2 == nullptr) {
            return false;
        }
        e1->blocked = blocked;
        e2->blocked = blocked;
        return true;
    }

    // uses BFS to count reachable nodes from start ignoring blocked edges
    int countReachable(int start) const {
        int n = nodes.size();
        if (start < 0 || start >= n) {
            return 0;
        }

        DynamicArray<bool> vis;
        for (int i = 0; i < n; i++) {
            vis.push_back(false);
        }

        DynamicArray<int> q;
        int head = 0;

        q.push_back(start);
        vis[start] = true;
        int count = 1;

        while (head < q.size()) {
            int cur = q[head++];
            for (int i = 0; i < adj[cur].size(); i++) {
                Edge e = adj[cur][i];
                if (!e.blocked && !vis[e.to]) {
                    vis[e.to] = true;
                    q.push_back(e.to);
                    count++;
                }
            }
        }

        return count;
    }

    void print() const {
        for (int i = 0; i < nodes.size(); i++) {
            std::cout << nodes[i].id << " " << nodes[i].name << " ->";
            for (int j = 0; j < adj[i].size(); j++) {
                std::cout << " " << adj[i][j].to << "(" << adj[i][j].weight << "s)";
            }
            std::cout << "\n";
        }
    }
};

#endif