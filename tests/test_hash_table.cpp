#include <iostream>
#include <string>
#include "../include/HashTable.h"

void check(bool condition, const std::string& name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
}

int main() {
    // Test 1: put and get ("Pharmacy" -> 0, "ICU_Room_4" -> 4)
    HashTable ht;
    ht.put("Pharmacy", 0);
    ht.put("ICU_Room_4", 4);

    int val1 = -1, val2 = -1;
    bool ok1 = ht.get("Pharmacy", val1);
    bool ok2 = ht.get("ICU_Room_4", val2);
    check(ok1 && val1 == 0 && ok2 && val2 == 4 && ht.size() == 2,
          "Test 1: put and get");

    // Test 2: missing key returns false
    int missing = -1;
    check(!ht.get("Unknown_Room", missing) && !ht.contains("Unknown_Room"),
          "Test 2: missing key returns false");

    // Test 3: update an existing key (size must NOT increase)
    ht.put("Pharmacy", 99);
    int updated = -1;
    ht.get("Pharmacy", updated);
    check(ht.size() == 2 && updated == 99,
          "Test 3: update existing key (size unchanged)");

    // Test 4: remove, then get must fail
    bool removed = ht.remove("Pharmacy");
    int afterRemove = -1;
    check(removed && !ht.get("Pharmacy", afterRemove) && !ht.remove("Pharmacy") && ht.size() == 1,
          "Test 4: remove and get fails");

    // Test 5: collisions: use a tiny table (HashTable t(2)), insert 10 keys,
    //         all must still be found
    HashTable t(2);
    for (int i = 0; i < 10; ++i) {
        t.put("Key_" + std::to_string(i), i * 100);
    }
    bool allTinyFound = (t.size() == 10);
    for (int i = 0; i < 10; ++i) {
        int out = -1;
        if (!t.get("Key_" + std::to_string(i), out) || out != i * 100) {
            allTinyFound = false;
            break;
        }
    }
    check(allTinyFound, "Test 5: tiny table collisions");

    // Test 6: rehash: insert 1000 keys like "Room_0".."Room_999";
    //         check bucketCount() grew, loadFactor() <= 0.75, all found
    HashTable bigTable;
    int startBuckets = bigTable.bucketCount();
    for (int i = 0; i < 1000; ++i) {
        bigTable.put("Room_" + std::to_string(i), i);
    }

    bool allBigFound = true;
    for (int i = 0; i < 1000; ++i) {
        int out = -1;
        if (!bigTable.get("Room_" + std::to_string(i), out) || out != i) {
            allBigFound = false;
            break;
        }
    }
    check(bigTable.size() == 1000 &&
          bigTable.bucketCount() > startBuckets &&
          bigTable.loadFactor() <= 0.75 &&
          allBigFound,
          "Test 6: rehash 1000 keys");

    // Test 7: print longestChain() for 1000 keys (should be small, about 5-8)
    int longest = bigTable.longestChain();
    std::cout << "Longest chain for 1000 keys: " << longest << "\n";
    check(longest > 0 && longest <= 10,
          "Test 7: longestChain is small");

    return 0;
}