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

    void readRequest(void);
    void readResponse(void);
    void passRequest(void);
    void passResponse(void);

    void addReadEvent(int fd);
    void addPassEvent(int fd);

    Message *pullMessage(MessageList& list);
    void logMessage(const Message *msg) const;

private:
    void read(MessageList& list, Socket& socket);
    int parse(MessageList& list, Socket& socket);
    void pass(MessageList& list, Socket& socket);
};
