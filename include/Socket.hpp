#pragma once

#include <string>
#include <fcntl.h>
#include <cstddef>
#include <unistd.h>
#include <arpa/inet.h>

#include "Log.hpp"
#include "Args.hpp"

class Socket {
    int         _fd;

    std::string _remainder;

    std::string _data;
    std::size_t _dataSize;
    std::size_t _dataPos;

public:
    Socket(void);
    ~Socket(void);

    void setFd(int);
    void setDataPos(std::size_t);
    void setDataSize(std::size_t);
    void setData(const std::string &);
    void setRemainder(const std::string &);
    void removeRemainderBytes(int bytes);

    int getFd(void) const;
    std::size_t getDataPos(void) const;
    std::size_t getDataSize(void) const;
    const std::string &getData(void) const;
    const std::string &getRemainder(void) const;

    int read(void);
    int write(void);
    int nonblock(void);

    int socket(void);
    int connect(const std::string &addr, int port);
    int listen(const std::string &addr, int port);

    void clear(void);
    void reset(void);

    int resolveHostname(const std::string &host, struct sockaddr_in *resAddr);
};