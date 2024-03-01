#pragma once

#include "Event.hpp"
#include "EventQueue.hpp"

// Hierarchy could be changed to get rid of forward declarations
class Server;

namespace Globals {
    extern EventQueue eventQueue;
    extern Server *server;
};  // namespace Globals

#include "Server.hpp"