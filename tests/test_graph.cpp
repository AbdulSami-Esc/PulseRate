#include <iostream>
#include <string>
#include <stdexcept>
#include "../include/Graph.h"

void check(bool condition, const std::string& name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
}

// runs unit tests for Graph operations and BFS reachability
int main() {
    Graph g;

    int id0 = g.addNode("Pharmacy", "room", 0);
    int id1 = g.addNode("Hall_A", "junction", 0);
    int id2 = g.addNode("Hall_B", "junction", 1);
    int id3 = g.addNode("ICU_1", "icu", 1);
    check(id0 == 0 && id1 == 1 && id2 == 2 && id3 == 3 && g.nodeCount() == 4, "Test 1: addNode and nodeCount");

    g.addCorridor(0, 1, 10);
    check(g.edgeCount() == 2, "Test 2: addCorridor creates 2 directed edges");

    g.addCorridor(1, 2, 20);
    g.addCorridor(2, 3, 30);

    const DynamicArray<Edge>& nb = g.neighbors(1);
    bool nbOk = (nb.size() == 2 && nb[0].to == 0 && nb[0].weight == 10 && nb[1].to == 2 && nb[1].weight == 20);
    check(nbOk, "Test 3: neighbors returns right targets and weights");

    check(g.findEdge(0, 1) != nullptr && g.findEdge(0, 3) == nullptr, "Test 4: findEdge existing and missing");

    g.setBlocked(1, 2, true);
    Edge* e1 = g.findEdge(1, 2);
    Edge* e2 = g.findEdge(2, 1);
    bool blockedBoth = (e1 != nullptr && e2 != nullptr && e1->blocked && e2->blocked);

    g.setBlocked(1, 2, false);
    bool unblockedBoth = (!e1->blocked && !e2->blocked);
    check(blockedBoth && unblockedBoth, "Test 5: setBlocked blocks and unblocks both directions");

    bool caught = false;
    try {
        g.addEdge(0, 99, 15);
    } catch (const std::out_of_range&) {
        caught = true;
    }
    check(caught, "Test 6: addEdge throws on invalid node id");

    int reachBefore = g.countReachable(0);
    g.setBlocked(1, 2, true);
    int reachAfter = g.countReachable(0);
    check(reachBefore == 4 && reachAfter == 2, "Test 7: countReachable before and after blocking");

    Edge* blockedEdge = g.findEdge(1, 2);
    check(blockedEdge != nullptr && blockedEdge->baseWeight == 20 && blockedEdge->weight == 20, "Test 8: blocked edge keeps baseWeight");

    return 0;
}