#pragma once

#include "Client.hpp"

struct Event {

    enum class Type {
        READ_REQUEST,
        READ_RESPONSE,
        PASS_REQUEST,
        PASS_RESPONSE
    };


    Client *client;
    Type type;
};