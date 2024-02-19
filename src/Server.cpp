#include "Server.hpp"


// Creates a new listening socket, binds it to the address and the port
// and starts listening on the port given.
int Server::initListenSocket(void) {

    if (_listSock.socket() < 0) {
        Log.crit() << "Server:: cannot create listening socket" << Log.endl;
        return -1;
    }

    if (_listSock.listen(Args::host, Args::port) < 0) {
        Log.crit() << "Server:: listen failed" << Log.endl;
        return -1;
    }

    _pollfds.push_back({ _listSock.getFd(), POLLIN, 0 });
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


void Server::start(void) {

    // Do later
    // signal(SIGINT, sigint_handler);
    
    if (initListenSocket() < 0) {
        stop();
        return;
    }

    // startWorkers();
    while (isWorking()) {

        if (poll() > 0) {
            process();
        }
    
        checkClientTimeouts();
    
        deleteClients();
    }

    // stopWorkers();
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
    // int res = ::poll(_pollfds.data(), _pollfds.size(), 100000);
    int res = ::poll(_pollfds.data(), _pollfds.size(), 0);

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
    
    if (clientFd < 0) {
        Log.crit() << "Server::connect::accept" << Log.endl;
        return -1;
    }

    return clientFd;
}

void Server::addClient(void) {
    
    Client *client = new Client();
    if (client == NULL) {
        Log.crit() << "Server::Cannot allocate memory for Client" << Log.endl;
        return ;
    }

    const int clientFrontSocketFd = acceptClient();

    Socket &frontSock = client->getFrontSocket();
    frontSock.setFd(clientFrontSocketFd);

    if (client->connect(Args::host, Args::port) < 0) {
        Log.error() << "Server::connect failed " << Log.endl;
        Log.error() << "Server::connect endpoint:" << Args::host << ":" << Args::port << Log.endl;
        delete client;
        return ;
    }

    Socket &backSock = client->getBackSocket();
    const int clientBackSocketFd = backSock.getFd();

    if (frontSock.nonblock() < 0 || backSock.nonblock() < 0) {
        delete client;
        return ;
    }
    
    // check if map already contains client with these fds ??
    _clients[clientFrontSocketFd] = client;
    _clients[clientBackSocketFd] = client;

    // Add fds to pollfds vector
    _pollfds.push_back({ clientFrontSocketFd, POLLIN | POLLOUT, 0 });
    _pollfds.push_back({ clientBackSocketFd, POLLIN | POLLOUT, 0 });

    Log.debug() << "Server::connect [" << clientFrontSocketFd << "] -> [" << clientBackSocketFd << "]" << Log.endl;
}


void
Server::pollin(int fd) {

    Client *client = _clients[fd];
    if (client == NULL) {
        return ;
    }

    if (fd == client->getFrontSocket().getFd()) {
        // read request from the client


    } else if (fd == client->getBackSocket().getFd()) {
        // read response from the server

    }
}

void
Server::pollout(int fd) {

    Client *client = _clients[fd];
    if (client == NULL) {
        return ;
    }

    if (fd == client->getFrontSocket().getFd()) {
        // send response to the client if have any

    } else if (fd == client->getBackSocket().getFd()) {
        // send request to the server if have any
    }
}

void
Server::pollhup(int fd) {

    Log.crit() << "Server::pollhup [" << fd << "]" << Log.endl;

    Client *client = _clients[fd];
    if (client == NULL) {
        return ;
    }

    // add client to delete q
}

void
Server::pollerr(int fd) {

    Log.crit() << "Server::pollerr [" << fd << "]" << Log.endl;

    Client *client = _clients[fd];
    if (client == NULL) {
        return ;
    }

    // add client to delete q
}


void Server::deleteClient(Client *client) {

    const int frontFd = client->getFrontSocket().getFd();
    const int backFd = client->getBackSocket().getFd();
    _clients[frontFd] = nullptr;
    _clients[backFd] = nullptr;

    for (int i = 0; i < _pollfds.size(); ++i) {
        if (_pollfds[i].fd == frontFd || _pollfds[i].fd == backFd) {
            _pollfds[i].fd = -1;
        }
    }

}

// Maybe should be moved to separate class/file
// And maybe set should be used to avoid double free
void Server::deleteClients(void) {

    _m_delClientsLock.lock();

    // Better option
    for (const auto &client : _delClientsSet) {
        deleteClient(client);
        delete client;
    }

    _delClientsSet.clear();

    _m_delClientsLock.unlock();
}


// Not sure what will be if worker tries to delete a client after main thread already deleted it.
void Server::addToDelClientsSet(Client *client) {

    _m_delClientsLock.lock();

    _delClientsSet.insert(client);

    _m_delClientsLock.unlock();
}


void Server::checkClientTimeouts(void) {

    // Move to the constants or settings file
    const auto maxTimeout = std::chrono::seconds(10);

    // Get current time;
    const auto currentTime = std::chrono::system_clock::now();
    for (const auto& [fd, client] : _clients) {
        if (currentTime - client->getLastTime() > maxTimeout) {
            // add client to delete q
        }
    }
}   