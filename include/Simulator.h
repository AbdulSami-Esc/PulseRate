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
    WeightModel& weights;
    int home;
    int currentTick;
    int agingLimit;

public:
    Simulator(DispatchQueue& q, Dispatcher& d, RobotFleet& f, WeightModel& w, int homeNode)
        : queue(q), dispatcher(d), fleet(f), weights(w), home(homeNode), currentTick(0), agingLimit(10) {}

    // one step of time: weights, aging, robots go home, dispatch, move
    void tick() {
        weights.tick();
        queue.ageAll(agingLimit);
        for (int i = 0; i < fleet.robotCount(); i++) fleet.sendHome(i, home);
        dispatcher.update();
        for (int i = 0; i < fleet.robotCount(); i++) fleet.advance(i);
        currentTick++;
    }

    int now() const { return currentTick; }
};

#endif