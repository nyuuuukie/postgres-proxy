#pragma once


// Hierarchy could be changed to get rid of forward declarations
class Server;
class EventQueue;

namespace Globals {
    extern EventQueue eventQueue;
    extern Server *server;
};  // namespace Globals

#include "EventQueue.hpp"
#include "Server.hpp"