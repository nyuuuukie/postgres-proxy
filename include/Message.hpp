#pragma once

#include <string>
#include <iostream>

// This class could be separated into 2 representing front and back
// messages respectively.

const std::string frontIds = "cdfpBCDEFHPQSX";
const std::string backIds = "123cdnstvACDEGHIKNRSTVWZ";

const int lenSize = 4;

class Message {

    std::string _data;
    char        _id;
    int         _len;
    int         _offset;
    char        _parseStage;

public:
    Message(const std::string &s);
    Message(const char *s);

    bool parse(void);
    int size(void) const;
    bool ready(void) const;

    void addData(const std::string &data);

    char getId(void) const;
    int getLen(void) const;
    const std::string &getData(void) const;


private:
    void parseId(void);
    void parseLen(void);
    void parseData(void);

};

std::ostream& operator<<(std::ostream &out, const Message &msg);
