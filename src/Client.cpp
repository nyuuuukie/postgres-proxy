#include "Client.hpp"

Client::Client(void) : _lastTime(Time::now()), connected(true), processing(false) {}

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

void Client::read(MessageList &list, Socket &socket) {
    if (socket.read() == 0) {
        Log.debug() << "Client::read disconnect" << Log.endl;
        connected = false;
        return ;
    }

    const std::string &data = socket.getRemainder();
    if (data.size() == 0) {
        // Log.debug() << "Client::read no data" << Log.endl;
        return ;
    }

    // Checking if previous request\response is not finished.
    Message *msg = nullptr;

    list.lock();    
    if (list.size() > 0 && !list.back()->ready()) {
        msg = list.pop();
    }
    list.unlock();

    // Create new request\response if last was finished,
    // otherwise create a new one.
    
    if (msg == nullptr) {
        Log.debug() << "Client::create new msg" << Log.endl;
        msg = new Message(data);
        
        if (msg == nullptr) {
            Log.crit() << "Client::readRequest: cannot allocate memory" << Log.endl;
            return ;
        }
    } else {
        Log.debug() << "Client::add data to msg" << Log.endl;
        msg->addData(data);
    }

    if (msg->parse()) {
        queryLog.info() << *msg << queryLog.endl;
        Log.debug() << "Client::read parsing completed" << Log.endl;
    } else {
        queryLog.debug() << *msg << queryLog.endl;
        Log.debug() << "Client::read parsing not completed" << Log.endl;
    }
    

    list.push(msg);

    // Delete bytes that were parsed 
    socket.removeRemainderBytes(msg->size());
}

void Client::pass(MessageList &list, Socket &socket) {
    Message *msg = nullptr;
    
    list.lock();    
    if (list.size() > 0 && list.back()->ready()) {
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
        Log.debug() << "Client::addReadEvent [" << fd << "]: rreq added to queue" << Log.endl;
        Globals::eventQueue.push({ this, Type::READ_REQUEST });

    } else if (fd == _backSock.getFd()) {
        Log.debug() << "Client::addReadEvent [" << fd << "]: rres added to queue" << Log.endl;
        Globals::eventQueue.push({ this, Type::READ_RESPONSE });
    }
}

void Client::addPassEvent(int fd) {
    using Type = Event::Type;

    if (fd == _frontSock.getFd()) {
        _responses.lock();
        if (_responses.size() > 0 && _responses.front()->ready()) {
            Log.debug() << "Client::addPassEvent [" << fd << "]: pres added to queue" << Log.endl;
            Globals::eventQueue.push({ this, Type::PASS_RESPONSE });
        }
        _responses.unlock();

    } else if (fd == _backSock.getFd()) {
        _requests.lock();
        if (_requests.size() > 0 && _requests.front()->ready()) {
            Log.debug() << "Client::addPassEvent [" << fd << "]: preq added to queue" << Log.endl;
            Globals::eventQueue.push({ this, Type::PASS_REQUEST });
        }
        _requests.unlock();
    }
}


void Client::readRequest(void) {
    // Log.debug() << "Client::readRequest" << Log.endl;

    read(_requests, _frontSock);
}

void Client::passRequest(void) {
    Log.debug() << "Client::passRequest" << Log.endl;
    pass(_requests, _backSock);
}

void Client::readResponse(void) {
    read(_responses, _backSock);
}

void Client::passResponse(void) {
    pass(_responses, _frontSock);
}
