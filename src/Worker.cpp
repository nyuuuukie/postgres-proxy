#include "Worker.hpp"

Event pullEvent(void) {
    Event event;

    Globals::eventQueue.lock();
    if (!Globals::eventQueue.empty()) {
        event = Globals::eventQueue.pop_front();

        if (event.type != Event::Type::NONE && event.client) {
            // Only one thread will be working with a client
            if (event.client->processing.load()) {
                Log.debug() << "Client already processing" << Log.endl;
                event = {};    
            } else if (!event.client->connected.load()) {
                Log.debug() << "Client not exist or disconnected" << Log.endl;
                event = {};
            } else {
                event.client->processing = true;
            }
        }
    }
    Globals::eventQueue.unlock();

    return event;
}

void handleEvent(Event event) {
    if (event.type == Event::Type::PARSE_REQUEST) {
        event.client->parseRequest();
    } else if (event.type == Event::Type::PARSE_RESPONSE) {
        event.client->parseResponse();
    } else {
        Log.error() << "Unknown event " << Log.endl;
    }

    event.client->processing = false;
}

void workerCycle(void) {
    Log.debug() << "Worker " << std::this_thread::get_id() << " started" << Log.endl;

    while (Globals::server->isWorking()) {
        Event event = pullEvent();

        if (!event.isOperative()) {
            continue;
        }

        handleEvent(event);
    }

    Log.debug() << "Worker " << std::this_thread::get_id() << " stopped" << Log.endl;
}