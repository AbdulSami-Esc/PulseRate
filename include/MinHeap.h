#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <stdexcept>
#include "DynamicArray.h"

// What the heap stores: a priority (smaller = served first) and an id.
// Dijkstra: priority = distance, id = node number
// Dispatch: priority = urgency, id = request number
class HeapItem {
public:
    int priority;
    int id;

    HeapItem() : priority(0), id(0) {}         
    HeapItem(int p, int id) : priority(p), id(id) {}
};

class MinHeap {
private:
    // COURSE: Complete binary tree implemented using an array
    DynamicArray<HeapItem> items;

    //Used to calculate parent index
    int parent(int i) const { 
        return (i - 1) / 2; 
    }
    //Used calculate the index of left child
    int left(int i)   const { 
        return 2 * i + 1; 
    }
    //Used calculate the index of right child
    int right(int i)  const { 
        return 2 * i + 2; 
    }

    void swapItems(int a, int b) {
        // TODO: swap items[a] and items[b] using a temp variable
        HeapItem temp=items[a];
        items[a]=items[b];
        items[b]=temp;
    }

    // After inserting at the END, move it UP while smaller than its parent.
    void siftUp(int i) {
        while(i>0 && items[i].priority<items[parent(i)].priority){
            swapItems(i,parent(i));
            i=parent(i);
        }

    }

    void siftDown(int i) {
        while (true) {
            int smallest = i;
            int l = left(i);
            int r = right(i);

            if (l < items.size() && items[l].priority < items[smallest].priority) {
                smallest = l;
            }
            if (r < items.size() && items[r].priority < items[smallest].priority) {
                smallest = r;
            }
            if (smallest == i) {
                break;
            }

            swapItems(i, smallest);
            i = smallest;
        }
    }

public:
    void push(int priority, int id) {
        HeapItem item(priority,id);

        items.push_back(item);
        siftUp(items.size() - 1);
    }

    // Look at the smallest without removing.
    HeapItem top() const {
        // TODO: throw std::out_of_range if empty, else items[0]
        if(items.size()==0){
            throw std::out_of_range("The top doesnt exist");
        }
        if(items.size()!=0){
            return items[0];
        }
        return HeapItem{0, 0}; // placeholder
    }

    HeapItem pop() {
        if (items.empty()) {
            throw std::out_of_range("Cannot pop from an empty MinHeap");
        }

        HeapItem result = items[0];
        items[0] = items[items.size() - 1];
        items.pop_back();

        if (!items.empty()) {
            siftDown(0);
        }

        return result;
    }

    bool empty() const { return items.empty(); }
    int  size()  const { return items.size(); }
    void clear() { items.clear(); }
};

#endif