#include "Socket.hpp"

static const std::size_t BUFFER_SIZE = 4095;

Socket::Socket(void) : _fd(-1), _nonblocking(false), _dataPos(0) {
}

Socket::~Socket(void) {
    if (_fd != -1) {
        Log.debug() << "Socket:: [" << _fd << "] is closed" << Log.endl;
        close(_fd);
    }
}

int Socket::getFd(void) const {
    return _fd;
}

void Socket::setFd(int fd) {
    _fd = fd;
}

std::recursive_mutex& Socket::getLock(void) const {
    return _lock;
}

void Socket::lock(void) {
    _lock.lock();
}

void Socket::unlock(void) {
    _lock.unlock();
}

std::size_t Socket::getReadDataSize(void) const {
    std::lock_guard<std::recursive_mutex> l(_lock);
    return _readData.size();
}

const std::string& Socket::getFirstReadData(void) {
    std::lock_guard<std::recursive_mutex> l(_lock);
    return _readData.front();
}

const std::string& Socket::getLastReadData(void) {
    std::lock_guard<std::recursive_mutex> l(_lock);
    return _readData.back();
}

void Socket::removeFirstReadData(void) {
    std::lock_guard<std::recursive_mutex> l(_lock);
    _readData.pop_front();
}

void Socket::removeLastReadData(void) {
    std::lock_guard<std::recursive_mutex> l(_lock);
    _readData.pop_back();
}

void Socket::addWriteData(const std::string& s) {
    _writeData.push_back(s);
}

bool Socket::readyToWrite(void) const {
    return !_writeData.empty();
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
        Log.error() << "Socket::connect: cannot resolve hostname" << Log.endl;
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
        Log.crit() << "Socket::nonblock failed [" << _fd << "]" << Log.endl;
        return -1;
    }
    _nonblocking = true;
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
    std::string buffer;
    buffer.resize(BUFFER_SIZE + 1);

    const int bytes = ::read(_fd, &buffer[0], BUFFER_SIZE);

    if (bytes > 0) {
        buffer.resize(bytes);

        _lock.lock();
        _readData.push_back(std::move(buffer));
        _lock.unlock();

        Log.debug() << "Socket::read [" << _fd << "]: " << bytes << " bytes" << Log.endl;
    } else if (bytes == 0) {
        Log.debug() << "Socket::read finished" << Log.endl;
    } else {
        if (!_nonblocking) {
            Log.crit() << "Socket::nonblocking socket failed" << Log.endl;
        }
    }

    return bytes;
}

int Socket::write(void) {
    if (_writeData.empty()) {
        return 0;
    }

    const std::string& data = _writeData.front();

    long bytes = ::write(_fd, &data[_dataPos], data.size() - _dataPos);

    if (bytes > 0) {
        _dataPos += bytes;

        if (_dataPos >= data.size()) {
            Log.debug() << "Socket::write [" << _fd << "]: " << _dataPos << "/" << data.size() << " bytes" << Log.endl;
            _dataPos = 0;
            _writeData.pop_front();
        }
    }

    return bytes;
}
