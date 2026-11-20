#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include "../include/MapLoader.h"

void check(bool condition, const std::string& name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
}

// runs tests for loading nodes and edges CSV files into Graph and HashTable
int main() {
    Graph g;
    HashTable names;
    std::string err;

    bool loaded = loadMap("../data/nodes.csv", "../data/edges.csv", g, names, err);
    check(loaded, "Test 1: loadMap succeeds on valid files");

    check(g.nodeCount() == 8 && g.edgeCount() == 16, "Test 2: nodeCount is 8 and edgeCount is 16");

    int id = -1;
    bool foundIcu = names.get("ICU_Room_4", id);
    check(foundIcu && id == 4, "Test 3: ICU_Room_4 maps to id 4");

    id = -1;
    bool foundPharm = names.get("Pharmacy", id);
    check(foundPharm && id == 0, "Test 4: Pharmacy maps to id 0");

    check(g.getNode(4).type == "icu", "Test 5: node 4 type is icu");

    check(g.countReachable(0) == 8, "Test 6: map is fully connected");

    Graph g2;
    HashTable n2;
    std::string err2;
    bool missingRes = loadMap("../data/no_such_file.csv", "../data/edges.csv", g2, n2, err2);
    check(!missingRes && !err2.empty(), "Test 7: missing file returns false with error message");

    std::ofstream out("temp_bad_edges.csv");
    out << "from,to,seconds,status\n";
    out << "0,1,abc,open\n";
    out.close();

    Graph g3;
    HashTable n3;
    std::string err3;
    bool badLineRes = loadMap("../data/nodes.csv", "temp_bad_edges.csv", g3, n3, err3);
    std::remove("temp_bad_edges.csv");
    check(!badLineRes && !err3.empty(), "Test 8: invalid weight in CSV returns false");

    return 0;
}