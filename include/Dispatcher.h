#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "DispatchQueue.h"
#include "RobotFleet.h"

// Matches waiting requests to idle robots.
class Dispatcher {
private:
    DispatchQueue& queue;
    RobotFleet& fleet;

public:
    Dispatcher(DispatchQueue& q, RobotFleet& f) : queue(q), fleet(f) {}

    // Give the most urgent requests to idle robots.
    // Returns how many requests were handed out this call.
    int update() {
        int handed = 0;
        for (int i = 0; i < fleet.robotCount() && !queue.empty(); i++) {
            if (fleet.getRobot(i).status != RobotStatus::IDLE) continue;

            Request req = queue.next();
            if (fleet.assign(i, req.destination)) {
                handed++;
            }
            // if assign fails (room unreachable) the request is dropped here;
            // you can decide later to re-submit it instead.
        }
        return handed;
    }
};

#endif