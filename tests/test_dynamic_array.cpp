#include <iostream>
#include <string>
#include <stdexcept>
#include "../include/DynamicArray.h"

// Tiny test helper
void check(bool condition, const std::string& name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
}

int main() {
    // Test 1: push_back and size
    DynamicArray<int> arr;
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);
    check(arr.size() == 3 && arr[0] == 10 && arr[1] == 20 && arr[2] == 30,
          "Test 1: push_back and size");

    // Test 2: growth
    DynamicArray<int> bigArr;
    for (int i = 0; i < 1000; ++i) {
        bigArr.push_back(i);
    }
    check(bigArr.size() == 1000 && bigArr[0] == 0 && bigArr[999] == 999,
          "Test 2: growth");

    // Test 3: bounds check
    bool caughtUpper = false;
    bool caughtLower = false;
    try {
        int val = bigArr[5000];
        (void)val;
    } catch (const std::out_of_range&) {
        caughtUpper = true;
    }
    try {
        int val = bigArr[-1];
        (void)val;
    } catch (const std::out_of_range&) {
        caughtLower = true;
    }
    check(caughtUpper && caughtLower, "Test 3: bounds check");

    // Test 4: deep copy
    DynamicArray<int> a;
    a.push_back(100);
    a.push_back(200);
    DynamicArray<int> b = a;
    b[0] = 999;
    check(a[0] == 100 && a[1] == 200 && b[0] == 999,
          "Test 4: deep copy");

    // Test 5: assignment and self-assignment
    DynamicArray<int> c;
    c.push_back(5);
    c = a;
    c[0] = 777;
    a = a; // self-assignment check
    check(a.size() == 2 && a[0] == 100 && a[1] == 200 && c[0] == 777,
          "Test 5: assignment and self-assignment");

    // Test 6: works with strings and string assignment
    DynamicArray<std::string> names;
    names.push_back("ICU_Room_4");
    names.push_back("ER_Bay_1");
    check(names.size() == 2 && names[0] == "ICU_Room_4" && names[1] == "ER_Bay_1",
          "Test 6a: works with strings");

    DynamicArray<std::string> strA, strB;
    strA.push_back("ICU_Room_4");
    strB = strA;
    check(strB.size() == 1 && strB[0] == "ICU_Room_4",
          "Test 6b: string assignment");

    // Test 7: removeAt / insertAt / reverse
    DynamicArray<int> seq;
    seq.push_back(1);
    seq.push_back(3);

    seq.insertAt(1, 2); // [1, 2, 3]
    bool insertOk = (seq.size() == 3 && seq[0] == 1 && seq[1] == 2 && seq[2] == 3);

    seq.removeAt(1);    // [1, 3]
    bool removeOk = (seq.size() == 2 && seq[0] == 1 && seq[1] == 3);

    seq.push_back(5);   // [1, 3, 5]
    seq.reverse();      // [5, 3, 1]
    bool reverseOk = (seq.size() == 3 && seq[0] == 5 && seq[1] == 3 && seq[2] == 1);

    check(insertOk && removeOk && reverseOk,
          "Test 7: removeAt / insertAt / reverse");

    return 0;
}