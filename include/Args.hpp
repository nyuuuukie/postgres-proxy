#pragma once

#include <arpa/inet.h>
#include <filesystem>
#include <iostream>
#include <vector>

#include "Log.hpp"

// These namespace is used to store functions used to parse
// command line arguments as well as to store the results of parsing

namespace Args {
    extern int targetPort;
    extern int loglvl;
    extern std::string targetHost;
    extern std::string logdir;

    extern std::string proxyHost;
    extern int proxyPort;

    extern int backlog;
    extern int workersCount;

    extern bool logAllMessages;

    int parse(char** av);
    void usage(void);
};  // namespace Args
