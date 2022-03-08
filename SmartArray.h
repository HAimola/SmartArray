#pragma once
#include <iostream>
#include <cmath>
#include <assert.h>

template<class T>
struct SmartArray{
    // Features:
    //      - Dynamic allocation and resizing (1.125 memory ratio)
    //      - Bound checking on run-time
    //      - Range based for loops compatibility
    //      - Negative indexing for inverse iteration
    //      - Reversed view range based for loops with SmartArrayReversed wrapper
    //      - Std::array input and output

    T *m_data{};

        // Creates a new dynamically allocated array, copies all current elements
        // to there and deletes old one
        // resize() does not follow the 1.125 capacity ratio, so it's user inaccessible
        void resize(size_t new_sz){
            if (new_sz == 0){ 
                erase();
                return;
            }
            assert (new_sz>0);

            T *arr = new T [new_sz];
            std::copy(m_data, m_data+new_sz, arr);
            delete[] m_data;

            length = new_sz;
            m_data = arr;
        }

    public:
        size_t length{};

        // ------------------------------
        // |       CONSTRUCTORS         |
        // ------------------------------

        // Default Constructor
        SmartArray(){
            m_data = NULL;
            length = 0;
        }

        // Length Constructor
        SmartArray(size_t length): length{length}{
            m_data = new T[static_cast<size_t>(length)]{};
        };

        // Length and pointer to dynamic array Constructor
        SmartArray(T* m_data, size_t length): length{length}, m_data{m_data}{}

        // Copy constructor, does not share pointers with copy
        SmartArray(SmartArray<T>& other): length{other.length}{
            m_data = new T[static_cast<size_t>(length)];
            std::copy(other.m_data, other.m_data+other.length, m_data);
        };


        inline void checkIndexOrSize(int index){
            if (index< -length && index>=length){
                delete[] m_data;
                printf("ERROR: Access index %d out of bounds! [length = %d]\n", index, length);
                throw;
            }
        }

        size_t size(){
            return length;
        }

        // begin() and end() functions allow range based for loops with SmartArray
        T* begin(){ return this->m_data; }
        T* end(){ return m_data+length; }

        void erase(){
            length = 0;
            m_data = NULL;
        }

        // Put a value at any given index with dynamic reallocs taken care of
        // Size ratio is 9/8 rounds up
        void put(T input, int index=-1){
            checkIndexOrSize(index);

            size_t old_length = length;
            resize(std::ceil(length * 1.125));

            index = index < 0 ? old_length + 1 + index : index;
            m_data[index] = input;
        }
        inline void append(T input){
            put(input);
        }

        void pop(int index= -1){
            checkIndexOrSize(index);

            index = index < 0 ? length + 1 + index : index;
            delete (m_data+index);
            resize(length-1);
        }

        // -----------------------------
        // |     FROM/TO ARRAY OPS     |
        // -----------------------------

        // Initializes the SmartArray from a regular array
        // There are 2 overloads: 1 takes the pointer to the first element
        // and the size, the other is a template which also takes the size 
        // and a reference to the array
        void from_arr(const T *arr, size_t arr_len){
            resize(arr_len);

            for(int i = 0; i < arr_len; i++){
                m_data[i] = arr[i];
            }
        }

        template<size_t N>
        void from_arr(const T (&arr)[N]){
            resize(N);

            for(int i = 0; i < N; i++){
                m_data[i] = arr[i];
            }
        }

        // Creates a new dynamic array, copies elements and returns pointer
        T* to_arr(int new_sz = -1){
            checkIndexOrSize(new_sz);

            if (new_sz == 0) return NULL;
            if (new_sz < 0) new_sz = length + new_sz;

            T *new_array = new T [new_sz]; 

            std::copy(m_data, m_data+new_sz, new_array);
            return new_array;
        }

        // ------------------------------
        // |     OPERATOR OVERLOADS     |
        // ------------------------------

        // Runtime out-of-bounds index checking and value return
        T& operator[](int index){
            checkIndexOrSize(index);
            if(index < 0) index = length + index;
            return m_data[index];
        }

        // Array concatenation. Basically appends one array to the other with the + operator
        // MODIFIES FIRST OPERAND
        // a = {1, 2, 3};
        // b = {10, 20, 30};
        // c = a + b = {1, 2, 3, 10, 20, 30};
        SmartArray& operator+(SmartArray& other){
            size_t old_length = length;
            resize(length+other.length);
            std::copy(other.m_data, other.m_data+old_length, m_data+old_length);

            return *this;
        }

        // Modifying pointers to const is undefined
        SmartArray(const SmartArray &arr) = delete;
        SmartArray& operator=(const SmartArray &arr) = delete;
        virtual ~SmartArray(){
            delete[] m_data;
        }
    
};

template<class T>
struct SmartArrayReverse: public SmartArray<T>{
    public:
        SmartArray<T> iter{};

        SmartArrayReverse(SmartArray<T> arr): iter{arr}{reverse();};
        void reverse(){

            SmartArray<T> buffer(iter.length);

            for(size_t i = 0; i < iter.length; i++){
                buffer[-i-1] = iter[i];
            }
            std::copy(buffer.m_data, buffer.m_data+buffer.length, iter.m_data);

        }
    
        // -----------------------------------
        // |     BOUND POINTER OVERLOADS     |
        // -----------------------------------

        T* begin(){ return iter.m_data; }
        T* end(){ return iter.m_data+iter.length; }
};

// Stream to create a nice repr when cout'ing SmartArray instances

// This only works with bound references:
// SmartArray<> a(3);
// SmartArrayReverse<> rev(a);
// std::cout << rev;
// rev is bound to a
template<class T>
std::ostream& operator<<(std::ostream& out, SmartArrayReverse<T> a){
    out << "{";
    for(auto i: a){
        out << i << ", ";
    }
    out << "}";
    return out;
}

template<class T>
std::ostream& operator<<(std::ostream& out, SmartArray<T> a){
    out << "{";
    for(auto i: a){
        out << i << ", ";
    }
    out << "}";
    return out;
}