#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "DispatchQueue.h"
#include "Dispatcher.h"
#include "RobotFleet.h"
#include "WeightModel.h"

class Simulator {
private:
    DispatchQueue& queue;
    Dispatcher& dispatcher;
    RobotFleet& fleet;
    int currentTick;
    int agingLimit;
    WeightModel& weights;
public:
    Simulator(DispatchQueue& q, Dispatcher& d, RobotFleet& f,WeightModel& w)
        : queue(q), dispatcher(d), fleet(f),weights(w), currentTick(0), agingLimit(10) {}

    // one step of time: age requests, dispatch, move robots
    void tick() {
        weights.tick();
        queue.ageAll(agingLimit);
        dispatcher.update();
        for (int i = 0; i < fleet.robotCount(); i++) fleet.advance(i);
        currentTick++;
    }

    int now() const { return currentTick; }
};

#endif