#include "iostream"
#ifndef REQUEST_H
#define REQUEST_H
using namespace std;

class Request {
public:
    int id;                // position in the request list
    string item;      
    int destination;       // node id
    int priority;          // 1 = STAT, 2 = urgent, 3 = routine
    int arrival;        
    int waited;            // ticks spent waiting
    bool served;          

    Request() : id(-1), destination(-1), priority(3), arrival(0), waited(0), served(false) {}
};

#endif