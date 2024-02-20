#pragma once

#include "Socket.hpp"
#include "Log.hpp"

class Client {
    // Front socket is the one that communicates with
    // the client and the back one is the one that 
    // communicates with the backend server
    Socket _frontSock;
    Socket _backSock;

    // time for timeout, updated every send or recv
    time_point _lastTime;


    

public:
    Client(void);
    ~Client(void);

    int connect(const std::string &host, int port);

    Socket &getFrontSocket(void);
    Socket &getBackSocket(void);

    time_point getLastTime(void) const;

    std::atomic<bool> connected;
};
