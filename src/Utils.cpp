#include "Utils.hpp"

int resolveHostname(const std::string &host, struct sockaddr_in *resAddr) {

    addrinfo *lst = NULL;
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int res = ::getaddrinfo(host.c_str(), NULL, &hints, &lst);

    if (res == 0) {
        *resAddr = *(sockaddr_in*)(lst->ai_addr);
    }

    if (lst != NULL) {
        ::freeaddrinfo(lst);
    }

    return (res == 0) ? 0 : -1;
}
