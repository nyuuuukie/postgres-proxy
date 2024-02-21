#include "Worker.hpp"
#include "Server.hpp"

void   workerCycle(void) {

    Log.debug() << "Worker " << std::this_thread::get_id() << ": cycle started" << Log.endl;

    while (Globals::server.isWorking()) {
    
        Event *event = nullptr;
    
        Globals::eventQueue.lock();
        if (!Globals::eventQueue.empty()) {
            event = Globals::eventQueue.pop_front();
        }
        Globals::eventQueue.unlock();


        if (event == nullptr) {
            // use worker_timeout variable
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // handle event
    }

    Log.debug() << "Worker " << std::this_thread::get_id() << ": cycle stopped" << Log.endl;

}