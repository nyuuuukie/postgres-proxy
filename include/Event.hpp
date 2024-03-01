#pragma once

#include "Log.hpp"

class Client;

struct Event {
    enum class Type { NONE, PARSE_REQUEST, PARSE_RESPONSE };

    Client* client;
    Type type;

    Event(void);
    Event(Client* client, Type type);

    bool isOperative(void) const;
    void handle(void);
};

#include "Client.hpp"