#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>

// ============================================================
// CustomQueue<T> — A generic queue implemented using a
// singly linked list. Supports all operations used by
// MatchKernel's matchmaking engine.
//
// Operations:
//   push(T)    → O(1)  enqueue at back
//   pop()      → O(1)  dequeue from front
//   front()    → O(1)  peek at front element
//   empty()    → O(1)  check if queue has no elements
//   size()     → O(1)  return number of elements
//
// Memory: Each node is heap-allocated. Copy constructor and
// copy assignment operator perform deep copies so queues can
// be passed by value safely (used in displayQueues and
// queueToJson).
// ============================================================

template <typename T>
class CustomQueue {

private:

    // Internal linked list node
    struct Node {
        T data;
        Node* next;

        Node(const T& value) : data(value), next(nullptr) {}
    };

    Node* headNode;   // front of queue — dequeue happens here
    Node* tailNode;   // back of queue  — enqueue happens here
    int   count;      // tracks size in O(1)

public:

    // --------------------------------------------------------
    // Default constructor — empty queue
    // --------------------------------------------------------
    CustomQueue() : headNode(nullptr), tailNode(nullptr), count(0) {}

    // --------------------------------------------------------
    // Copy constructor — deep copy of entire linked list
    // Called when queue is passed by value (e.g. in
    // displayQueues and queueToJson to safely peek without
    // modifying the original queue)
    // --------------------------------------------------------
    CustomQueue(const CustomQueue<T>& other)
        : headNode(nullptr), tailNode(nullptr), count(0) {

        Node* current = other.headNode;
        while (current != nullptr) {
            push(current->data);
            current = current->next;
        }
    }

    // --------------------------------------------------------
    // Copy assignment operator — clears existing data then
    // deep copies from other
    // --------------------------------------------------------
    CustomQueue<T>& operator=(const CustomQueue<T>& other) {
        if (this == &other) return *this;

        // Clear current contents
        while (!empty()) pop();

        // Deep copy from other
        Node* current = other.headNode;
        while (current != nullptr) {
            push(current->data);
            current = current->next;
        }

        return *this;
    }

    // --------------------------------------------------------
    // Destructor — frees all heap-allocated nodes
    // --------------------------------------------------------
    ~CustomQueue() {
        while (!empty()) pop();
    }

    // --------------------------------------------------------
    // push(value) — enqueue at back
    // New node becomes the new tail
    // --------------------------------------------------------
    void push(const T& value) {
        Node* newNode = new Node(value);

        if (tailNode == nullptr) {
            // Queue was empty — new node is both head and tail
            headNode = newNode;
            tailNode = newNode;
        } else {
            tailNode->next = newNode;
            tailNode       = newNode;
        }

        count++;
    }

    // --------------------------------------------------------
    // pop() — dequeue from front
    // Removes head node and advances head pointer
    // Throws if queue is empty
    // --------------------------------------------------------
    void pop() {
        if (empty()) {
            throw std::underflow_error("pop() called on empty queue");
        }

        Node* toDelete = headNode;
        headNode       = headNode->next;

        if (headNode == nullptr) {
            // Queue is now empty — tail must also be null
            tailNode = nullptr;
        }

        delete toDelete;
        count--;
    }

    // --------------------------------------------------------
    // front() — returns reference to front element
    // Does not remove it
    // Throws if queue is empty
    // --------------------------------------------------------
    T& front() {
        if (empty()) {
            throw std::underflow_error("front() called on empty queue");
        }
        return headNode->data;
    }

    const T& front() const {
        if (empty()) {
            throw std::underflow_error("front() called on empty queue");
        }
        return headNode->data;
    }

    // --------------------------------------------------------
    // empty() — returns true if no elements present
    // --------------------------------------------------------
    bool empty() const {
        return count == 0;
    }

    // --------------------------------------------------------
    // size() — returns number of elements
    // --------------------------------------------------------
    int size() const {
        return count;
    }
};

#endif