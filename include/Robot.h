#ifndef ROBOT_H
#define ROBOT_H

#include "DynamicArray.h"

enum class RobotStatus { IDLE, MOVING, STUCK };

class Robot {
public:
    int id;
    DynamicArray<int> route;   // e.g. [0, 1, 2, 4]
    int position;              // current index position of robot
    int destination;           // final node
    RobotStatus status;
    bool returning;

    Robot() : id(-1), position(0), destination(-1), status(RobotStatus::IDLE),returning(false) {}

    // the node the robot is standing on
    int currentNode() const { return route[position]; }
};

#endif