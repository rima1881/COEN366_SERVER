//
// Created by Amdgh on 10/24/2025.
//

#ifndef COEN366_NETWORKPROTOCOL_HEADER_H
#define COEN366_NETWORKPROTOCOL_HEADER_H

#include <cstdint>
#include <string>

namespace Header {
    std::string generateHeader(std::uint32_t payloadSize);
    std::uint32_t extractHeader(std::string header);
}

#endif //COEN366_NETWORKPROTOCOL_HEADER_H