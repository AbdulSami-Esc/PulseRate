#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H

#include <string>
#include <utility>
#include "DynamicArray.h"
#include "HashTable.h"

// Corridor -> list of robots whose remaining route uses it.
class ReverseIndex {
private:
    HashTable slots;                        // "1-2" -> slot number
    DynamicArray< DynamicArray<int> > lists; // lists[slot] = robot ids

    // "1-2" and "2-1" must give the same key: smaller node first
    std::string makeKey(int a, int b) const {
        if (a > b) std::swap(a, b);
        return std::to_string(a) + "-" + std::to_string(b);
    }

    // returns the slot of a corridor
    int findSlot(int a, int b) const {
        int slot;
        if (slots.get(makeKey(a, b), slot)) return slot;
        return -1;
    }

public:
    // robot starts using corridor a-b
    void add(int a, int b, int robotId) {
        int slot = findSlot(a, b);
        if (slot == -1) {
            slot = lists.size();
            lists.push_back(DynamicArray<int>());
            slots.put(makeKey(a, b), slot);
        }
        lists[slot].push_back(robotId);
    }

    // robot stops using corridor a-b
    void remove(int a, int b, int robotId) {
        int slot = findSlot(a, b);
        if (slot == -1) return;
        DynamicArray<int>& list = lists[slot];
        for (int i = 0; i < list.size(); i++) {
            if (list[i] == robotId) {
                list.removeAt(i);
                return;
            }
        }
    }

    // who uses corridor a-b? 
    DynamicArray<int> robotsOn(int a, int b) const {
        int slot = findSlot(a, b);
        if (slot == -1) return DynamicArray<int>();
        return lists[slot];
    }

    void addRoute(const DynamicArray<int>& route, int from, int robotId) {
        for (int i = from; i + 1 < route.size(); i++) {
            add(route[i], route[i + 1], robotId);
        }
    }

    void removeRoute(const DynamicArray<int>& route, int from, int robotId) {
        for (int i = from; i + 1 < route.size(); i++) {
            remove(route[i], route[i + 1], robotId);
        }
    }
};

#endif