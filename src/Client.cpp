#include "Client.hpp"

Client::Client(void) : connected(true) {}

Client::~Client(void) {}


// Connects new client to the back server
int Client::connect(const std::string &host, int port) {

    if (_backSock.socket() < 0) {
        Log.error() << "Server::connectClient:: cannot create socket" << Log.endl;
        return -1;
    }
    
    if (_backSock.connect(host, port) < 0) {
        Log.error() << "Server::connectClient:: cannot connect" << Log.endl;
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

time_point Client::getLastTime(void) const {
    return _lastTime;
}