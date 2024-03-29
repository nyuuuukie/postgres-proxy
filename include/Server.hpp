#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>

#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Args.hpp"
#include "Client.hpp"
#include "Socket.hpp"

class Server {
    std::vector<struct pollfd> _pollfds;
    std::unordered_map<int, Client *> _clients;
    std::vector<std::thread> _workers;

    std::atomic<bool> _working;
    Socket _listSock;

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

    int acceptClient(void);
    void addClient(void);
    void addPollfdData(struct pollfd pfd);
    void deleteClient(Client* client);
    void deleteClients(void);
    void disconnectClients(void);

    void startWorkers(void);
    void stopWorkers(void);

    void startupInfo(void);
};