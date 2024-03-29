#pragma once

#include <list>
#include <mutex>
#include "Message.hpp"

class MessageList {
    mutable std::recursive_mutex _m_operationLock;

    std::list<Message*> _l;

public:
    void lock(void) {
        _m_operationLock.lock();
    }

    void unlock(void) {
        _m_operationLock.unlock();
    }

    void push(Message* msg) {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        _l.push_back(msg);
    }

    Message* back(void) const {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        return _l.back();
    }

    Message* front(void) const {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        return _l.front();
    }

    bool empty(void) const {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        return _l.empty();
    }

    std::size_t size(void) const {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);
        return _l.size();
    }

    Message* pop_front(void) {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);

        Message* msg = nullptr;
        if (!empty()) {
            msg = _l.front();
            _l.pop_front();
        }

        return msg;
    }

    Message* pop(void) {
        std::lock_guard<std::recursive_mutex> lk(_m_operationLock);

        Message* msg = nullptr;
        if (!empty()) {
            msg = _l.back();
            _l.pop_back();
        }

        return msg;
    }
};