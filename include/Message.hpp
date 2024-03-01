#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <arpa/inet.h>

#include "Args.hpp"
// This class could be separated into 2 representing front and back
// messages respectively.

const std::string frontIds = "cdfpBCDEFHPQSX";
const std::string backIds = "123cdnstvACDEGHIKNRSTVWZ";

const std::size_t lenFieldSize = 4;

class Message {

    enum class Stages { ID, LENGTH, DATA, DONE };

    std::string _data;
    char _id;
    std::size_t _len;
    std::size_t _dataLen;
    std::size_t _offset;
    Stages _parseStage;
    
    std::string _message;

public:
    Message(void);
    ~Message(void);

    bool ready(void) const;

    char getId(void) const;
    std::size_t getLen(void) const;
    const std::string& getData(void) const;

    std::size_t parse(const std::string &data, std::size_t dataSize, std::size_t pos);

    void log(void) const;

    std::size_t idLen(void) const;
    std::size_t totalLen(void) const;
    std::size_t headerLen(void) const;

private:
    std::size_t parseId(const std::string &data, std::size_t dataSize, std::size_t pos);
    std::size_t parseLen(const std::string &data, std::size_t dataSize, std::size_t pos);
    std::size_t parseData(const std::string &data, std::size_t dataSize, std::size_t pos);

};

std::ostream& operator<<(std::ostream& out, const Message& msg);
