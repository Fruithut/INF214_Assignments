#include <iostream>
#include <assert.h>
#include "alang.hpp"

using namespace std;

template <typename E> class queue;    // forward declaration
template <typename E> class node_iterator;  // forward declaration

template <typename E>
class node {
    E data;
    node<E>* next;
    node(E d, node<E>* n) : data(d), next(n) {}
public:
    friend class queue<E>;         // allow queue<E> to access private members
    friend class node_iterator<E>; // allow node_iterator<E> to access private members
};

template <typename E>
class node_iterator {
    node<E>* it;
public:
    node_iterator(node<E>* n) : it(n) {}
    node_iterator& operator++() { assert(it != nullptr); it = it->next; return *this; }
    bool done() const { return it == nullptr; }
    E operator*() const { assert(it != nullptr); return it->data; }
};

template <typename E>
class queue {
    node<E>* head;
    node<E>* rear;
    int _size;
    semaphore generalLock = 1;
public:
    queue() : head(nullptr), rear(nullptr), _size(0) {}

    bool empty() const { return _size == 0; }
    // alternatively we could test for head or rear being equal to nullptr

    void enqueue(E d) {
        node<E>* newNode = new node<E>(d, nullptr);

        generalLock.P();
        if (rear == nullptr) head = newNode; // enqueue first element
        else rear->next = newNode;   
        rear = newNode;
        _size = _size + 1;
        generalLock.V();
    }

    E dequeue() {
        generalLock.P();
        //evaluate queue-size without interference
        if (empty()) {
            generalLock.V();
            throw "empty queue";
        }

        node<E>* oldHead = head;
        head = head->next;
        if (head == nullptr) rear = nullptr; // removed the last element
        _size = _size - 1;
        generalLock.V();

        E e = oldHead->data;
        delete oldHead;

        return e;
    }

    int size() const {
        return _size;
    }

    node_iterator<E> iterator() {
        return head;
    }
};

/**
 * Tests if the dequeue method of the queue class can function concurrently
 * @param N size of queue to run dequeue operations on
 */
void dequeueTest(const int N) {
    queue<int> d;
    for (int i=0; i<N; ++i) {
        d.enqueue(i);
    }

    int successful_dequeues = 0;
    {
        processes ps;
        ps += [&]{
            int count = 0;
            for (int i = 0; i < N / 2; ++i) {
                try {d.dequeue(); ++count;} catch (...) {}
            }
            successful_dequeues += count;
        };
        ps += [&]{
            int count = 0;
            for (int i = 0; i < N / 2; ++i) {
                try {d.dequeue(); ++count;} catch (...) {}
            }
            successful_dequeues += count;
        };
    }

    cout << "\nDEQUEUE TEST" << endl;
    cout << "Enqueued items: " << N << endl;
    cout << "Total dequeues: " << successful_dequeues << endl;
    cout << "Queue size: " << d.size() << endl;

    auto iterator = d.iterator();
    int it_count = 0;
    while (!iterator.done()) { ++it_count; ++iterator; }
    cout << "Iterator queue count: " << it_count << endl;

    if (it_count > 0 || !d.empty() || successful_dequeues < N) throw "Queue has not been emptied properly";
}

/**
 * Tests if the enqueue method of the queue class can function concurrently
 * @param N size of insertions
 */
void enqueueTest(const int N) {
    queue<int> e;

    {
        processes ps;
        ps += [&]{
            for (int i = 0; i < N / 2; ++i) {
                e.enqueue(1);
            }
        };
        ps += [&]{
            for (int i = 0; i < N / 2; ++i) {
                e.enqueue(2);
            }
        };
    }

    cout << "\nENQUEUE TEST" << endl;
    cout << "Enqueued items: " << N << endl;
    cout << "Queue size: " << e.size() << endl;

    auto iterator = e.iterator();
    int it_count = 0;
    while (!iterator.done()) { ++it_count; ++iterator; }
    cout << "Iterator queue count: " << it_count << endl;

    if (it_count != N || e.size() < N) throw "Queue insertions have been missed";
}

void enqueueAndDequeueTest(const int N) {
    queue<int> de;

    int successful_dequeues = 0;
    {
        processes ps;
        ps += [&]{
            for (int i=0; i<N; ++i) {
                de.enqueue(i);
            }
        };
        ps += [&]{
            int count = 0;
            while (true) {
                if (count == N) break;
                try {de.dequeue(); ++count;} catch (...) {}
            }
            successful_dequeues = count;
        };
    }

    cout << "\nENQUEUE AND DEQUEUE TEST" << endl;
    cout << "Elements enqueued: " << N << endl;
    cout << "Successful dequeue count: " << successful_dequeues << endl;
    cout << "Queue size: " << de.size() << endl;

    auto it = de.iterator();
    int iterator_count = 0;
    while (!it.done()) {cout << *it << " "; ++iterator_count; ++it;}
    cout << "Iterator queue count: " << iterator_count << endl;

    if (iterator_count > 0 || !de.empty() || successful_dequeues < N) throw "Queue operations conflicting";
}

int main() {
    const int N = 10000;
    dequeueTest(N);
    enqueueTest(N);
    enqueueAndDequeueTest(N);

    //FROM ASSIGNMENT DOC TEST USECASE:
    //You will not be able to dequeue all items if this process wastes "iterations of N" on trying
    //to dequeue an empty queue
    /*
    for (int i=0; i<N; ++i) {
        try { d.dequeue(); ++c; } catch (...) {}
    }
    */
}