#include "Message.hpp"
#include "Log.hpp"

Message::Message(void) : _id(0), _len(0), _dataLen(0), _offset(0), _parseStage(Stages::ID) {
}

Message::~Message(void) {
}

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

inline std::size_t Message::idLen(void) const {
    return (_id != 0) ? 1 : 0;
}

inline std::size_t Message::totalLen(void) const {
    return _len + idLen();
}

inline std::size_t Message::headerLen(void) const {
    return lenFieldSize + idLen();
}

std::size_t Message::parseId(const std::string &data, std::size_t dataSize, std::size_t pos) {

    if (frontIds.find(data[pos]) != std::string::npos || \
         backIds.find(data[pos]) != std::string::npos) {
        _id = data[pos];
    }
    _parseStage = Stages::LENGTH;

    // For first SSL response
    if (_id == 'N' && dataSize == 1) {
        Log.debug() << "SSL response" << Log.endl;
        _parseStage = Stages::DONE;
    }

    return idLen();
}

inline std::size_t Message::parseLen(const std::string &data, std::size_t dataSize, std::size_t pos) {

    std::size_t i = pos;
    for (; i < pos + dataSize && _offset < 32; ++i) {
        const std::size_t c = static_cast<std::size_t>(data[i]);
        _len |= (c << _offset);
        _offset += 8;
    }

    if (_offset == 32) {
        _len = ntohl(_len);
        _parseStage = Stages::DATA;
    }

    return i - pos;
}

inline std::size_t Message::parseData(const std::string &data, std::size_t dataSize, std::size_t pos) {
    (void)data;

    if (_dataLen + dataSize >= totalLen()) {
        dataSize = totalLen() - _dataLen - pos;

        _parseStage = Stages::DONE;
    }

    return dataSize;
}

// Parses a message and returns amount of bytes that were processed 
std::size_t Message::parse(const std::string& newData, std::size_t dataSize, std::size_t pos) {
    
    if (dataSize == 0) {
        return 0;
    }

    std::size_t bytes = 0;

    if (_parseStage == Stages::ID) {
        bytes += parseId(newData, dataSize, pos + bytes);
    }

    if (_parseStage == Stages::LENGTH) {
        bytes += parseLen(newData, dataSize, pos + bytes);
    }

    if (_parseStage == Stages::DATA) {
        bytes += parseData(newData, dataSize, bytes);
    }

    _dataLen += bytes;
    _data.append(newData, pos, bytes);

    return bytes;
}


void Message::log(void) const {
    if (Args::logAllMessages) {
        queryLog.print() << *this << queryLog.endl;
    } else {
        // Print query SQL requests
        if (_id == 'Q') {
            queryLog.print() << &_data[5] << queryLog.endl;
        }
    }
}

// Formatted output for whole message
std::ostream& operator<<(std::ostream& out, const Message& msg) {
    std::size_t offset = 0;

    const char id = msg.getId();
    const size_t len = msg.getLen();
    const std::string& data = msg.getData();

    // Print message identifier
    out << std::setw(1) << (id ? id : ' ') << ' ';
    offset += (id ? 1 : 0);

    // Print message len
    out << std::setw(3) << (len ? len : ' ') << ' ';
    offset += len ? lenFieldSize : 0;

    // Print message data
    for (std::size_t i = offset; i < data.size(); ++i) {
        if (isprint(data[i])) {
            out << data[i];
        } else {
            out << "[" << (int)(unsigned char)data[i] << "]";
        }
    }

    return out;
}