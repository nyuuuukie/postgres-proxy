#include "Message.hpp"
#include "Log.hpp"

Message::Message(const std::string& s) : _data(s), _id(0), _len(0), _offset(0), _parseStage(0) {
}

Message::Message(const char* s) : _data(s), _id(0), _len(0), _offset(0), _parseStage(0) {
}

int Message::size(void) const {
    return _offset;
}

bool Message::ready(void) const {
    return _parseStage == 3;
}

void Message::addData(const std::string& data) {
    _data += data;
}

const std::string& Message::getData(void) const {
    return _data;
}

char Message::getId(void) const {
    return _id;
}

int Message::getLen(void) const {
    return _len;
}

void Message::parseId(void) {
    if (frontIds.find(_data[0]) != std::string::npos || backIds.find(_data[0]) != std::string::npos) {
        _id = _data[0];
        _offset++;
    }
    _parseStage = 1;
}

void Message::parseLen(void) {
    const int o1 = static_cast<int>(_data[_offset + 3]) << 24;
    const int o2 = static_cast<int>(_data[_offset + 2]) << 16;
    const int o3 = static_cast<int>(_data[_offset + 1]) << 8;
    const int o4 = static_cast<int>(_data[_offset + 0]) << 0;
    _len = ntohl(o1 | o2 | o3 | o4);

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

    // For first SSL response
    if (_id == 'N') {
        Log.debug() << "SSL response" << Log.endl;
        if (_data.size() == 1) {
            _parseStage = 3;
            return true;
        }
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

std::ostream& operator<<(std::ostream& out, const Message& msg) {
    std::size_t offset = 0;

    if (msg.getId() != 0) {
        out << msg.getId();
        offset++;
    } else {
        out << " ";
    }
    out << " ";

    const std::size_t size = msg.getLen() + lenSize;
    if (size != 0) {
        out << std::setw(3) << std::right << size;
        offset += lenSize;
    }
    out << " ";

    const std::string& data = msg.getData();
    for (std::size_t i = offset; i < data.size(); ++i) {
        if (isprint(data[i])) { //&& (i == offset || isprint(data[i - 1]) || !isprint(data[i - 1]))) {
            out << data[i];
        } else {
            out << "[" << (int)(unsigned char)data[i] << "]";
        }
    }

    return out;
}