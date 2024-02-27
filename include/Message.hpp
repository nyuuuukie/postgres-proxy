#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <arpa/inet.h>

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
    Stages _parseStage;

public:
    Message(void);
    ~Message(void);

    bool ready(void) const;

    char getId(void) const;
    std::size_t getLen(void) const;
    const std::string& getData(void) const;

    std::size_t totalLen(void) const;
    std::size_t parse(const std::string &newData);

private:
    void parseId(void);
    void parseLen(void);
    void parseData(void);

};

std::ostream& operator<<(std::ostream& out, const Message& msg);
