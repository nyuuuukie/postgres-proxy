#include "Socket.hpp"

static const std::size_t BUFFER_SIZE = 65536;

Socket::Socket(void) : _fd(-1), _dataSize(0), _dataPos(0) {
}

Socket::~Socket(void) {
    if (_fd != -1) {
        close(_fd);
    }
}

int Socket::getFd(void) const {
    return _fd;
}

void Socket::setFd(int fd) {
    _fd = fd;
}

const std::string& Socket::getRemainder(void) const {
    return _remainder;
}

void Socket::setRemainder(const std::string& rem) {
    _remainder = rem;
}

void Socket::removeRemainderBytes(int bytes) {
    if (bytes > 0) {
        _remainder.erase(0, bytes);
    }
}

void Socket::setData(const std::string& data) {
    _data = data;
    setDataSize(data.length());
}

void Socket::setDataSize(std::size_t size) {
    _dataSize = size;
}

void Socket::setDataPos(std::size_t pos) {
    _dataPos = pos;
}

const std::string& Socket::getData(void) const {
    return _data;
}

std::size_t Socket::getDataSize(void) const {
    return _dataSize;
}

std::size_t Socket::getDataPos(void) const {
    return _dataPos;
}

void Socket::clear(void) {
    _data = "";
    setDataPos(0);
    setDataSize(0);
}

void Socket::reset(void) {
    setFd(-1);
    clear();
    _remainder = "";
}

int Socket::socket() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        Log.crit() << "Socket::socket failed" << Log.endl;
    } else {
        setFd(fd);
    }

    return fd;
}

int Socket::resolveHostname(const std::string& host, struct sockaddr_in* resAddr) {
    addrinfo* lst = nullptr;
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int res = ::getaddrinfo(host.c_str(), nullptr, &hints, &lst);

    if (res == 0) {
        *resAddr = *(sockaddr_in*)(lst->ai_addr);
    }

    if (lst != nullptr) {
        ::freeaddrinfo(lst);
    }

    return (res == 0) ? 0 : -1;
}

int Socket::connect(const std::string& host, int port) {
    sockaddr_in addr = {};
    if (resolveHostname(host, &addr) < 0) {
        Log.error() << "Server::connectClient:: cannot resolve hostname" << Log.endl;
        return -1;
    }

    addr.sin_port = htons(port);
    if (::connect(_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        Log.error() << "Socket::connect failed" << Log.endl;
        return -1;
    }

    return _fd;
}

int Socket::nonblock(void) {
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
        Log.crit() << "Socket::fcntl(O_NONBLOCK) failed, fd: " << _fd << " " << Log.endl;
        return -1;
    }
    return 0;
}

int Socket::listen(const std::string& addr, int port) {
    int i = 1;
    struct sockaddr_in data;
    data.sin_family = AF_INET;
    data.sin_port = htons(port);
    data.sin_addr.s_addr = inet_addr(addr.c_str());

    if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) < 0) {
        Log.crit() << "Socket::setsockopt failed on [" << _fd << "], " << addr << ":" << port << Log.endl;
        return -1;
    }

    if (::bind(_fd, (struct sockaddr*)&data, sizeof(data)) < 0) {
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
    char buf[BUFFER_SIZE + 1] = {0};

    int bytes = ::read(_fd, buf, BUFFER_SIZE);

    if (bytes > 0) {
        buf[bytes] = '\0';

        _remainder.append(buf, bytes);
    }

    return bytes;
}

int Socket::write(void) {
    long bytes = ::write(_fd, _data.c_str() + _dataPos, _dataSize - _dataPos);

    if (bytes > 0) {
        _dataPos += bytes;

        if (_dataPos >= _dataSize) {
            Log.debug() << "Socket::write [" << _fd << "]: " << _dataPos << "/" << _dataSize << " bytes" << Log.endl;
            clear();
        }
    }

    return bytes;
}
