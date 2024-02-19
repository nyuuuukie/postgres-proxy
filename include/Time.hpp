#pragma once

#include <chrono>
#include <string>

using time_point = std::chrono::system_clock::time_point;

namespace Time {
    extern const char * format;

    std::string nowFmtString(const char *fmt = Time::format);
    time_point now(void);
}