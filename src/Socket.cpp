#include "Socket.hpp"
#include "Server.hpp"

static const std::size_t BUFFER_SIZE = 65536;

Socket::Socket(void) 
    : _fd(-1)
    // , _port(0)
    , _dataSize(0)
    , _dataPos(0) {}

Socket::~Socket(void) {
    if (_fd != -1) {
        close(_fd);
    }
}

int
Socket::getFd(void) const { 
    return _fd;
}

void
Socket::setFd(int fd) {
    _fd = fd;
}

// void
// Socket::setAddr(const std::string &addr) {
//     _addr = addr;
// }

// int
// Socket::getPort(void) const {
//     return _port;
// }

// void
// Socket::setPort(std::size_t port) {
//     _port = port;
// }

// const std::string &
// Socket::getAddr(void) const {
//     return _addr;
// }

const std::string &
Socket::getRem(void) const {
    return _rem;
}

void
Socket::setData(const std::string &data) {
    _data = data;
    setDataSize(data.length());
}

void
Socket::setDataSize(std::size_t size) {
    _dataSize = size;
}

void
Socket::setDataPos(std::size_t pos) {
    _dataPos = pos;
}

const std::string &
Socket::getData(void) const {
    return _data;
}

std::size_t
Socket::getDataSize(void) const {
    return _dataSize;
}

std::size_t
Socket::getDataPos(void) const {
    return _dataPos;
}

void
Socket::clear(void) {
    _data = "";
    setDataPos(0);
    setDataSize(0);
}

void
Socket::reset(void) {
    setFd(-1);
    // setAddr("");
    // setPort(0);
    clear();
    _rem = "";
}


int
Socket::socket() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        Log.crit() << "Socket::socket failed" << Log.endl;
    } else {
        setFd(fd);
    }

    return fd;
}

int
Socket::connect(const std::string &host, int port) {

    sockaddr_in addr = {};
    if (resolveHostname(host, &addr) < 0) {
        Log.error() << "Server::connectClient:: cannot resolve hostname" << Log.endl;
        return -1;
    }

    addr.sin_port = htons(port);
    if (::connect(_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        Log.error() << "Socket::connect failed" << Log.endl;
        return -1;
    }

    return _fd;
}

int
Socket::nonblock(void) {
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
        Log.crit() << "Socket::fcntl(O_NONBLOCK) failed, fd: " << _fd << " " << Log.endl;
        return -1;
    }
    return 0;
}

int
Socket::listen(const std::string &addr, int port) {

    int i = 1;
    struct sockaddr_in data;
    data.sin_family = AF_INET;
    data.sin_port   = htons(port);
    data.sin_addr.s_addr = inet_addr(addr.c_str());

    // setAddr(addr);
    // setPort(port);

    if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) < 0) {
        Log.crit() << "Socket::setsockopt failed on [" << _fd << "], " << addr << ":" << port << Log.endl;
        return -1;
    }

    if (::bind(_fd, (struct sockaddr *)&data, sizeof(data)) < 0) {
        Log.crit() << "Socket::bind failed on [" << _fd << "], " << addr << ":" << port << Log.endl;
        return -1;
    }

    if (::listen(_fd, Args::backlog) < 0) {
        Log.crit() << "Socket::listen failed on [" << _fd << "]," << addr << ":" << port << Log.endl;
        return -1;
    }

    Log.info() << "Listening on " << addr << ":" << port << Log.endl;
    return _fd;
}

int Socket::read(void) {

    char buf[BUFFER_SIZE + 1] = { 0 };

    int bytes = ::read(_fd, buf, BUFFER_SIZE);
 
    if (bytes > 0) {
        buf[bytes] = '\0';
        
        _rem.append(buf, bytes);
        return bytes;
    } 

    return bytes;
}

int
Socket::write(void) {

    long bytes = ::write(_fd,  _data.c_str() + _dataPos, _dataSize - _dataPos);
    
    if (bytes > 0) {
        _dataPos += bytes;
    
        if (_dataPos >= _dataSize) {
            Log.debug() << "Socket::write [" << _fd << "]: " << _dataPos << "/" << _dataSize << " bytes" << Log.endl;
            clear();
        }
    }

    return bytes;
}
