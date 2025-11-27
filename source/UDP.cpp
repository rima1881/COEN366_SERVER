//
// Created by Amdgh on 10/24/2025.
//

#include <stdexcept>
#include "../header/UDP.h"
#include "../header/Globals.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include "../header/HMAC.h"
#include "../header/Header.h"
#include <iostream>
#include <ostream>

std::string UDP::sendMessage(const std::string &address, const int port ,std::string msg, const std::string &secretKey) {


    const auto bodyLen = static_cast<uint32_t>(msg.size());
    const std::string header = Header::generateHeader(bodyLen);
    const std::string hmac = HMAC::generateHMAC(msg, secretKey);
    msg = header + hmac + msg;

    const SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Todo
    // We have to do logging or something like that instead of printing
    if (sock == INVALID_SOCKET)
    {
        throw std::runtime_error("Could not create socket");
    }

    // Server endpoint
    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);

    // Todo
    // We have to do logging or something like that instead of printing
    if (inet_pton(AF_INET, address.c_str(), &srv.sin_addr) != 1)
    {
        closesocket(sock);
        throw std::runtime_error("Invalid IP address");
    }


    int sent = sendto(sock, msg.c_str(), static_cast<int>(msg.size()), 0, reinterpret_cast<sockaddr *>(&srv), sizeof(srv));

    // Todo
    // We have to do logging or something like that instead of printing
    // Check if sending failed
    if (sent == SOCKET_ERROR)
    {
        closesocket(sock);
        throw std::runtime_error("Could not send message");
    }

    // timeout
    DWORD rcvTimeoutMs = 2000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&rcvTimeoutMs), sizeof(rcvTimeoutMs));

    // Receive ack
    char buf[UDP_BUF_SIZE];
    sockaddr_in from{};
    int fromLen = sizeof(from);
    int recvd = recvfrom(sock, buf, UDP_BUF_SIZE - 1, 0, reinterpret_cast<sockaddr *>(&from), &fromLen);


    // Handles if successful or failed
    bool ok = false;
    // Todo
    // We have to do logging or something like that instead of printing
    // Check if sending failed
    if (recvd == SOCKET_ERROR)
    {
        throw std::runtime_error("Could not receive message");
    }

    closesocket(sock);

    buf[recvd] = '\0';
    std::string reply(buf);
    return buf;
}

SOCKET UDP::setupSocket(const int port) {

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed");
    }

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&(address)), sizeof(address)) == SOCKET_ERROR) {
        closesocket(sockfd);
        throw std::runtime_error("Bind failed");
    }

    return sockfd;
}

void UDP::setupListener(SOCKET sockfd, const std::string& secretKey, const std::function<std::string(std::string)> &handler) {

    sockaddr_in  clientAddr{};
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        // Getting header
        char buffer[UDP_BUF_SIZE];
        int bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrSize);

        if (bytesReceived == SOCKET_ERROR) {
            continue;
        }

        if (bytesReceived < HEADER_SIZE) {
            throw std::runtime_error("Receive header failed");
        }

        // Convert little endian bytes to 32 int
        uint32_t payloadSize =
            static_cast<uint8_t>(buffer[0]) |
            (static_cast<uint8_t>(buffer[1]) << 8) |
            (static_cast<uint8_t>(buffer[2]) << 16) |
            (static_cast<uint8_t>(buffer[3]) << 24);


        // TODO
        // Has to be changed to return error code instead of exception
        if (payloadSize != bytesReceived - HEADER_SIZE - HMAC_SIZE) {
            throw std::runtime_error("Receive payload failed");
        }

        std::string hmac(buffer + HEADER_SIZE, HMAC_SIZE);
        std::string payload(buffer + HEADER_SIZE + HMAC_SIZE, payloadSize);

        // TODO
        // Do st for fucked up hmac
        if (!HMAC::verifyHMAC(payload, hmac, secretKey)) {
            throw std::runtime_error("message is corrupted");
        }

        std::string responsePayload = handler(payload);

        const int bytesSent = sendto(
            sockfd,
            responsePayload.data(),
            responsePayload.size(),
            0,
            reinterpret_cast<const sockaddr*>(&clientAddr),
            clientAddrSize
        );

        if (bytesSent < 0) {
            perror("sendto failed");
        }

    }

}//
// Created by Amdgh on 11/26/2025.
//