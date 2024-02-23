#include "Message.hpp"

Message::Message(const std::string &s) : _data(s), _id(0), _len(0), _offset(0), _parseStage(0) {}

Message::Message(const char *s) : _data(s), _id(0), _len(0), _offset(0), _parseStage(0) {}

int Message::size(void) const {
    return _offset;
}

bool Message::ready(void) const {
    return _parseStage == 3;
}

void Message::addData(const std::string &data) {
    _data += data;
}

const std::string &Message::getData(void) const {
    return _data;
}

char Message::getId(void) const {
    return _id;
}

int Message::getLen(void) const {
    return _len;
}

void Message::parseId(void) {
    if (frontIds.find(_data[0]) != std::string::npos || \
        backIds.find(_data[0]) != std::string::npos) {
        _id = _data[0];
        _offset++;
    }
    _parseStage = 1;
}

void Message::parseLen(void) {
    const int o1 = static_cast<int>(_data[_offset + 3]) << 24;
    const int o2 = static_cast<int>(_data[_offset + 2]) << 16;
    const int o3 = static_cast<int>(_data[_offset + 1]) << 8;
    const int o4 = static_cast<int>(_data[_offset + 0]);
    _len = ntohl(o1 | o2 | o3 | o4);
    // Equivalent to _len = ntohl(*(int *)&_data[_offset]);

    // Substracting size of len field from result
    _len -= lenSize;

    _offset += lenSize;
    _parseStage = 2;
}

void Message::parseData(void) {
    _offset += _len;
    _data = _data.substr(0, _offset);

    _parseStage = 3;
}

// Returns true if parsing succeeded, false otherwise
bool Message::parse(void) {

    if (_data.size() == 0) {
        return false;
    }
    
    if (_parseStage == 0) {
        parseId();
    }

    if (_data.size() < static_cast<std::size_t>(_offset + lenSize)) {
        return false;
    }

    if (_parseStage == 1) {
        parseLen();
    }

    if (_data.size() < static_cast<std::size_t>(_offset + _len)) {
        return false;
    }
    
    if (_parseStage == 2) {
        parseData();
    }

    return true;
}

std::ostream& operator<<(std::ostream &out, const Message &msg) {
    if (msg.getId() != 0) {
        out << "id: " << msg.getId() << ", ";
    }

    out << "len: " << msg.getLen() << ", ";
    out << "data: " << msg.getData();
    
    return out;
}