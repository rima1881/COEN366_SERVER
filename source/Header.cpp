//
// Created by Amdgh on 10/24/2025.
//

#include "../header/Header.h"

std::string Header::generateHeader(std::uint32_t payloadSize) {

    // Create a 4-byte header and copy the length value
    char header[4];
    header[0] = static_cast<uint8_t>(payloadSize & 0xFF);        // lowest byte
    header[1] = static_cast<uint8_t>((payloadSize >> 8) & 0xFF);
    header[2] = static_cast<uint8_t>((payloadSize >> 16) & 0xFF);
    header[3] = static_cast<uint8_t>((payloadSize >> 24) & 0xFF); // highest byte
    std::string headerString(header, 4);
    return headerString;
}


std::uint32_t Header::extractHeader(std::string header) {

}