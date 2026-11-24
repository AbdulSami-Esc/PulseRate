#ifndef ROBOT_FLEET_H
#define ROBOT_FLEET_H

#include <stdexcept>
#include "DynamicArray.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Robot.h"
#include "ReverseIndex.h"

class RobotFleet {
private:
    Graph& graph;
    ReverseIndex index;
    DynamicArray<Robot> robots;   // robot id == array position

    void checkId(int id) const {
        if (id < 0 || id >= robots.size()) throw std::out_of_range("Invalid robot id");
    }

    // Re-run Dijkstra from the robot's CURRENT node. Returns false if no route exists.
    bool reroute(int id) {
        Robot& r = robots[id];
        int current = r.currentNode();

        index.removeRoute(r.route, r.position, id);          // forget the old route
        RouteResult res = shortestPath(graph, current, r.destination);

        if (res.found) {
            r.route = res.path;
            r.position = 0;
            r.status = (r.route.size() > 1) ? RobotStatus::MOVING : RobotStatus::IDLE;
            index.addRoute(r.route, 0, id);                  // remember the new route
            return true;
        }

        DynamicArray<int> stay;                              // no way out: wait here
        stay.push_back(current);
        r.route = stay;
        r.position = 0;
        r.status = RobotStatus::STUCK;
        return false;
    }

public:
    explicit RobotFleet(Graph& g) : graph(g) {}

    // New idle robot standing on startNode. Returns its id.
    int addRobot(int startNode) {
        Robot r;
        r.id = robots.size();
        r.route.push_back(startNode);
        r.destination = startNode;
        robots.push_back(r);
        return r.id;
    }

    // Send a robot to 'destination' from where it stands now.
    bool assign(int id, int destination) {
        checkId(id);
        Robot& r = robots[id];

        RouteResult res = shortestPath(graph, r.currentNode(), destination);
        if (!res.found) return false;

        index.removeRoute(r.route, r.position, id);         // drop any old route
        r.route = res.path;
        r.position = 0;
        r.destination = destination;
        r.returning = false;
        r.status = (r.route.size() > 1) ? RobotStatus::MOVING : RobotStatus::IDLE;
        index.addRoute(r.route, 0, id);
        return true;
    }

    // Move one node forward. Returns false if the robot is not moving.
    bool advance(int id) {
        checkId(id);
        Robot& r = robots[id];
        if (r.status != RobotStatus::MOVING) return false;

        // the corridor we are leaving is now behind us
        index.remove(r.route[r.position], r.route[r.position + 1], id);
        r.position++;

        if (r.position == r.route.size() - 1) r.status = RobotStatus::IDLE;   // arrived
        return true;
    }

    // Block a corridor and reroute ONLY the robots that use it.
    // Returns how many robots were affected
    int blockCorridor(int a, int b) {
        if (!graph.setBlocked(a, b, true)) return -1;

        DynamicArray<int> affected = index.robotsOn(a, b);
        for (int i = 0; i < affected.size(); i++) {
            reroute(affected[i]);
        }
        return affected.size();
    }

    
    int unblockCorridor(int a, int b) {
        if (!graph.setBlocked(a, b, false)) return -1;

        int recovered = 0;
        for (int i = 0; i < robots.size(); i++) {
            if (robots[i].status == RobotStatus::STUCK && reroute(i)) recovered++;
        }
        return recovered;
    }

    const Robot& getRobot(int id) const { checkId(id); return robots[id]; }
    int robotCount() const { return robots.size(); }
    const ReverseIndex& reverseIndex() const { return index; }

    bool sendHome(int id, int home) {
        checkId(id);
        Robot& r = robots[id];
        if (r.status != RobotStatus::IDLE || r.currentNode() == home) return false;
        if (!assign(id, home)) return false;
        robots[id].returning = true;
        return true;
    }
};

#endif