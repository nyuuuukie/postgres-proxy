#pragma once

#include <list>
#include <mutex>
#include "Event.hpp"
#include "Log.hpp"
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

    void push_back(Event e) {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        _queue.push_back(e);
    }

    bool empty(void) const {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        return _queue.empty();
    }

    Event pop_front(void) {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);

        Event e;
        if (!empty()) {
            e = _queue.front();
            _queue.pop_front();
        }

        return e;
    }

    template <class UnaryPredicate>
    void remove_if(UnaryPredicate p) {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        _queue.remove_if(p);
    }
};
