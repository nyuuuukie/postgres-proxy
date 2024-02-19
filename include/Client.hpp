#pragma once

#include "Socket.hpp"
#include "Log.hpp"
#include "Utils.hpp"

class Client {
    // Front socket is the one that communicates with
    // the client and the back one is the one that 
    // communicates with the backend server
    Socket _frontSock;
    Socket _backSock;

public:
    Client(void);
    ~Client(void);

    int connect(const std::string &host, int port);

    Socket &getFrontSocket(void);
    Socket &getBackSocket(void);

};
