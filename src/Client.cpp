#include "Client.hpp"

Client::Client(void) : connected(true), processing(false) {}

Client::~Client(void) {}

Socket & Client::getFrontSocket(void) {
    return _frontSock;
}

Socket & Client::getBackSocket(void) {
    return _backSock;
}

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

void Client::parse(MessageList &list, Socket &socket) {

    const std::string &data = socket.getRemainder();
    if (data.size() == 0) {
        return ;
    }

    Message *msg = nullptr;
    list.lock();
    if (list.size() > 0 && !list.back()->ready()) {
        msg = list.pop();
    }
    list.unlock();


    // Create new request\response if last was finished,
    // otherwise create a new one.
    if (msg == nullptr) {
        msg = new Message(data);
        
        if (msg == nullptr) {
            Log.crit() << "Client::parse: cannot allocate memory" << Log.endl;
            return ;
        }
    } else {
        msg->addData(data);
    }

    if (msg->parse()) {
        // Print query SQL requests
        if (msg->getId() == 'Q') {
            queryLog.print() << &msg->getData()[5] << queryLog.endl;
        }
    }

    list.push(msg);

    // Delete bytes that were parsed 
    socket.removeRemainderBytes(msg->size());
}

void Client::read(Socket &socket) {
    if (socket.read() == 0) {
        Log.debug() << "Client::end of read, disconnect" << Log.endl;
        connected = false;
        return ;
    }
}

void Client::pass(MessageList &list, Socket &socket) {
    Message *msg = nullptr;
    
    list.lock();    
    if (list.size() > 0 && list.back()->ready()) {
        msg = list.pop_front();
    }
    list.unlock();

    if (msg != nullptr) {
        queryLog.print() << *msg << queryLog.endl;
        socket.setData(msg->getData());

        socket.write();

        delete msg;
    }
}

void Client::addReadEvent(int fd) {
    using Type = Event::Type;

    if (fd == _frontSock.getFd()) {
        Log.debug() << "Client::addReadEvent [" << fd << "]: READ_REQUEST event added" << Log.endl;
        Globals::eventQueue.push({ this, Type::READ_REQUEST });

    } else if (fd == _backSock.getFd()) {
        Log.debug() << "Client::addReadEvent [" << fd << "]: READ_RESPONSE event added" << Log.endl;
        Globals::eventQueue.push({ this, Type::READ_RESPONSE });
    }
}

void Client::addParseEvent(int fd) {
    using Type = Event::Type;

    if (fd == _frontSock.getFd()) {
        
        if (!_frontSock.getRemainder().empty()) {
            Log.debug() << "Client::addParseEvent [" << fd << "]: PARSE_REQUEST event added" << Log.endl;
            Globals::eventQueue.push({ this, Type::PARSE_REQUEST });
        }

    } else if (fd == _backSock.getFd()) {
        if (!_backSock.getRemainder().empty()) {
            Log.debug() << "Client::addParseEvent [" << fd << "]: PARSE_RESPONSE event added" << Log.endl;
            Globals::eventQueue.push({ this, Type::PARSE_RESPONSE });
        }
    }
}

void Client::addPassEvent(int fd) {
    using Type = Event::Type;

    if (fd == _frontSock.getFd()) {
        _responses.lock();
        if (_responses.size() > 0 && _responses.front()->ready()) {
            Log.debug() << "Client::addPassEvent [" << fd << "]: PASS_RESPONSE event added" << Log.endl;
            Globals::eventQueue.push({ this, Type::PASS_RESPONSE });
        }
        _responses.unlock();

    } else if (fd == _backSock.getFd()) {
        _requests.lock();
        if (_requests.size() > 0 && _requests.front()->ready()) {
            Log.debug() << "Client::addPassEvent [" << fd << "]: PASS_REQUEST event added" << Log.endl;
            Globals::eventQueue.push({ this, Type::PASS_REQUEST });
        }
        _requests.unlock();
    }
}


void Client::readRequest(void) {
    read(_frontSock);
}

void Client::passRequest(void) {
    pass(_requests, _backSock);
}

void Client::readResponse(void) {
    read(_backSock);
}

void Client::passResponse(void) {
    pass(_responses, _frontSock);
}

void Client::parseRequest(void) {
    parse(_requests, _frontSock);
}

void Client::parseResponse(void) {
    parse(_responses, _backSock);
}
