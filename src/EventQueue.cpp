#include "EventQueue.hpp"

void EventQueue::lock(void) {
    _m_operationLock.lock();
}

void EventQueue::unlock(void) {
    _m_operationLock.unlock();
}

void EventQueue::push_back(Event e) {
    std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
    _queue.push_back(e);
}

bool EventQueue::empty(void) const {
    std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
    return _queue.empty();
}

Event EventQueue::pop_front(void) {
    std::lock_guard<std::recursive_mutex> lk(_m_operationLock);

    Event e;
    if (!empty()) {
        e = _queue.front();
        _queue.pop_front();
    }

    return e;
}

void EventQueue::remove_if(const std::function<bool(Event)> &delPredicate) {
    std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
    _queue.remove_if(delPredicate);
}

Event EventQueue::pull(void) {
    std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
    Event event;

    if (!empty()) {
        event = pop_front();

        if (event.type != Event::Type::NONE && event.client) {
            // Only one thread will be working with a client
            if (event.client->processing.load()) {
                Log.debug() << "Client already processing" << Log.endl;
                event = {};    
            } else if (!event.client->connected.load()) {
                Log.debug() << "Client not exist or disconnected" << Log.endl;
                event = {};
            } else {
                event.client->processing.store(true);
            }
        }
    }

    return event;
}