#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdexcept>   // std::out_of_range
#include <cstddef>     // size_t

template <typename T>
class DynamicArray {
private:
    T*     data;       // pointer to heap memory
    int length;     // how many elements are used
    int capacity;   // how many slots are allocated

    // Allocate bigger memory, copy old items, free old memory.
    void grow() {
        int newcapacity;
        if (capacity==0){
            newcapacity=2;
        }else{
            newcapacity=capacity*2;
        }
        T* newData=new T[newcapacity];
        for(int i=0;i<length;i++){
            newData[i]=data[i];
        }
        delete[] data;
        data=newData;
        capacity=newcapacity;
    }

public:
    DynamicArray() : data(nullptr), length(0), capacity(0) {
    }

    // Copy constructor: DEEP copy
    DynamicArray(const DynamicArray& other):length(other.length),capacity(other.capacity) {
        data=(capacity>0)? new T[capacity]:nullptr;

        for(int i=0;i<length;i++){
            data[i]=other.data[i];
        }

    }

    // Copy assignment: a = b
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) {
            return *this;
        }

        delete[] data; // 1. Free old memory FIRST

        length = other.length;
        capacity = other.capacity;
        data = (capacity > 0) ? new T[capacity] : nullptr; // 2. Allocate new memory

        for (int i = 0; i < length; ++i) { // 3. Copy elements
            data[i] = other.data[i];
        }

        return *this;
    }

    // Destructor
    ~DynamicArray() {
        delete[] data;
    }

    //Core operations 
    void push_back(const T& value) {
        if(length==capacity){
            this->grow();
        }
        data[length++]=value;
    }

    

    void pop_back() {
        if (length == 0) {
            throw std::out_of_range("Cannot pop_back from an empty DynamicArray");
        }
        --length;
    }

    T& operator[](int index) {
        if (index<0 || index>=length) {
            throw std::out_of_range("index out of range");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= length){
            throw std::out_of_range("Index out of range");
        }
        return data[index];   
    }

    int size() const { return length; }
    bool   empty() const { return length == 0; }
    void   clear() { length = 0; }   

    // Extras for later phases
    void insertAt(int index, const T& value) {
        if (index < 0 || index > length) {
            throw std::out_of_range("Index out of range");
        }
        if (length == capacity) {
            grow();
        }
        for (int i = length; i > index; --i) {
            data[i] = data[i - 1];
        }
        data[index] = value;
        ++length;
    }

    void removeAt(int index) {
        if (index < 0 || index >= length) {
            throw std::out_of_range("Index out of range");
        }
        for (int i = index; i < length - 1; ++i) {
            data[i] = data[i + 1];
        }
        --length;
    }

    void reverse() {
        int left = 0;
        int right = length - 1;
        while (left < right) {
            T temp = data[left];
            data[left] = data[right];
            data[right] = temp;
            ++left;
            --right;
        }
    }
};

#endif