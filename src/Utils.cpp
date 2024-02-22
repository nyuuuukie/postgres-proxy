#include "Utils.hpp"

int resolveHostname(const std::string &host, struct sockaddr_in *resAddr) {

    addrinfo *lst = nullptr;
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int res = ::getaddrinfo(host.c_str(), nullptr, &hints, &lst);

    if (res == 0) {
        *resAddr = *(sockaddr_in*)(lst->ai_addr);
    }

    if (lst != nullptr) {
        ::freeaddrinfo(lst);
    }

    return (res == 0) ? 0 : -1;
}
