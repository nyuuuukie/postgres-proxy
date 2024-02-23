#include "Args.hpp"


// Constants could be moved to a separate file
const int proxyPortDefault = 8080;
const int loglvlDefault = 2;
const std::string proxyHostDefault = "127.0.0.1";
const std::string logdirDefault = "logs";

const int targetPortDefault = 5432;
const std::string targetHostDefault = "127.0.0.1";

int Args::targetPort = targetPortDefault;
int Args::loglvl = loglvlDefault;
std::string Args::targetHost = targetHostDefault;
std::string Args::logdir = logdirDefault;

std::string Args::proxyHost = proxyHostDefault;
int Args::proxyPort = proxyPortDefault;

int Args::backlog = SOMAXCONN;
int Args::workersCount = 3;

// These functions won't be called outside of these file
// so there's no need to place their prototypes in the header
std::string parseHost(const std::string &arg, const std::string &def);
std::string parseDir(const std::string &arg, const std::string &def);
int         parseLogLevel(const std::string &arg, int def);
int         parsePort(const std::string &arg, int def);

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
               if (args[i] == "-th" || args[i] == "--target-host") {
            // For now host parameter could only contain ipv4 address
            // This could be improved to be able to store hostnames and ipv6 addresses
            Args::targetHost = parseHost(args[++i], targetHostDefault);
        } else if (args[i] == "-h" || args[i] == "--host") {
            Args::proxyHost = parseHost(args[++i], proxyHostDefault);
        }else if (args[i] == "-tp" || args[i] == "--target-port") {
            Args::targetPort = parsePort(args[++i], targetPortDefault);
        } else if (args[i] == "-p" || args[i] == "--port") {
            Args::proxyPort = parsePort(args[++i], proxyPortDefault);
        } else if (args[i] == "-d" || args[i] == "--logdir") {
            Args::logdir = parseDir(args[++i], logdirDefault);
        } else if (args[i] == "-l" || args[i] == "--loglvl") {
            Args::loglvl = parseLogLevel(args[++i], loglvlDefault);
        }
    }

    return 0;
}

bool isValidIPv4(const std::string &ipv4) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipv4.c_str(), &(sa.sin_addr));
    return result != 0;
}

std::string parseHost(const std::string &arg, const std::string &def) {
    if (isValidIPv4(arg)) {
        return arg;
    } else {
        Log.error() << arg << ": invalid host(ipv4)" << Log.endl;
        Log.error() << "Default value will be used: " << def << Log.endl;
        
        return def;
    }
}

int parsePort(const std::string &arg, int def) {

    int parsedNum;
    
    try {
        parsedNum = stoi(arg);
    } catch (std::exception &e) {
        Log.error() << arg << ": invalid port number" << Log.endl;
        Log.error() << "Default value will be used: " << def << Log.endl;

        return def;
    }

    if (parsedNum > 1024 && parsedNum < 65535) {
        return parsedNum;
    } else {
        Log.error() << parsedNum << ": invalid port number" << Log.endl;
        Log.error() << "Default value will be used: " << def << Log.endl;

        return def;
    }

    
}

std::string parseDir(const std::string &arg, const std::string &def) {
    namespace fs = std::filesystem;

    fs::path path = arg;

    if (fs::is_directory(path)) {
        return arg;
    } else {
        Log.error() << arg << " does not exist or not a directory" << Log.endl;
        Log.error() << "Default value will be used: " << def << Log.endl;

        return def;
    }
}

int parseLogLevel(const std::string &arg, int def) {
    
    int parsedLvl;

    try {
        parsedLvl = stoi(arg);
    } catch (std::exception &e) {
        Log.error() << arg << ": invalid level number" << Log.endl;
        Log.error() << "Default value will be used: " << def << Log.endl;
        
        return def;
    }

    if (parsedLvl >= 0 && parsedLvl < 4) {
        return parsedLvl;
    } else {
        Log.error() << parsedLvl << ": invalid level number" << Log.endl;
        Log.error() << "Default value will be used: " << def << Log.endl;

        return def;
    }


}

