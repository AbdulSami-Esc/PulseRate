#ifndef DISPATCH_QUEUE_H
#define DISPATCH_QUEUE_H

#include <string>
#include <stdexcept>
#include "DynamicArray.h"
#include "MinHeap.h"
#include "Request.h"

// Waiting requests, most urgent first.
class DispatchQueue {
private:
    MinHeap heap;                    // (key, requestId)
    DynamicArray<Request> requests;  // requestId == array position
    int waiting;                     // requests still in the heap

    // smaller key = served first. Priority decides, arrival breaks ties.
    int makeKey(const Request& r) const {
        return r.priority * 100000 + r.arrival;
    }

    // throw away the heap and rebuild it from the unserved requests
    void rebuild() {
        heap.clear();
        for (int i = 0; i < requests.size(); i++) {
            if (!requests[i].served) heap.push(makeKey(requests[i]), i);
        }
    }

public:
    DispatchQueue() : waiting(0) {}

    // add a delivery request, returns its id
    int submit(const std::string& item, int destination, int priority) {
        if (priority < 1 || priority > 3) throw std::invalid_argument("priority must be 1 to 3");
        Request r;
        r.id = requests.size();
        r.item = item;
        r.destination = destination;
        r.priority = priority;
        r.arrival = r.id;
        requests.push_back(r);
        heap.push(makeKey(r), r.id);
        waiting++;
        return r.id;
    }

    // remove and return the most urgent request
    Request next() {
        if (waiting == 0) throw std::out_of_range("no waiting requests");
        int id = heap.pop().id;
        requests[id].served = true;
        waiting--;
        return requests[id];
    }

    // one tick passes: every request waits longer.
    void ageAll(int limit) {
        bool changed = false;
        for (int i = 0; i < requests.size(); i++) {
            Request& r = requests[i];
            if (r.served) continue;
            r.waited++;
            if (r.waited >= limit && r.priority > 1) {
                r.priority--;
                r.waited = 0;
                changed = true;
            }
        }
        if (changed) rebuild();
    }

    bool empty() const { return waiting == 0; }
    int size() const { return waiting; }
    const Request& get(int id) const { return requests[id]; }
};

#endif