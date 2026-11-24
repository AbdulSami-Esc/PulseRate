#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "DispatchQueue.h"
#include "RobotFleet.h"

// Gives waiting requests to idle robots that are standing at the home node (Pharmacy).
class Dispatcher {
private:
    DispatchQueue& queue;
    RobotFleet& fleet;
    int home;

public:
    Dispatcher(DispatchQueue& q, RobotFleet& f, int homeNode) : queue(q), fleet(f), home(homeNode) {}

    // Returns how many requests were handed out this call.
    int update() {
        int handed = 0;
        for (int i = 0; i < fleet.robotCount() && !queue.empty(); i++) {
            const Robot& r = fleet.getRobot(i);
            if (r.status != RobotStatus::IDLE || r.currentNode() != home) continue;

            Request req = queue.next();
            if (fleet.assign(i, req.destination)) handed++;
        }
        return handed;
    }
};

#endif