#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <stdexcept>
#include "DynamicArray.h"

// What the heap stores: a priority (smaller = served first) and an id.
// Dijkstra: priority = distance, id = node number
// Dispatch: priority = urgency, id = request number
struct HeapItem {
    int priority;
    int id;
};

class MinHeap {
private:
    // COURSE: Complete binary tree implemented using an array
    DynamicArray<HeapItem> items;

    int parent(int i) const { return (i - 1) / 2; }
    int left(int i)   const { return 2 * i + 1; }
    int right(int i)  const { return 2 * i + 2; }

    void swapItems(int a, int b) {
        // TODO: swap items[a] and items[b] using a temp variable
    }

    // After inserting at the END, move it UP while smaller than its parent.
    void siftUp(int i) {
        // TODO: while (i > 0 && items[i].priority < items[parent(i)].priority)
        //           swapItems(i, parent(i)); i = parent(i);
    }

    // After moving the last item to the TOP, move it DOWN.
    void siftDown(int i) {
        // TODO: loop:
        //   smallest = i
        //   if left(i) < size and its priority < items[smallest] -> smallest = left(i)
        //   if right(i) < size and its priority < items[smallest] -> smallest = right(i)
        //   if smallest == i -> stop
        //   else swapItems(i, smallest); i = smallest;
    }

public:
    void push(int priority, int id) {
        // TODO: push_back a HeapItem, then siftUp(size - 1)
    }

    // Look at the smallest without removing.
    HeapItem top() const {
        // TODO: throw std::out_of_range if empty, else items[0]
        return HeapItem{0, 0}; // placeholder
    }

    // Remove and return the smallest.
    HeapItem pop() {
        // TODO 1: throw if empty
        // TODO 2: save items[0] as result
        // TODO 3: items[0] = last item; items.pop_back();
        // TODO 4: if not empty -> siftDown(0)
        // TODO 5: return result
        return HeapItem{0, 0}; // placeholder
    }

    bool empty() const { return items.empty(); }
    int  size()  const { return items.size(); }
    void clear() { items.clear(); }
};

#endif