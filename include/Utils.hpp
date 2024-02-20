#pragma once

#include <string>
#include <netdb.h>

// Resolves hostname by calling getaddrinfo and writes the result into sockaddr_in arg.
int resolveHostname(const std::string &host, struct sockaddr_in *resAddr);