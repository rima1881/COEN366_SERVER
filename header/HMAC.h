//
// Created by Amdgh on 10/24/2025.
//


#ifndef COEN366_NETWORKPROTOCOL_HMAC_H
#define COEN366_NETWORKPROTOCOL_HMAC_H
#include <string>

namespace HMAC {
    std::string generateHMAC(std::string payload, std::string key);
    bool verifyHMAC(std::string payload, std::string hash, std::string key);
}

#endif //COEN366_NETWORKPROTOCOL_HMAC_H