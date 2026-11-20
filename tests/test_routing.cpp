#include <iostream>
#include <string>
#include <cstdlib>
#include "../include/Router.h"

void check(bool condition, const std::string& name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
}

// Builds the 8-node sample hospital by hand (no files needed)
void buildSample(Graph& g, HashTable& names) {
    const char* rooms[8]  = {"Pharmacy", "Junction_A", "Junction_B", "Lab",
                             "ICU_Room_4", "Elevator_1F", "Elevator_2F", "Ward_201"};
    const char* types[8]  = {"pharmacy", "junction", "junction", "lab",
                             "icu", "elevator", "elevator", "ward"};
    int floors[8] = {1, 1, 1, 1, 1, 1, 2, 2};
    for (int i = 0; i < 8; i++) {
        int id = g.addNode(rooms[i], types[i], floors[i]);
        names.put(rooms[i], id);
    }
    g.addCorridor(0, 1, 10);
    g.addCorridor(1, 2, 15);
    g.addCorridor(1, 3, 20);
    g.addCorridor(2, 4, 12);
    g.addCorridor(3, 4, 25);
    g.addCorridor(1, 5, 8);
    g.addCorridor(5, 6, 30);
    g.addCorridor(6, 7, 10);
}

// Slow but simple reference solution (Bellman-Ford) to cross-check Dijkstra
void referenceDistances(const Graph& g, int src, DynamicArray<int>& dist) {
    int n = g.nodeCount();
    dist.clear();
    for (int i = 0; i < n; i++) dist.push_back(INF);
    dist[src] = 0;
    for (int round = 0; round < n; round++) {
        bool changed = false;
        for (int u = 0; u < n; u++) {
            if (dist[u] == INF) continue;
            const DynamicArray<Edge>& list = g.neighbors(u);
            for (int k = 0; k < list.size(); k++) {
                const Edge& e = list[k];
                if (e.blocked) continue;
                if (dist[u] + e.weight < dist[e.to]) {
                    dist[e.to] = dist[u] + e.weight;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }
}

int main() {
    Graph g;
    HashTable names;
    buildSample(g, names);

    // --- Dijkstra basics ---
    RouteResult r = shortestPath(g, 0, 4);
    check(r.found && r.totalTime == 37, "Test 1: Pharmacy -> ICU takes 37s");
    check(r.path.size() == 4 && r.path[0] == 0 && r.path[1] == 1 &&
          r.path[2] == 2 && r.path[3] == 4, "Test 2: path is 0,1,2,4");

    r = shortestPath(g, 3, 3);
    check(r.found && r.totalTime == 0 && r.path.size() == 1, "Test 3: source == target");

    r = shortestPath(g, 0, 7);
    check(r.found && r.totalTime == 58 && r.path.size() == 5, "Test 4: cross-floor route, 58s");

    // --- Blockages ---
    g.setBlocked(1, 2, true);
    r = shortestPath(g, 0, 4);
    check(r.found && r.totalTime == 55 && r.path[2] == 3, "Test 5: block 1-2 -> goes via Lab, 55s");

    r = shortestPath(g, 1, 4);
    check(r.found && r.totalTime == 45 && r.path.size() == 3 && r.path[0] == 1,
          "Test 6: reroute from the middle (node 1), 45s");

    g.setBlocked(3, 4, true);
    r = shortestPath(g, 0, 4);
    check(!r.found && r.totalTime == -1, "Test 7: ICU cut off -> found = false");

    g.setBlocked(1, 2, false);
    g.setBlocked(3, 4, false);
    r = shortestPath(g, 0, 4);
    check(r.found && r.totalTime == 37, "Test 8: unblock restores 37s");

    // --- Modes and errors ---
    r = shortestPath(g, 0, 4, false);
    check(r.found && r.totalTime == 37, "Test 9: full run gives the same answer");

    bool threw = false;
    try { shortestPath(g, 0, 99); } catch (const std::out_of_range&) { threw = true; }
    check(threw, "Test 10: invalid node id throws");

    // --- Router ---
    Router router(g, names);
    r = router.route("Pharmacy", "ICU_Room_4");
    check(r.found && r.totalTime == 37, "Test 11: Router by names");
    check(router.describe(r) == "Pharmacy -> Junction_A -> Junction_B -> ICU_Room_4 (37s)",
          "Test 12: describe() text");

    r = router.route("Pharmacy", "Nowhere");
    check(!r.found, "Test 13: unknown name is handled safely");
    check(router.describe(r) == "No route found", "Test 14: describe() for no route");

    // --- Cross-check against the reference on a random map ---
    srand(42);
    Graph big;
    int n = 200;
    for (int i = 0; i < n; i++) big.addNode("N" + std::to_string(i), "junction", 1);
    for (int i = 1; i < n; i++) big.addCorridor(i, rand() % i, 1 + rand() % 50);   // connected tree
    for (int i = 0; i < 400; i++) {
        int a = rand() % n, b = rand() % n;
        if (a != b) big.addCorridor(a, b, 1 + rand() % 50, rand() % 10 == 0);     // ~10% blocked
    }

    bool allMatch = true;
    for (int src = 0; src < 3; src++) {
        DynamicArray<int> expected;
        referenceDistances(big, src, expected);
        for (int t = 0; t < n; t++) {
            RouteResult x = shortestPath(big, src, t);
            if (expected[t] == INF) { if (x.found) allMatch = false; }
            else if (!x.found || x.totalTime != expected[t]) allMatch = false;
        }
    }
    check(allMatch, "Test 15: matches reference on a random 200-node map");

    return 0;
}