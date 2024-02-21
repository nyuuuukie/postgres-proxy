#pragma once

#include "EventQueue.hpp"
#include "Event.hpp"

// Hierarchy could be changed to get rid of forward declarations
class Server;

namespace Globals {

    extern safeQueue<Event> eventQueue;
    extern Server server;

};

#include "Server.hpp"