#pragma once

#include <atomic>

#include "Globals.hpp"
#include "MessageList.hpp"
#include "Socket.hpp"

class Client {
    // Front socket is the one that communicates with
    // the client and the back one is the one that
    // communicates with the backend server
    Socket _frontSock;
    Socket _backSock;

    MessageList _requests;
    MessageList _responses;

public:
    std::atomic<bool> connected;
    std::atomic<bool> processing;

    Client(void);
    ~Client(void);

    Socket& getFrontSocket(void);
    Socket& getBackSocket(void);

    int connect(const std::string& host, int port);

    void parseRequest(void);
    void parseResponse(void);
    
    void pollinHandler(int fd);
    void polloutHandler(int fd);

    Message *pullMessage(MessageList& list);

private:
    void read(Socket& s1, Socket& s2);
    void parse(MessageList &, Socket &);
};
