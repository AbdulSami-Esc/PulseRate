#ifndef CLI_H
#define CLI_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include "DynamicArray.h"
#include "HashTable.h"
#include "Graph.h"
#include "Trie.h"
#include "DispatchQueue.h"
#include "RobotFleet.h"
#include "Simulator.h"

class CLI {
private:
    const Graph& graph;
    const HashTable& names;
    const Trie& trie;
    DispatchQueue& queue;
    RobotFleet& fleet;
    Simulator& sim;

    DynamicArray<std::string> split(const std::string& line) const {
        DynamicArray<std::string> words;
        std::string cur;
        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            if (c == ' ' || c == '\t' || c == '\r') {
                if (!cur.empty()) { words.push_back(cur); cur = ""; }
            } else {
                cur += c;
            }
        }
        if (!cur.empty()) words.push_back(cur);
        return words;
    }

    std::string upper(std::string s) const {
        for (size_t i = 0; i < s.size(); i++) s[i] = (char)toupper((unsigned char)s[i]);
        return s;
    }

    // name -> node id; on failure prints "Did you mean" using the trie
    bool lookup(const std::string& name, int& id) const {
        if (names.get(name, id)) return true;
        std::cout << "Unknown room '" << name << "'.";
        for (int len = (int)name.size(); len >= 1; len--) {
            DynamicArray<std::string> hints = trie.suggest(name.substr(0, len));
            if (hints.size() > 0) {
                std::cout << " Did you mean:";
                for (int i = 0; i < hints.size() && i < 5; i++) std::cout << " " << hints[i];
                break;
            }
        }
        std::cout << "\n";
        return false;
    }

    std::string statusName(RobotStatus s) const {
        if (s == RobotStatus::MOVING) return "MOVING";
        if (s == RobotStatus::STUCK) return "STUCK";
        return "IDLE";
    }

    // "R0 [MOVING] Junction_A -> Lab -> ICU_Room_4"
    void printRobot(int id) const {
        const Robot& r = fleet.getRobot(id);
        std::cout << "R" << id << " [" << statusName(r.status) << "] ";
        if (r.status == RobotStatus::STUCK) {
            std::cout << "waiting at " << graph.getNode(r.currentNode()).name << "\n";
            return;
        }
        for (int i = r.position; i < r.route.size(); i++) {
            if (i > r.position) std::cout << " -> ";
            std::cout << graph.getNode(r.route[i]).name;
        }
        std::cout << "\n";
    }

    // dispatch <room> <STAT|URGENT|ROUTINE> <item>
    void cmdDispatch(const DynamicArray<std::string>& w) {
        if (w.size() < 3) { std::cout << "Usage: dispatch <room> <STAT|URGENT|ROUTINE> <item>\n"; return; }
        int dest;
        if (!lookup(w[1], dest)) return;

        std::string level = upper(w[2]);
        int priority;
        if (level == "STAT") priority = 1;
        else if (level == "URGENT") priority = 2;
        else if (level == "ROUTINE") priority = 3;
        else { std::cout << "Priority must be STAT, URGENT or ROUTINE\n"; return; }

        std::string item = (w.size() > 3) ? w[3] : "Item";
        int id = queue.submit(item, dest, priority);
        std::cout << "Request " << id << " queued (" << level << "): " << item << " -> " << w[1] << "\n";
    }

    // block <roomA> <roomB>: reroutes only the robots that use this corridor
    void cmdBlock(const DynamicArray<std::string>& w) {
        if (w.size() < 3) { std::cout << "Usage: block <roomA> <roomB>\n"; return; }
        int a, b;
        if (!lookup(w[1], a) || !lookup(w[2], b)) return;

        DynamicArray<int> affected = fleet.reverseIndex().robotsOn(a, b);
        if (fleet.blockCorridor(a, b) == -1) {
            std::cout << "No corridor between " << w[1] << " and " << w[2] << "\n";
            return;
        }
        std::cout << "ALERT: " << w[1] << " - " << w[2] << " blocked. Affected robots: " << affected.size() << "\n";
        for (int i = 0; i < affected.size(); i++) {
            std::cout << "  re-routed: ";
            printRobot(affected[i]);
        }
    }

    void cmdUnblock(const DynamicArray<std::string>& w) {
        if (w.size() < 3) { std::cout << "Usage: unblock <roomA> <roomB>\n"; return; }
        int a, b;
        if (!lookup(w[1], a) || !lookup(w[2], b)) return;

        int recovered = fleet.unblockCorridor(a, b);
        if (recovered == -1) {
            std::cout << "No corridor between " << w[1] << " and " << w[2] << "\n";
            return;
        }
        std::cout << w[1] << " - " << w[2] << " reopened. Stuck robots recovered: " << recovered << "\n";
    }

    // tick [n]: runs n steps and shows where every robot is
    void cmdTick(const DynamicArray<std::string>& w) {
        int n = (w.size() > 1) ? atoi(w[1].c_str()) : 1;
        if (n < 1) { std::cout << "Usage: tick [number]\n"; return; }
        for (int t = 0; t < n; t++) {
            sim.tick();
            std::cout << "[tick " << sim.now() << "]";
            for (int i = 0; i < fleet.robotCount(); i++) {
                const Robot& r = fleet.getRobot(i);
                std::cout << "  R" << i << "@" << graph.getNode(r.currentNode()).name;
            }
            std::cout << "\n";
        }
    }

    void cmdStatus() const {
        int idle = 0, moving = 0, stuck = 0;
        for (int i = 0; i < fleet.robotCount(); i++) {
            RobotStatus s = fleet.getRobot(i).status;
            if (s == RobotStatus::IDLE) idle++;
            else if (s == RobotStatus::MOVING) moving++;
            else stuck++;
        }
        std::cout << "Tick " << sim.now() << " | waiting requests: " << queue.size()
                  << " | robots idle " << idle << ", moving " << moving << ", stuck " << stuck << "\n";
    }

    void cmdRobots() const {
        for (int i = 0; i < fleet.robotCount(); i++) printRobot(i);
    }

    void cmdFind(const DynamicArray<std::string>& w) const {
        if (w.size() < 2) { std::cout << "Usage: find <prefix>\n"; return; }
        DynamicArray<std::string> hits = trie.suggest(w[1]);
        if (hits.size() == 0) { std::cout << "No rooms start with '" << w[1] << "'\n"; return; }
        std::cout << "Matches:";
        for (int i = 0; i < hits.size(); i++) std::cout << " " << hits[i];
        std::cout << "\n";
    }

    void cmdHelp() const {
        std::cout << "dispatch <room> <STAT|URGENT|ROUTINE> <item>\n"
                  << "block <roomA> <roomB>\n"
                  << "unblock <roomA> <roomB>\n"
                  << "tick [n]\n"
                  << "status\n"
                  << "robots\n"
                  << "find <prefix>\n"
                  << "help\n"
                  << "quit\n";
    }

public:
    CLI(const Graph& g, const HashTable& n, const Trie& t,
        DispatchQueue& q, RobotFleet& f, Simulator& s)
        : graph(g), names(n), trie(t), queue(q), fleet(f), sim(s) {}

    // main loop: read a line, run the matching command
    void run() {
        std::cout << "PulseRoute ready. Type 'help' for commands.\n";
        std::string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;

            DynamicArray<std::string> w = split(line);
            if (w.empty()) continue;

            std::string cmd = w[0];
            if (cmd == "quit" || cmd == "exit") break;
            else if (cmd == "dispatch") cmdDispatch(w);
            else if (cmd == "block") cmdBlock(w);
            else if (cmd == "unblock") cmdUnblock(w);
            else if (cmd == "tick") cmdTick(w);
            else if (cmd == "status") cmdStatus();
            else if (cmd == "robots") cmdRobots();
            else if (cmd == "find") cmdFind(w);
            else if (cmd == "help") cmdHelp();
            else std::cout << "Unknown command. Type 'help'.\n";
        }
    }
};

#endif