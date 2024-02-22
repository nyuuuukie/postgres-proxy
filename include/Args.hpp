#pragma once

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <filesystem>

#include "Log.hpp"

namespace Args {
    extern int targetPort;
    extern int loglvl;
    extern std::string targetHost;
    extern std::string logdir;

    extern std::string proxyHost;
    extern int proxyPort;

    extern int backlog;
    extern int workersCount;

    int parse(char **av);

};
