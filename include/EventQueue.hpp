#pragma once

#include <list>
#include <mutex>
#include <functional>
#include "Event.hpp"
#include "Log.hpp"

class Client;

class EventQueue {
    mutable std::recursive_mutex _m_operationLock;
    std::list<Event> _queue;

public:
    void lock(void);
    void unlock(void);

    Event pop_front(void);
    void push_back(Event e);

    bool empty(void) const;

    void remove_if(const std::function<bool(Event)> &f);

    Event pull(void);
};

#include "Client.hpp"
