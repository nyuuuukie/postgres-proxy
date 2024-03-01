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

    std::recursive_mutex& getLock(void) const;

    void setFd(int);
    // void setDataPos(std::size_t);
    // void setDataSize(std::size_t);
    // void setData(const std::string&);
    // void setRemainder(const std::string&);
    // void removeRemainderBytes(int bytes);

    // void addData(const std::string&);
    bool readyToWrite(void) const;
    int getFd(void) const;
    // std::size_t getDataPos(void) const;
    // std::size_t getDataSize(void) const;
    // const std::string& getData(void) const;
    std::size_t getReadDataSize(void) const;
    const std::string& getFirstReadData(void);
    const std::string& getLastReadData(void);
    void removeFirstReadData(void);
    void removeLastReadData(void);

    // const std::string& getReadData(void) const;
    void addWriteData(const std::string& s);

    int read(void);
    int write(void);
    int nonblock(void);

    int socket(void);
    int connect(const std::string& addr, int port);
    int listen(const std::string& addr, int port);

    // void clear(void);
    // void reset(void);

    void lock(void);
    void unlock(void);

    int resolveHostname(const std::string& host, struct sockaddr_in* resAddr);
};