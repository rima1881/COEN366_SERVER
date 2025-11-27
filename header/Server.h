//
// Created by Amdgh on 11/26/2025.
//

#ifndef COEN366_SERVER_SERVER_H
#define COEN366_SERVER_SERVER_H

#include <string>
#include <winsock2.h>
#include <shared_mutex>
#include <unordered_map>
#include <deque>
#include "../header/Types.h"

class Server {
public:

    explicit Server(std::string secretKey);
    std::string getRegisteredNodes() const;


private:
    int capacity;
    int min_storage;
    SOCKET sockfd;

    std::string secretKey;
    std::deque<client> clients;
    std::deque<storage> storages;
    std::deque<clientStorage> clientStorages;

    std::unordered_map<std::string, client*> clientMap;
    std::unordered_map<std::string, storage*> storageMap;
    std::unordered_map<std::string, clientStorage*> clientStorageMap;

    // Thread-safety
    mutable std::shared_timed_mutex nodesMutex;

    std::string routeRequest(std::string payload);
    std::string registerNode(const std::string& payload);
    std::string deregisterNode(const std::string& payload);
    std::string backupRequestHandle(const std::string& payload);

    int getChuckSize() const;

    void heartbeat();

};

#endif //COEN366_SERVER_SERVER_H