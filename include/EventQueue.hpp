#pragma once 

#include <list>
#include <mutex>
#include "Event.hpp"

class EventQueue {
    mutable std::recursive_mutex _m_operationLock;

    std::list<Event> _queue;

public:
    void lock(void) {
        _m_operationLock.lock();
    }

    void unlock(void) {
        _m_operationLock.unlock();
    }

    void push(Event e) {
        lock();
        _queue.push_back(e);
        unlock();
    }

    bool empty(void) const {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        return _queue.empty();
    }

    Event pop_front(void) {
        lock();

        Event e;
        if (!empty()) {
            e = _queue.front();
            _queue.pop_front();
        }

        unlock();

        return e;
    }

    template<class UnaryPredicate>
    void remove_if(UnaryPredicate p) {
        lock();

        _queue.remove_if(p);

        unlock();
    }
};
