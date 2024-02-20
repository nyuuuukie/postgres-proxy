#pragma once

#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Args.hpp"
#include "Client.hpp"
#include "Globals.hpp"

class Server {

    std::vector<struct pollfd>  _pollfds;
    std::unordered_map<int, Client *> _clients;

    bool _working;

    Socket _listSock;

    void start(void);
    void stop(void);
    
    void process(void);
    void addClient(void);
    void deleteClient(Client *client);
    
    int poll(void);

    void pollin(int fd);
    void pollhup(int fd);
    void pollerr(int fd);
    void pollout(int fd);

    int acceptClient(void);
    int initListenSocket(void);

    std::mutex _m_delClientsLock;
    std::unordered_set<Client *> _delClientsSet;

    void deleteClients(void);
    void addToDelClientsSet(Client *client);
    void addPollfdData(struct pollfd pfd);

    void checkClientTimeouts(void);

    bool isWorking() const;
};