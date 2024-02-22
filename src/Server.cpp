#include "Server.hpp"

// Creates a new listening socket, binds it to the address and the port
// and starts listening on the port given.
int Server::initListenSocket(void) {

    if (_listSock.socket() < 0) {
        Log.crit() << "Server:: cannot create listening socket" << Log.endl;
        return -1;
    }

    if (_listSock.listen(Args::proxyHost, Args::proxyPort) < 0) {
        Log.crit() << "Server:: listen failed" << Log.endl;
        return -1;
    }

    addPollfdData({ _listSock.getFd(), POLLIN, 0 });

    return 0;
}

Server::Server(void) : _working(true) {
}

Server::~Server(void) {
}

bool Server::isWorking(void) const {
    return _working;
}

void Server::stop(void) {
    _working = false;
}

void sigintHandler(int) {
    std::cout << std::endl;
    Log.info() << "Server is stopping..." << Log.endl;

    Globals::server.stop();
}

void Server::start(void) {

    Log.info() << "Server starting on " << Args::proxyHost << ":" << Args::proxyPort << Log.endl;
    Log.info() << "Proxying to " << Args::targetHost << ":" << Args::targetPort << Log.endl;
    Log.info() << "Log level: " << Args::loglvl << Log.endl;
    Log.info() << "Log directory: " << Args::logdir << Log.endl;

    signal(SIGINT, sigintHandler);
    
    if (initListenSocket() < 0) {
        stop();
        return;
    }

    startWorkers();

    while (isWorking()) {

        if (poll() > 0) {
            process();
        }
    
        // checkClientTimeouts();
    
        deleteClients();
    }

    stopWorkers();
}

void Server::startWorkers(void) {

    for (int i = 0; i < Args::workersCount; ++i) {
        // add args if needed
        _workers.push_back(std::thread(workerCycle));
    }

}

void Server::stopWorkers(void) {

    for (auto &worker : _workers) {
        worker.join();
    }
}


void Server::process(void) {

    for (std::size_t i = 0; i < _pollfds.size(); i++) {

        const int fd = _pollfds[i].fd;

        // Skip in case of invalid fd or if nothing left to do
        if (fd < 0 || _pollfds[i].revents & POLLNVAL) {
            continue ;
        }

        if (i == 0) {
            if (_pollfds[i].revents & POLLIN) {
                addClient();
            }
        } else {
            if (_pollfds[i].revents & POLLERR) {
                pollerr(fd);
            } else if (_pollfds[i].revents & POLLHUP) {
                pollhup(fd);
            } else {
                if (_pollfds[i].revents & POLLIN) {
                    pollin(fd);
                }

                if (_pollfds[i].revents & POLLOUT) {
                    pollout(fd);
                }
            }
        }

        _pollfds[i].revents = 0;
    }
}

int Server::poll(void) {
    int res = ::poll(_pollfds.data(), _pollfds.size(), 100000);

    if (res < 0) {
        if (isWorking()) {
            Log.crit() << "Server::poll" << Log.endl;
        }
    }

    return res;
}


// Accepts a new client and returns fd of the created socket
int Server::acceptClient(void) {
    
    const int servFd = _listSock.getFd();

    struct sockaddr_in clientData;
    socklen_t clientLen = sizeof(clientData);
    const int clientFd = accept(servFd, reinterpret_cast<sockaddr *>(&clientData), &clientLen);

    return clientFd;
}

void Server::addPollfdData(struct pollfd pfd) {
    auto it = std::find_if(_pollfds.begin(), _pollfds.end(), [](pollfd pfd) { 
        return pfd.fd == -1; 
    });

    if (it == _pollfds.end()) {
        _pollfds.push_back(pfd);
    } else {
        *it = pfd;
    }
}

void Server::addClient(void) {
    
    Client *client = new Client();
    if (client == nullptr) {
        Log.crit() << "Server::Cannot allocate memory for client" << Log.endl;
        return ;
    }

    const int clientFrontSocketFd = acceptClient();
    if (clientFrontSocketFd < 0) {
        Log.crit() << "Server::accept failed" << Log.endl;
        delete client;
        return ;
    }

    Socket &frontSock = client->getFrontSocket();
    frontSock.setFd(clientFrontSocketFd);

    if (client->connect(Args::targetHost, Args::targetPort) < 0) {
        Log.error() << "Server::connect failed " << Log.endl;
        Log.error() << "Server::connect endpoint: " << Args::targetHost << ":" << Args::targetPort << Log.endl;
        delete client;
        return ;
    }

    Socket &backSock = client->getBackSocket();
    const int clientBackSocketFd = backSock.getFd();

    if (frontSock.nonblock() < 0 || backSock.nonblock() < 0) {
        delete client;
        return ;
    }
    
    // Probably not happen, but
    // check if map already contains these fds
    _clients[clientFrontSocketFd] = client;
    _clients[clientBackSocketFd] = client;

    // Add fds to pollfds vector
    addPollfdData({ clientFrontSocketFd, POLLIN | POLLOUT, 0 });
    addPollfdData({ clientBackSocketFd, POLLIN | POLLOUT, 0 });

    Log.debug() << "Server::connect [" << clientFrontSocketFd << "] -> [" << clientBackSocketFd << "]" << Log.endl;
}



