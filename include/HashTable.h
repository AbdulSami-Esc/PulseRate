#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include "DynamicArray.h"

struct HashEntry {
    std::string key;
    int value;
};

class HashTable {
private:
    DynamicArray< DynamicArray<HashEntry> > buckets; //Making eah bucket a list to ensure that collisions are cattered
    int numBuckets;
    int numItems;

    //Turning test into number to find the location of room
    int hashFunction(const std::string& key) const {
        unsigned long h = 0;
        for (char c : key) {
            h = h * 31 + static_cast<unsigned char>(c);
        }
        return static_cast<int>(h % numBuckets);
    }


    void rehash() {
        DynamicArray< DynamicArray<HashEntry> > oldBuckets = buckets;
        int newCount = (numBuckets > 0) ? numBuckets * 2 : 16;
        initBuckets(newCount);
        numItems = 0;

        for (int i = 0; i < oldBuckets.size(); ++i) {
            for (int j = 0; j < oldBuckets[i].size(); ++j) {
                put(oldBuckets[i][j].key, oldBuckets[i][j].value);
            }
        }
    }

    void initBuckets(int count) {
        numBuckets = count;
        buckets.clear();
        for (int i = 0; i < count; ++i) {
            buckets.push_back(DynamicArray<HashEntry>());
        }
    }

public:
    HashTable(int initialBuckets = 16) : numBuckets(0), numItems(0) {
        initBuckets(initialBuckets);
    }

    //Calculates buckte numbe to put the node in
    void put(const std::string& key, int value) {
        if (numBuckets == 0 || (numItems + 1) * 4 > numBuckets * 3) {
            rehash();
        }

        int idx = hashFunction(key);
        for (int i = 0; i < buckets[idx].size(); ++i) {
            if (buckets[idx][i].key == key) {
                buckets[idx][i].value = value;
                return;
            }
        }

        HashEntry entry;
        entry.key = key;
        entry.value = value;
        buckets[idx].push_back(entry);
        ++numItems;
    }

    //It calculate the bucket number and then goes to that bucket,finds that node and outputs the value
    bool get(const std::string& key, int& out) const {
        int idx = hashFunction(key);
        for (int i = 0; i < buckets[idx].size(); ++i) {
            if (buckets[idx][i].key == key) {
                out = buckets[idx][i].value;
                return true;
            }
        }
        return false;
    }

    //Used to check whether  hash entry with that specific key exist and return true or false
    bool contains(const std::string& key) const {
        int dummy;
        return get(key, dummy);
    }

    //Used to delete hashentry with that specific key
    bool remove(const std::string& key) {
        int idx = hashFunction(key);
        for (int i = 0; i < buckets[idx].size(); ++i) {
            if (buckets[idx][i].key == key) {
                buckets[idx].removeAt(i);
                --numItems;
                return true;
            }
        }
        return false;
    }

    int size() const { return numItems; }
    int bucketCount() const { return numBuckets; }

    double loadFactor() const {
        if (numBuckets == 0) return 0.0;
        return static_cast<double>(numItems) / numBuckets;
    }

    //Used to loop through buckets and find the bucket with most collision
    int longestChain() const {
        int maxLen = 0;
        for (int i = 0; i < buckets.size(); ++i) {
            if (buckets[i].size() > maxLen) {
                maxLen = buckets[i].size();
            }
        }
        return maxLen;
    }
};

#endif