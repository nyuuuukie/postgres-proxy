#pragma once

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <filesystem>

#include "Log.hpp"

namespace Args {
    extern int port;
    extern int loglvl;
    extern std::string host;
    extern std::string logdir;


    extern int backlog;
    extern int workersCount;

    int parse(char **av);

};
