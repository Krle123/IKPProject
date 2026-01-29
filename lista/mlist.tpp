#include "mlist.hpp"

template<typename T>
List<T>::List()
{
    head = NULL;
    length = 0;
}
template<typename T>
List<T>::~List(){
    delete_list();
}

template<typename T>
node_st<T> *List<T>::create_node(T data){
    node_st<T> *created = (node_st<T> *)malloc(sizeof(node_st<T>));
    
    if(created == NULL){
        std::cout << "Error allocating memory\n";
        return NULL;
    }
    
    created->data = data;

    created->next = NULL;
    return created;
}

template<typename T>
void List<T>::delete_list(){
    node_st<T>* current;

    while (head != NULL)
    {
        current = head;
        head = head->next;
        current->next = NULL;
        free(current);
    }
}

template<typename T>
int List<T>::len(){
    return length;
}

template<typename T>
bool List<T>::add(T data, int idx){
    node_st<T>* new_node = create_node(data);

    if(new_node == NULL){
        return false;
    }
    if(idx > length-1){
        std::cout << "Index: " << idx << " out of bounds\n"; 
        return false;
    }
    node_st<T>* current = head;
    for(int i=0; i<idx-1; i++){
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
    length++;
    return true;    
}

template<typename T>
T List<T>::front(){
    return head->data;
}

template<typename T>
T List<T>::back(){
    node_st<T> *current = head;
    while(current->next != NULL){
        current = current->next;
    }
    return current->data;
}

template<typename T>
T List<T>::pop_front(){
    node_st<T> *current = head;
    head = head->next;
    T to_send = current->data;
    free(current);
    length--;
    return to_send;
}

template<typename T>
T List<T>::pop_back(){
    node_st<T> *current = head;
    while (current->next->next != NULL)
    {
        current = current->next;
    }

    T to_send = current->next->data;
    current->next = NULL;
    free(current->next);
    length--;
    return to_send;
}

template<typename T>
bool List<T>::push_front(T data){
    node_st<T>* new_node = create_node(data);

    if(new_node == NULL){
        return false;
    }

    if(head == NULL){
        head = new_node;
        length++;
        return true;
    }else{

        node_st<T>* current = head;
        head = new_node;
        head->next = current;
        length++;
        return true;
    }
}
template<typename T>
bool List<T>::push_back(T data){
    node_st<T>* new_node = create_node(data);

    if(new_node == NULL){
        return false;
    }

    if(head == NULL){
        head = new_node;
        length++;
        return true;
    }else{
        node_st<T>* current = head;
        while(current->next != NULL){
            current = current->next;
        }
        
        current->next = new_node;
        length++;
        return true;
    }
}
template<typename T>
T& List<T>::operator[] (int i){
    node_st<T> *current = head;
    for(int j=0; j < i; j++){
        current = current->next;
    }
    
    return current->data;
}

template<typename T>
T List<T>::operator[] (int i) const{
    node_st<T> *current = head;
    for(int j=0; j < i; j++){
        current = current->next;
    }
    return current->data;
}