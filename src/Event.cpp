#include "Event.hpp"

Event::Event(void) : client(nullptr), type(Type::NONE) {
}

Event::Event(Client* client, Type type) : client(client), type(type) {
}

bool Event::isOperative(void) const {
    return type != Type::NONE && client != nullptr;
}

void Event::handle(void) {
    if (type == Event::Type::PARSE_REQUEST) {
        client->parseRequest();
    } else if (type == Event::Type::PARSE_RESPONSE) {
        client->parseResponse();
    } else {
        Log.error() << "NONE event handled" << Log.endl;
    }

    client->processing = false;
}