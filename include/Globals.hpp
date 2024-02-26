#pragma once

#include "EventQueue.hpp"
#include "Event.hpp"

// Hierarchy could be changed to get rid of forward declarations
class Server;

namespace Globals {

    extern EventQueue eventQueue;
    extern Server server;

};

#include "Server.hpp"