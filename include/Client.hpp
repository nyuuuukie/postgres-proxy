#pragma once

#include "Socket.hpp"
#include "Log.hpp"
#include "EventQueue.hpp"
#include "MessageList.hpp"
#include "Globals.hpp"

class Client {
    // Front socket is the one that communicates with
    // the client and the back one is the one that 
    // communicates with the backend server
    Socket _frontSock;
    Socket _backSock;

    // time for timeout, updated every send or recv
    time_point _lastTime;

    MessageList _requests;
    MessageList _responses;

public:
    std::atomic<bool> connected;
    std::atomic<bool> processing;

    Client(void);
    ~Client(void);

    Socket &getFrontSocket(void);
    Socket &getBackSocket(void);

    int connect(const std::string &host, int port);

    time_point getLastTime(void) const;

    void readRequest(void);
    void readResponse(void);
    void parseRequest(void);
    void parseResponse(void);
    void passRequest(void);
    void passResponse(void);

    void addReadEvent(int fd);
    void addParseEvent(int fd);
    void addPassEvent(int fd);

private:
    void read(Socket &socket);
    void parse(MessageList &list, Socket &socket);
    void pass(MessageList &list, Socket &socket);

};
