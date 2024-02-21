#pragma once 

#include <list>
#include <iostream>

template <typename T>
class safeQueue {
    
    typedef T        value_type;
    typedef T&       reference_type;
    typedef const T& const_reference_type;

    std::recursive_mutex _m_operationLock;

    std::list<T> _queue;

    public:
        void lock(void) {
            _m_operationLock.lock();
        }

        void unlock(void) {
            _m_operationLock.unlock();
        }

        void push(const_reference_type value) {
            lock();
            _queue.push_back(value);
            unlock();
        }

        bool empty(void) const {
            return _queue.empty();
        }

        reference_type pop_front(void) {
            lock();

            reference_type tmp = _queue.front();
            _queue.pop_front();

            unlock();

            return tmp;
        }

        template<class UnaryPredicate>
        void remove_if(UnaryPredicate p) {
            lock();

            _queue.remove_if(p);

            unlock();
        }

        // void print(void) {
        //     for (const T& v : _queue) {
        //         std::cout << v.client << " " << (int)v.type << std::endl;
        //     }
        // }
}; 