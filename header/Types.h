//
// Created by Amdgh on 11/26/2025.
//

#ifndef COEN366_SERVER_FILE_H
#define COEN366_SERVER_FILE_H
#include <string>
#include <vector>

struct chunk {
    int id;
    int size;
};

struct tempChunk {  // just used for transmission before confirmation
    int id;
    bool hasConfirmed;
    std::string fileName;
};


struct file {
    std::string name;
    std::vector<chunk> chunks;
};

struct info {
    std::string name;
    std::string address;
    int udpPort;
    int tcpPort;
};

struct storage {
    int capacity;
    std::vector<chunk> chunks;
    info info;
};

struct clientStorage {
    int capacity;
    std::vector<file> files;
    info info;
};

struct client {
    std::vector<file> files;
    info info;
};


#endif //COEN366_SERVER_FILE_H