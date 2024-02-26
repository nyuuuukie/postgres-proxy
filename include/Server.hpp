#pragma once

#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>

#include "Args.hpp"
#include "Client.hpp"
#include "Globals.hpp"
#include "Worker.hpp"

class Server {

    std::vector<struct pollfd>  _pollfds;
    std::unordered_map<int, Client *> _clients;

    bool _working;

    Socket _listSock;

    std::vector<std::thread> _workers;

public:
    Server(void);
    ~Server(void);

    void start(void);
    void stop(void);
    bool isWorking(void) const;

private: 
    int initListenSocket(void);
    void process(void);

    int poll(void);
    void pollin(int fd);
    void pollhup(int fd);
    void pollerr(int fd);
    void pollout(int fd);
    void checkParseEvent(int fd);

    int acceptClient(void);
    void addClient(void);
    void deleteClients(void);
    void addPollfdData(struct pollfd pfd);
    void deleteClient(Client *client);

    void checkClientTimeouts(void);

    void startWorkers(void);
    void stopWorkers(void);
};