#include "Client.hpp"

Client::Client(void) : connected(true), processing(false) {
}

Client::~Client(void) {
    while (_requests.size()) {
        Message* msg = _requests.back();
        _requests.pop();
        delete msg;
    }

    while (_responses.size()) {
        Message* msg = _responses.back();
        _responses.pop();
        delete msg;
    }
}

Socket& Client::getFrontSocket(void) {
    return _frontSock;
}

Socket& Client::getBackSocket(void) {
    return _backSock;
}

// Connects new client to the back server
int Client::connect(const std::string& host, int port) {
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

// Returns the last message in the list if it's not parsed fully.
// Otherwise, allocates a new message and returns it.
Message* Client::pullMessage(MessageList& list) {
    Message* msg = nullptr;
    list.lock();
    if (list.size() > 0 && !list.back()->ready()) {
        msg = list.pop();
    }
    list.unlock();

    if (msg == nullptr) {
        msg = new Message();

        if (msg == nullptr) {
            Log.crit() << "Client::parse: cannot allocate memory" << Log.endl;
        }
    }

    return msg;
}

void Client::logMessage(const Message* msg) const {
    if (Args::logAllMessages) {
        queryLog.print() << *msg << queryLog.endl;
    } else {
        // Print query SQL requests
        if (msg->getId() == 'Q') {
            queryLog.print() << &msg->getData()[5] << queryLog.endl;
        }
    }
}

// Parses a message and returns amount of bytes that were processed
int Client::parse(MessageList& list, Socket& socket) {
    const std::string& data = socket.getRemainder();
    if (data.size() == 0) {
        return 0;
    }

    Message* msg = pullMessage(list);
    if (msg == nullptr) {
        return 0;
    }

    const int parsedBytes = msg->parse(data);
    if (msg->ready()) {
        logMessage(msg);
    }

    list.push(msg);

    Log.debug() << "Client::parse [" << socket.getFd() << "]: " << parsedBytes << " bytes" << Log.endl;
    return parsedBytes;
}

// Reads request\response and parses as much as possible.
void Client::read(MessageList& list, Socket& socket) {
    if (socket.read() == 0) {
        Log.debug() << "Client::end of read, disconnect" << Log.endl;
        connected = false;
        return;
    }

    int bytes = 0;
    do {
        bytes = parse(list, socket);

        // Delete bytes that were parsed
        socket.removeRemainderBytes(bytes);
    } while (bytes);
}

// Passing ready request to the backend server,
// or ready response to the client.
void Client::pass(MessageList& list, Socket& socket) {
    Message* msg = nullptr;

    list.lock();
    if (list.size() > 0 && list.front()->ready()) {
        msg = list.pop_front();
    }
    list.unlock();

    if (msg != nullptr) {
        socket.setData(msg->getData());
        socket.write();

        delete msg;
    }
}

void Client::addReadEvent(int fd) {
    using Type = Event::Type;

    if (fd == _frontSock.getFd()) {
        Log.debug() << "Client:: [" << fd << "]: READ_REQUEST event added" << Log.endl;
        Globals::eventQueue.push({this, Type::READ_REQUEST});

    } else if (fd == _backSock.getFd()) {
        Log.debug() << "Client:: [" << fd << "]: READ_RESPONSE event added" << Log.endl;
        Globals::eventQueue.push({this, Type::READ_RESPONSE});
    }
}

void Client::addPassEvent(int fd) {
    using Type = Event::Type;

    if (fd == _frontSock.getFd()) {
        _responses.lock();
        if (_responses.size() > 0 && _responses.front()->ready()) {
            Log.debug() << "Client:: [" << fd << "]: PASS_RESPONSE event added" << Log.endl;
            Globals::eventQueue.push({this, Type::PASS_RESPONSE});
        }
        _responses.unlock();

    } else if (fd == _backSock.getFd()) {
        _requests.lock();
        if (_requests.size() > 0 && _requests.front()->ready()) {
            Log.debug() << "Client:: [" << fd << "]: PASS_REQUEST event added" << Log.endl;
            Globals::eventQueue.push({this, Type::PASS_REQUEST});
        }
        _requests.unlock();
    }
}

void Client::readRequest(void) {
    read(_requests, _frontSock);
}

void Client::passRequest(void) {
    pass(_requests, _backSock);
}

void Client::readResponse(void) {
    read(_responses, _backSock);
}

void Client::passResponse(void) {
    pass(_responses, _frontSock);
}
