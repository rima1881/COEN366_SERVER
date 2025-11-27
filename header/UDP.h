//
// Created by Amdgh on 11/26/2025.
//

#ifndef COEN366_SERVER_UDP_H
#define COEN366_SERVER_UDP_H

//
// Created by Amdgh on 10/24/2025.
//

#ifndef COEN366_NETWORKPROTOCOL_UDP_H
#define COEN366_NETWORKPROTOCOL_UDP_H
#include <string>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace UDP {
    std::string sendMessage(const std::string &address, int port, std::string msg, const std::string &secretKey);
    void setupListener(SOCKET socket, const std::string& secretKey, const std::function<std::string(std::string)> &);
    SOCKET setupSocket(int port);
}

#endif //COEN366_NETWORKPROTOCOL_UDP_H

#endif //COEN366_SERVER_UDP_H