#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "DispatchQueue.h"
#include "Dispatcher.h"
#include "RobotFleet.h"

class Simulator {
private:
    DispatchQueue& queue;
    Dispatcher& dispatcher;
    RobotFleet& fleet;
    int currentTick;
    int agingLimit;

public:
    Simulator(DispatchQueue& q, Dispatcher& d, RobotFleet& f)
        : queue(q), dispatcher(d), fleet(f), currentTick(0), agingLimit(10) {}

    // one step of time: age requests, dispatch, move robots
    void tick() {
        queue.ageAll(agingLimit);
        dispatcher.update();
        for (int i = 0; i < fleet.robotCount(); i++) fleet.advance(i);
        currentTick++;
    }

    int now() const { return currentTick; }
};

#endif