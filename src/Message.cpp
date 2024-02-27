#include "Message.hpp"
#include "Log.hpp"

Message::Message(void) : _id(0), _len(0), _dataLen(0), _parseStage(Stages::ID) {
}

Message::~Message(void) {}

// std::size_t Message::size(void) const {
//     return _dataLen;
// }

bool Message::ready(void) const {
    return _parseStage == Stages::DONE;
}

const std::string& Message::getData(void) const {
    return _data;
}

char Message::getId(void) const {
    return _id;
}

std::size_t Message::getLen(void) const {
    return _len;
}

std::size_t Message::totalLen(void) const {
    return _len + ((_id != 0) ? 1 : 0);
}

void Message::parseId(void) {
    if (frontIds.find(_data[0]) != std::string::npos || backIds.find(_data[0]) != std::string::npos) {
        _id = _data[0];
        _dataLen++;
    }
    _parseStage = Stages::LENGTH;

    // For first SSL response
    if (_id == 'N' && _data.size() == 1) {
        Log.debug() << "SSL response" << Log.endl;
        _parseStage = Stages::DONE;
    }
}

void Message::parseLen(void) {
    const int i = _dataLen;
    const int oct1 = static_cast<int>(_data[i + 3]) << 24;
    const int oct2 = static_cast<int>(_data[i + 2]) << 16;
    const int oct3 = static_cast<int>(_data[i + 1]) << 8;
    const int oct4 = static_cast<int>(_data[i]);
    _len = ntohl(oct1 | oct2 | oct3 | oct4);

    _dataLen += lenFieldSize;

    _parseStage = Stages::DATA;
}

void Message::parseData(void) {
    _dataLen += _len - lenFieldSize;
    _data = _data.substr(0, _dataLen);

    _parseStage = Stages::DONE;
}

// Returns amount of parsed bytes 
std::size_t Message::parse(const std::string &newData) {

    if (newData.size() == 0) {
        return 0;
    }

    _data += newData;
    std::size_t bytes = newData.size();

    if (_parseStage == Stages::ID) {
        parseId();
    }

    if (_parseStage == Stages::LENGTH) {
        if (_data.size() >= _dataLen + lenFieldSize) {
            parseLen();
        }
    }

    if (_parseStage == Stages::DATA) {
        if (_data.size() >= totalLen()) {
            bytes -= _data.size() - totalLen();
            parseData();
        }
    }

    return bytes;
}


// Formatted output for whole message 
std::ostream& operator<<(std::ostream& out, const Message& msg) {
    std::size_t offset = 0;

    // Print message identifier
    if (msg.getId() != 0) {
        out << msg.getId();
        offset++;
    } else {
        out << " ";
    }
    out << " ";

    // Print message len
    const std::size_t size = msg.getLen();
    if (size != 0) {
        out << std::setw(3) << std::right << size;
        offset += lenFieldSize;
    }
    out << " ";

    // Print message data
    const std::string& data = msg.getData();
    for (std::size_t i = offset; i < data.size(); ++i) {
        if (isprint(data[i])) { // && (i == offset || isprint(data[i - 1]) || !isprint(data[i - 1]))) {
            out << data[i];
        } else {
            out << "[" << (int)(unsigned char)data[i] << "]";
        }
    }

    return out;
}