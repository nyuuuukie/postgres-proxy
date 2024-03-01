#pragma once

class Client;

struct Event {
    enum class Type { NONE, PARSE_REQUEST, PARSE_RESPONSE };

    Client* client;
    Type type;

    Event(void) : client(nullptr), type(Type::NONE) {
    }

    Event(Client* client, Type type) : client(client), type(type) {
    }

    bool isOperative(void) const {
        return type != Type::NONE && client != nullptr;
    }
};