#pragma once

class Client;

struct Event {

    enum class Type {
        NONE,
        READ_REQUEST,
        READ_RESPONSE,
        PASS_REQUEST,
        PASS_RESPONSE
    };


    Client *client;
    Type type;

    Event(void) : client(nullptr), type(Type::NONE) {}
    Event(Client *client, Type type) : client(client), type(type) {}

    bool isOperative(void) const { return type != Type::NONE && client != nullptr; }
};