#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include "../include/MinHeap.h"

void check(bool condition, const std::string& name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
}

int main() {
    // Test 1: push (5,A) (9,B) (7,C) (3,D); top().priority must be 3
    MinHeap heap;
    heap.push(5, 'A');
    heap.push(9, 'B');
    heap.push(7, 'C');
    heap.push(3, 'D');
    check(heap.size() == 4 && heap.top().priority == 3,
          "Test 1: push and top().priority == 3");

    // Test 2: pop everything; priorities must come out sorted: 3,5,7,9
    HeapItem a = heap.pop();
    HeapItem b = heap.pop();
    HeapItem c = heap.pop();
    HeapItem d = heap.pop();
    check(a.priority == 3 && b.priority == 5 &&
          c.priority == 7 && d.priority == 9 && heap.empty(),
          "Test 2: pop everything in sorted order (3, 5, 7, 9)");

    // Test 3: pop on empty heap must throw (try/catch)
    bool threwOnEmpty = false;
    try {
        heap.pop();
    } catch (const std::exception&) {
        threwOnEmpty = true;
    }
    check(threwOnEmpty, "Test 3: pop on empty heap throws");

    // Test 4: duplicates: push the same priority twice, both come out
    MinHeap dupHeap;
    dupHeap.push(5, 101);
    dupHeap.push(2, 100);
    dupHeap.push(5, 102);

    HeapItem first = dupHeap.pop();
    HeapItem second = dupHeap.pop();
    HeapItem third = dupHeap.pop();
    bool idsMatch = (second.id == 101 && third.id == 102) ||
                    (second.id == 102 && third.id == 101);
    check(first.priority == 2 && second.priority == 5 &&
          third.priority == 5 && idsMatch && dupHeap.empty(),
          "Test 4: duplicate priorities both come out");

    // Test 5: stress: push 1000 random numbers (rand()), pop all,
    //         check the output is non-decreasing
    MinHeap stressHeap;
    std::srand(42);
    for (int i = 0; i < 1000; ++i) {
        stressHeap.push(std::rand(), i);
    }

    bool nonDecreasing = (stressHeap.size() == 1000);
    int prev = stressHeap.pop().priority;
    for (int i = 1; i < 1000; ++i) {
        int curr = stressHeap.pop().priority;
        if (curr < prev) {
            nonDecreasing = false;
            break;
        }
        prev = curr;
    }
    check(nonDecreasing && stressHeap.empty(),
          "Test 5: stress test 1000 random elements non-decreasing");

    // Test 6: ids travel with priorities: push (10, id 42), pop, id must be 42
    MinHeap idHeap;
    idHeap.push(25, 10);
    idHeap.push(10, 42);
    idHeap.push(30, 99);
    HeapItem minItem = idHeap.pop();
    check(minItem.priority == 10 && minItem.id == 42,
          "Test 6: id travels with priority");

    return 0;
}