void
Server::pollin(int fd) {

    using Type = Event::Type;

    Client *client = _clients[fd];
    if (client == nullptr) {
        return ;
    }

    if (fd == client->getFrontSocket().getFd()) {
        // read request from the client
        // check memory allocation
        Log.debug() << "Server::pollin [" << fd << "]: rreq added to queue" << Log.endl;
        // Globals::eventQueue.push(new Event({ client, Type::READ_REQUEST }));
        Globals::eventQueue.push({ client, Type::READ_REQUEST });


    } else if (fd == client->getBackSocket().getFd()) {
        // read response from the server
        // check memory allocation
        Log.debug() << "Server::pollin [" << fd << "]: rres added to queue" << Log.endl;
        // Globals::eventQueue.push( new Event({ client, Type::READ_RESPONSE }) );
        Globals::eventQueue.push({ client, Type::READ_RESPONSE });
    }
}

void
Server::pollout(int fd) {
    
    // using Type = Event::Type;

    Client *client = _clients[fd];
    if (client == nullptr) {
        return ;
    }

    if (fd == client->getFrontSocket().getFd()) {
        // send response to the client if have any
        // check memory allocation
        // Globals::eventQueue.push( new Event({ client, Type::PASS_RESPONSE }) );

    } else if (fd == client->getBackSocket().getFd()) {
        // send request to the server if have any
        // check memory allocation
        // Globals::eventQueue.push( new Event({ client, Type::PASS_REQUEST }) );
    }
}

void
Server::pollhup(int fd) {

    Log.debug() << "Server::pollhup [" << fd << "]" << Log.endl;
    Log.debug() << "Server::pollhup client has probably closed connection" << Log.endl;

    Client *client = _clients[fd];
    if (client == nullptr) {
        return ;
    }

    client->connected = false;
    // addToDelClientsSet(client);
}

void
Server::pollerr(int fd) {

    Log.crit() << "Server::pollerr [" << fd << "]" << Log.endl;

    Client *client = _clients[fd];
    if (client == nullptr) {
        return ;
    }

    // addToDelClientsSet(client);
    client->connected = false;
}


void Server::deleteClient(Client *client) {

    const int frontFd = client->getFrontSocket().getFd();
    const int backFd = client->getBackSocket().getFd();
    _clients[frontFd] = nullptr;
    _clients[backFd] = nullptr;

    // This could be improved not to run through whole vector for every client
    for (auto &p : _pollfds) {
        if (p.fd == frontFd || p.fd == backFd) {
            p.fd = -1;
        }
    }
    // for (int i = 0; i < _pollfds.size(); ++i) {
    //     if (_pollfds[i].fd == frontFd || _pollfds[i].fd == backFd) {
    //         _pollfds[i].fd = -1;
    //     }
    // }

}

void Server::checkClientTimeouts(void) {

    // Move to the constants or settings file
    const auto maxTimeout = std::chrono::seconds(20);

    // Get current time;
    const auto currentTime = std::chrono::system_clock::now();
    for (const auto& [fd, client] : _clients) {
        if (currentTime - client->getLastTime() > maxTimeout) {
            // addToDelClientsSet(client);
            Log.debug() << "Client is disconnecting..." << Log.endl;
            client->connected = false;
        }
    }
}




// Not sure what will be if worker tries to delete a client after main thread already deleted it.
// void Server::addToDelClientsSet(Client *client) {

//     _m_delClientsLock.lock();

//     _delClientsSet.insert(client);

//     _m_delClientsLock.unlock();
// }

// Maybe should be moved to separate class/file
// And maybe set should be used to avoid double free
void Server::deleteClients(void) {

    // _m_delClientsLock.lock();

    std::unordered_set<Client *> deleteClients;
    for (auto& [fd, client] : _clients) {
        if (client && !client->connected) { // and not processing
            deleteClients.insert(client);
        }
    }

    // Remove all clients requests from event queue
    Globals::eventQueue.remove_if([&set=deleteClients](Event e) {
        return set.count(e.client) > 0;
    });

    for (const auto &client : deleteClients) {
        deleteClient(client);
        delete client;
    }

    // _delClientsSet.clear();

    // _m_delClientsLock.unlock();
}