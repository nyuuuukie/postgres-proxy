#pragma once

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <mutex>

#include "Time.hpp"

class Logger : private std::streambuf, public std::ostream {
public:
    enum class Levels { LOG_CRIT, LOG_ERROR, LOG_INFO, LOG_DEBUG, LOG_NOSPEC };

private:
    bool _fileLogs;
    bool _stdoutLogs;

    std::string _logfile;
    std::string _logDir;
    std::string _logFilePrefix;

    Levels _curLevel;
    Levels _askLevel;

    // Each logger has its own mutex to prevent
    // mixing between multiple threads output
    std::mutex _m_lock_print;

    // For file logging
    std::ofstream _out;

public:
    Logger(void);
    ~Logger(void);

    Logger& info(void);
    Logger& debug(void);
    Logger& error(void);
    Logger& crit(void);
    Logger& print(Levels = Levels::LOG_NOSPEC);

    void fileLogs(bool);
    void stdoutLogs(bool);

    void setLevel(int level);
    void setLevel(Levels level);
    void setLogDir(const std::string&);
    void setFilePrefix(const std::string&);

    Logger& operator<<(std::ostream& (*func)(std::ostream&));

    template <typename T>
    Logger& operator<<(const T& val) {
        if (_askLevel <= _curLevel) {
            if (_fileLogs && _out.good()) {
                _out << val;
            }

            if (_stdoutLogs) {
                if (_askLevel <= Levels::LOG_ERROR) {
                    std::cerr << val;
                } else {
                    std::cout << val;
                }
            }
        }
        return *this;
    }

    static std::ostream& endl(std::ostream& out);
    static std::ostream& ends(std::ostream& out);
};

extern Logger Log;
extern Logger queryLog;