#pragma once

#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>

#include <vector>
#include <unordered_map>

#include "Args.hpp"
#include "Client.hpp"

class Server {

    std::vector<struct pollfd>  _pollfds;
    std::unordered_map<int, Client *> _clients;

    bool _working;

    Socket _listSock;

    void start(void);
    void stop(void);
    
    void process(void);
    void addClient(void);
    int poll(void);

    void pollin(int fd);
    void pollhup(int fd);
    void pollerr(int fd);
    void pollout(int fd);

    int acceptClient(void);
    int initListenSocket(void);
    
    bool isWorking() const;
};