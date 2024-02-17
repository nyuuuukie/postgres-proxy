#include <Time.hpp>

const char *Time::format = "%F %T";

std::string Time::now(const char *fmt) {
    const auto now = std::chrono::system_clock::now();
    const time_t tp = std::chrono::system_clock::to_time_t(now);

    // Could be replaced with 'format' in C++20
    // std::format("{:%F %T}", std::chrono::system_clock::now());

    char buff[100];
    strftime(buff, sizeof(buff), fmt, std::localtime(&tp));

    return buff;
}
