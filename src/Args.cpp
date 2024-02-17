#include "Args.hpp"


const int portDefault = 8080;
const int loglvlDefault = 2;
const std::string hostDefault = "127.0.0.1";
const std::string logdirDefault = "log";

int Args::port = portDefault;
int Args::loglvl = loglvlDefault;
std::string Args::host = hostDefault;
std::string Args::logdir = logdirDefault;

int backlog = SOMAXCONN;

// These functions won't be called outside of these file
// so there's no need to place their prototypes in the header
void parseHost(const std::string &arg);
void parsePort(const std::string &arg);
void parseDir(const std::string &arg);
void parseLogLevel(const std::string &arg);

int Args::parse(char **av) {

    std::vector<std::string> args;
    for (int i = 1; av[i]; ++i) {
        args.push_back(av[i]);
    }

    // Type and value validations need to be added here
    // Default values will be set in case of missing\incorrect value
    // but a good solution would be to throw an error here.
    const int size = args.size() - 1;
    for (int i = 0; i < size; ++i) {
               if (args[i] == "-h" || args[i] == "--host") {
            // For now, host parameter could only contain ipv4 address
            // This could be improved to be able to store hostnames and ipv6 addresses
            parseHost(args[++i]);
        } else if (args[i] == "-p" || args[i] == "--port") {
            parsePort(args[++i]);
        } else if (args[i] == "-d" || args[i] == "--logdir") {
            parseDir(args[++i]);
        } else if (args[i] == "-l" || args[i] == "--loglvl") {
            parseLogLevel(args[++i]);
        }
    }

    return 0;
}

bool isValidIPv4(const std::string &ipv4) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipv4.c_str(), &(sa.sin_addr));
    return result != 0;
}

void parseHost(const std::string &arg) {
    if (isValidIPv4(arg)) {
        Args::host = arg;
    } else {
        Log.error() << arg << ": invalid host(ipv4)" << Log.endl;
        Log.error() << "Default value will be used: " << hostDefault << Log.endl;
    }
}

void parsePort(const std::string &arg) {

    int parsedNum;
    
    try {
        parsedNum = stoi(arg);
    } catch (std::exception &e) {
        Log.error() << arg << ": invalid port number" << Log.endl;
        Log.error() << "Default value will be used: " << portDefault << Log.endl;
    }

    if (parsedNum > 1024 && parsedNum < 65535) {
        Args::port = parsedNum;
    } else {
        Log.error() << parsedNum << ": invalid port number" << Log.endl;
        Log.error() << "Default value will be used: " << portDefault << Log.endl;
    }

}

void parseDir(const std::string &arg) {
    namespace fs = std::filesystem;

    fs::path path = arg;

    
    if (fs::is_directory(path)) {
        Args::logdir = arg;
    } else {
        Log.error() << arg << " does not exist or not a directory" << Log.endl;
        Log.error() << "Default value will be used: " << logdirDefault << Log.endl;
    }
}

void parseLogLevel(const std::string &arg) {
    
    int parsedLvl;

    try {
        parsedLvl = stoi(arg);
    } catch (std::exception &e) {
        Log.error() << arg << ": invalid level number" << Log.endl;
        Log.error() << "Default value will be used: " << loglvlDefault << Log.endl;
    }

    if (parsedLvl >= 0 && parsedLvl < 4) {
        Args::loglvl = parsedLvl;
    } else {
        Log.error() << parsedLvl << ": invalid level number" << Log.endl;
        Log.error() << "Default value will be used: " << loglvlDefault << Log.endl;
    }
}

