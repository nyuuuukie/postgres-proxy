#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cstddef>
#include <list>
#include <string>

#include "Args.hpp"
#include "Log.hpp"

class Socket {
    int _fd;
    bool _nonblocking;

    mutable std::recursive_mutex _lock;
    std::list<std::string> _readData;
    std::list<std::string> _writeData;

    std::size_t _dataPos;

public:
    Socket(void);
    ~Socket(void);

    void lock(void);
    void unlock(void);
    std::recursive_mutex& getLock(void) const;

    int getFd(void) const;
    void setFd(int);

    bool readyToWrite(void) const;
    std::size_t getReadDataSize(void) const;
    const std::string& getFirstReadData(void);
    const std::string& getLastReadData(void);
    void removeFirstReadData(void);
    void removeLastReadData(void);
    void addWriteData(const std::string&);

    int read(void);
    int write(void);
    int nonblock(void);

    int socket(void);
    int connect(const std::string& addr, int port);
    int listen(const std::string& addr, int port);

    int resolveHostname(const std::string& host, struct sockaddr_in* resAddr);
};