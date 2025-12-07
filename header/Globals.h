//
// Created by Amdgh on 10/12/2025.
//

#ifndef COEN366_NETWORKPROTOCOL_GLOBALS_H
#define COEN366_NETWORKPROTOCOL_GLOBALS_H

#define SECRET_CODE "meow"

// Server addresses
#define SERVER_IP_ADDRESS "127.0.0.1" // Localhost
#define SERVER_TCP_PORT 3000
#define SERVER_UDP_PORT 5000

#define UDP_BUF_SIZE    2048
#define TCP_BUF_SIZE    65536


// Message Structure
#define HEADER_SIZE 4u
#define HMAC_SIZE   32u

// Protocol Codes Client
#define REGISTER_CODE           0x00
#define DEREGISTER_CODE         0x01
#define BACKUP_REQUEST_CODE     0x02
#define STORAGE_TASK_ACK        0x03
#define HEARTBEAT_RESPONSE      0x07
#define CHUNK_OK                0x08
#define RECOVERY_REQUEST        0x09

// Protocol Codes Server
#define REGISTERED_CLIENT   0x60
#define DEREGISTERED_CLIENT 0x61
#define HEARTBEAT_REQUEST   0x62
#define STORAGE_TASK        0x63
#define BACKUP_PLAN         0x64
#define CHUNK_OK_ACK        0x65


// Error Codes
#define REGISTER_DENIED         0xFF
#define DEREGISTER_DENIED       0xFE
#define BACKUP_DENIED           0xFD
#define INVALID_REQUEST_CODE    0xFC
#define CHECKSUM_MISMATCH       0xFB
#define INVALID_PAYLOAD         0xFA



#endif //COEN366_NETWORKPROTOCOL_GLOBALS_H