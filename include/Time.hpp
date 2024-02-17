#pragma once

#include <chrono>
#include <string>

namespace Time {
    extern const char * format;

    std::string now(const char *fmt = Time::format);
}