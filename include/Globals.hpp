#pragma once

#include "Server.hpp"
#include "EventQueue.hpp"
#include "Event.hpp"

namespace Globals {

    extern safeQueue<Event> eventQueue;
    extern Server server;

};
