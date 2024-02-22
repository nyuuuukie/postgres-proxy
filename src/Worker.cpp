#include "Worker.hpp"
#include "Server.hpp"

void   workerCycle(void) {

    Log.debug() << "Worker " << std::this_thread::get_id() << ": cycle started" << Log.endl;

    while (Globals::server.isWorking()) {
    
        Event event;
    
        Globals::eventQueue.lock();
        if (!Globals::eventQueue.empty()) {
            event = Globals::eventQueue.pop_front();
            
            if (event.type != Event::Type::NONE) {
                if (event.client && event.client->connected) {
                    event.client->processing = true;
                } else {
                    event = {};
                }
            }
        }
        Globals::eventQueue.unlock();


        if (!event.isOperative()) {
            // use worker_timeout variable
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // handle event
        // event.client->handleEvent(event);
               if (event.type == Event::Type::READ_REQUEST) {
            event.client->readRequest();
        } else if (event.type == Event::Type::PASS_REQUEST) {
            event.client->passRequest();
        } else if (event.type == Event::Type::READ_RESPONSE) {
            event.client->readResponse();
        } else if (event.type == Event::Type::PASS_RESPONSE) {
            event.client->passResponse();
        } else {
            Log.error() << "Unknown event " << Log.endl;
        }
    }

    Log.debug() << "Worker " << std::this_thread::get_id() << ": cycle stopped" << Log.endl;

}