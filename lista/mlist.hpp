#ifndef LIST_HPP_INCLUDED
#define LIST_HPP_INCLUDED

#include <iostream>


template <typename T> struct node_st
{
    T data;
    struct node_st *next;

    T& operator=(const T&);
};

template<typename T>
T& node_st<T>::operator=(const T& t){
    this.data = t;
}

template <typename T> class List{
private:
    node_st<T> *head;
    int length;
    node_st<T> *create_node(T data);
    void delete_list();
public:
    List();
    ~List();
    //returns the length of the list
    int len();
    // adds element to specified index
    // @param data element to add
    // @param idx index at witch to add
    bool add(T data, int idx);
    //returns the value at the head of the list
    T front();
    //returns the value at the back of the list
    T back();
    //removes the first element and returns it's value
    T pop_front();
    //removes the last element and returns it's value
    T pop_back();
    //adds element to the front of the list
    // @param data elemet to add
    bool push_front(T data);
    //adds element to the back of the list
    // @param data elemet to add
    bool push_back(T data);
    T& operator[] (int i);
    T operator[] (int i) const;
};

#include "mlist.tpp"
#endif