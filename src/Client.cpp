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
        Log.error() << "Client::connect: cannot create socket" << Log.endl;
        return -1;
    }

    if (_backSock.connect(host, port) < 0) {
        Log.error() << "Client::connect:: failed" << Log.endl;
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


// Reads request\response and parses as much as possible.
void Client::read(Socket& s1, Socket& s2) {

    {
        std::scoped_lock<std::recursive_mutex> l(s1.getLock());

        const int bytes = s1.read();
        if (bytes == 0) {
            Log.debug() << "Disconnect client" << Log.endl;
            connected = false;
            return ;
        } else if (bytes < 0) {
            return ;
        }

        // Pass data to the other socket
        s2.addWriteData(s1.getLastReadData());
    }

    // Add parsing event with data
    if (&s1 == &_frontSock) {
        Globals::eventQueue.push_back({ this, Event::Type::PARSE_REQUEST });
    } else {
        Globals::eventQueue.push_back({ this, Event::Type::PARSE_RESPONSE });
    }

}

void Client::pollinHandler(int fd) {

    if (fd == _frontSock.getFd()) {
        Log.debug() << "Client:: [" << fd << "]: READ_REQUEST" << Log.endl;
        read(_frontSock, _backSock);
    
    } else if (fd == _backSock.getFd()) {
        Log.debug() << "Client:: [" << fd << "]: READ_RESPONSE" << Log.endl;
        read(_backSock, _frontSock);
    }
}

void Client::polloutHandler(int fd) {

    if (fd == _frontSock.getFd() && _frontSock.readyToWrite()) {
        Log.debug() << "Client:: [" << fd << "]: PASS_RESPONSE" << Log.endl;
        _frontSock.write();

    } else if (fd == _backSock.getFd() && _backSock.readyToWrite()) {
        Log.debug() << "Client:: [" << fd << "]: PASS_REQUEST" << Log.endl;
        _backSock.write();
    }
}

void Client::parseRequest(void) {
    parse(_requests, _frontSock);
}

void Client::parseResponse(void) {
    parse(_responses, _backSock);
}

void Client::parse(MessageList &list, Socket &sock) {
    const int fd = sock.getFd();

    // Log.debug() << "Client::parse [" << fd << "]: " << Log.endl;
    do {
        if (sock.getReadDataSize() == 0) {
            return ;
        }

        std::string data = sock.getFirstReadData();     
      
        // Log.debug() << "Client:: parse size " << data.size() << Log.endl;
        if (data.size() == 0) {
            break ;
        }
    
        for (std::size_t pos = 0, bytes = 0; pos < data.size(); pos += bytes) {
            Message* msg = pullMessage(list);
            if (msg == nullptr) {
                return;
            }

            // Log.debug() << "Client:: parse " << data.size() - pos << " " << pos << Log.endl;
            bytes = msg->parse(data, data.size() - pos, pos);
            if (bytes == 0) {
                break;
            }
        
            if (msg->ready()) {
                msg->log();
                // Log.debug() << "Message is ready" << Log.endl;

                // Messages could be stored instead
                delete msg;
            } else {
                list.push(msg);
            }

            Log.debug() << "Client::parse [" << fd << "]: " << bytes << " bytes" << Log.endl;
        }

        sock.removeFirstReadData();
        
    } while (sock.getReadDataSize() > 0);
}
