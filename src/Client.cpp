#include "Client.hpp"

// Connects new client to the back server
int Client::connect(const std::string &host, int port) {

    if (_backSock.socket() < 0) {
        Log.error() << "Server::connectClient:: cannot create socket" << Log.endl;
        return -1;
    }
    
    sockaddr_in addr = {};
    if (resolveHostname(host, &addr) < 0) {
        Log.error() << "Server::connectClient:: cannot resolve hostname" << Log.endl;
        return -1;
    }

    addr.sin_port = htons(port);
    if (::connect(_backSock.getFd(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        return -1;
    }

    return 0;
}

Socket & Client::getFrontSocket(void) {
    return _frontSock;
}

Socket & Client::getBackSocket(void) {
    return _backSock;
}