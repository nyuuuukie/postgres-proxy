#include "Log.hpp"

Logger Log;
Logger queryLog;

static const std::vector<std::string> titles {
    "CRIT", "ERROR", "INFO", "DEBUG", ""
};

Logger::Logger() : std::ostream(this),
    _fileLogs(false),
    _stdoutLogs(true),
    _logfile(""),
    _logDir(""),
    _curLevel(Levels::LOG_NOSPEC),
    _askLevel(Levels::LOG_NOSPEC) {
}

Logger::~Logger() {}

void
Logger::setLogDir(const std::string &dir) {
    _logDir = dir;
}

void
Logger::stdoutLogs(bool flag) {
    _stdoutLogs = flag;
}

void
Logger::fileLogs(bool flag) {

    if (_fileLogs == flag) {
        return ;
    }

    _fileLogs = flag;
    if (_fileLogs) {
        // Constants could be moved to another file with other setings
        _logfile = _logDir + Time::now("%d-%m-%Y_%H-%M-%S") + ".log";

        _out.open(_logfile, std::ios_base::out | std::ios_base::trunc);

        if (!_out.good()) {
            this->error() << "Cannot open/create logfile " << _logfile << Log.endl;
        } else {
            this->info() << "Logging into " << _logfile << Log.endl;
        }
    } else {
        _out.close();
        _out.clear();
    }
}

void
Logger::setLevel(Levels level) {
    _curLevel = level;
}

Logger &
Logger::print(Levels level) {
    _m_lock_print.lock();
    _askLevel = level;

    *this << Time::now() << " ";
    const int idx = static_cast<int>(_askLevel);
    if (!titles[idx].empty()) {
        *this << titles[idx] << " ";
    }

    return *this;
}



Logger &
Logger::debug(void) {
    return print(Levels::LOG_DEBUG);
}

Logger &
Logger::error(void) {
    return print(Levels::LOG_ERROR);
}

Logger &
Logger::crit(void) {
    print(Levels::LOG_CRIT) << "errno [" << errno << "]: " << strerror(errno) << Log.endl;
    return print(Levels::LOG_CRIT);
}

Logger &
Logger::info(void) {
    return print(Levels::LOG_INFO);
}

Logger &
Logger::operator<<(std::ostream& (*func)(std::ostream &)) {
    if (_askLevel <= _curLevel) {
        if (_fileLogs && _out.good()) {
            func(_out);
        }

        if (_askLevel <= Levels::LOG_ERROR) {
            func(std::cerr);
        } else {
            func(std::cout);
        }

        _askLevel = Levels::LOG_NOSPEC;
    }
    _m_lock_print.unlock();

    return *this;
}

std::ostream&
Logger::endl(std::ostream& out) {
    out << std::endl;
    return out;
}

std::ostream&
Logger::ends(std::ostream& out) {
    out << std::ends;
    return out;
}