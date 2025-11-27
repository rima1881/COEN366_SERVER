//
// Created by Amdgh on 11/26/2025.
//

#include "../header/Server.h"
#include <cstdint>
#include "../header/Globals.h"
#include  <string>
#include  <thread>
#include <sstream>
#include <utility>
#include <iostream>
#include "../header/UDP.h"
#include <vector>
#include <mutex>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")


Server::Server(std::string secretKey) : secretKey(std::move(secretKey)){

    this -> sockfd = UDP::setupSocket(SERVER_UDP_PORT);

    std::thread handlerThread(UDP::setupListener, std::ref(sockfd), std::ref(secretKey), [this](std::string payload) {
        return this->routeRequest(std::move(payload));
    });
    handlerThread.detach();

    std::thread heartBeatThread([this]() {
        while (true) {
            this -> heartbeat();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });

    heartBeatThread.detach();
}


// It can be built much nicer but I don't have time to do it
void Server::heartbeat() {
    std::unique_lock<std::shared_timed_mutex> lock(nodesMutex);

    std::vector<int> removeClientIndexes;

    for (int i = 0; i < clients.size(); i++) {
        const std::string heartbeatRequest(1, static_cast<char>(HEARTBEAT_REQUEST));
        bool failed = false;

        try {
            std::string response = UDP::sendMessage(
                clients[i].info.address,
                clients[i].info.udpPort,
                heartbeatRequest,
                this->secretKey);

            const std::string expectedResponse(1, static_cast<char>(HEARTBEAT_RESPONSE));

            if (response != expectedResponse) {
                failed = true;
            }
        }
        catch (...) {
            failed = true;
        }

        if (failed) {
            std::cout << "Node " << clients[i].info.name << " failed Heartbeat" << std::endl;
            removeClientIndexes.push_back(i);
        }
    }

    // Remove failed clients in reverse order to keep indexes valid
    for (auto it = removeClientIndexes.rbegin(); it != removeClientIndexes.rend(); ++it) {
        int idx = *it;
        clientMap.erase(clients[idx].info.name); // remove from map
        clients.erase(clients.begin() + idx);    // remove from deque
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<int> removeStorageIndexes;

    for (int i = 0; i < storages.size(); i++) {
        const std::string heartbeatRequest(1, static_cast<char>(HEARTBEAT_REQUEST));
        bool failed = false;

        try {
            std::string response = UDP::sendMessage(
                storages[i].info.address,
                storages[i].info.udpPort,
                heartbeatRequest,
                this->secretKey);

            const std::string expectedResponse(1, static_cast<char>(HEARTBEAT_RESPONSE));

            if (response != expectedResponse) {
                failed = true;
            }
        }
        catch (...) {
            failed = true;
        }

        if (failed) {
            std::cout << "Node " << storages[i].info.name << " failed Heartbeat" << std::endl;
            removeStorageIndexes.push_back(i);
        }
    }

    // Remove failed clients in reverse order to keep indexes valid
    for (auto it = removeStorageIndexes.rbegin(); it != removeStorageIndexes.rend(); ++it) {
        int idx = *it;
        storageMap.erase(storages[idx].info.name); // remove from map
        storages.erase(storages.begin() + idx);    // remove from deque
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<int> removeClientStorageIndexes;

    for (int i = 0; i < clientStorages.size(); i++) {
        const std::string heartbeatRequest(1, static_cast<char>(HEARTBEAT_REQUEST));
        bool failed = false;

        try {
            std::string response = UDP::sendMessage(
                clientStorages[i].info.address,
                clientStorages[i].info.udpPort,
                heartbeatRequest,
                this->secretKey);

            const std::string expectedResponse(1, static_cast<char>(HEARTBEAT_RESPONSE));

            if (response != expectedResponse) {
                failed = true;
            }
        }
        catch (...) {
            failed = true;
        }

        if (failed) {
            std::cout << "Node " << clientStorages[i].info.name << " failed Heartbeat" << std::endl;
            removeClientStorageIndexes.push_back(i);
        }
    }

    // Remove failed clients in reverse order to keep indexes valid
    for (auto it = removeClientStorageIndexes.rbegin(); it != removeClientStorageIndexes.rend(); ++it) {
        int idx = *it;
        clientStorageMap.erase(clientStorages[idx].info.name); // remove from map
        clientStorages.erase(clientStorages.begin() + idx);    // remove from deque
    }




}

std::string Server::routeRequest(std::string payload) {

    const uint16_t command = static_cast<uint16_t>(payload[0]);
    payload.erase(0, 1);

    if (command == REGISTER_CODE) {
        return registerNode(payload);
    }
    if (command == DEREGISTER_CODE) {
        return deregisterNode(payload);
    }
    if (command == BACKUP_REQUEST_CODE) {
        return backupRequestHandle(payload);
    }


    std::string str(1, static_cast<char>(INVALID_REQUEST_CODE));
    return str;
}

std::string Server::registerNode(const std::string& payload) {

    std::istringstream iss(payload);
    std::vector<std::string> words;
    std::string word;

    while (iss >> word)
        words.push_back(word);

    if (words.size() != 6)
        return std::string(1, static_cast<char>(REGISTER_DENIED)); // todo add why

    try {

        const std::string& name = words[0];
        if (clientMap.count(name) || storageMap.count(name) || clientStorageMap.count(name)) {
            return std::string(1, static_cast<char>(REGISTER_DENIED)); // todo add why
        }

        const std::string& type = words[4];

        info nodeInfo{
            words[0],
            words[1],
            std::stoi(words[2]),
            std::stoi(words[3])
        };

        // Lock before touching shared structures
        std::unique_lock<std::shared_timed_mutex> lock(nodesMutex);

        if (type == "Client") {

            clients.emplace_back( client{
                std::vector<file>{},
                nodeInfo
            });
            clientMap[nodeInfo.name] = &clients.back();

        } else if (type == "Storage") {

            storages.emplace_back( storage{
                std::stoi(words[5]),
                std::vector<chunk>{},
                nodeInfo
            });
            storageMap[nodeInfo.name] = &storages.back();

        } else if (type == "ClientStorage") {

            clientStorages.emplace_back( clientStorage{
            std::stoi(words[5]),
            std::vector<file>{},
            nodeInfo
            });
            clientStorageMap[nodeInfo.name] = &clientStorages.back();

        } else {
            throw std::exception();
        }


    }
    catch (...) {
        return std::string(1, static_cast<char>(REGISTER_DENIED)); // TODO Add why
    }

    return std::string(1, static_cast<char>(REGISTERED_CLIENT));
}

std::string Server::deregisterNode(const std::string& name) {

    // Lock before reading or modifying shared containers
    std::unique_lock<std::shared_timed_mutex> lock(nodesMutex);

    // Remove from clients
    auto itClient = clientMap.find(name);
    if (itClient != clientMap.end()) {
        // Remove from deque
        auto& clientDeque = clients;
        clientDeque.erase(
            std::remove_if(clientDeque.begin(), clientDeque.end(),
                [&name](const client& c) { return c.info.name == name; }),
            clientDeque.end()
        );

        // Remove from map
        clientMap.erase(itClient);
        return std::string(1, static_cast<char>(DEREGISTERED_CLIENT));
    }

    // Remove from storages
    auto itStorage = storageMap.find(name);
    if (itStorage != storageMap.end()) {
        auto& storageDeque = storages;
        storageDeque.erase(
            std::remove_if(storageDeque.begin(), storageDeque.end(),
                [&name](const storage& s) { return s.info.name == name; }),
            storageDeque.end()
        );

        storageMap.erase(itStorage);
        return std::string(1, static_cast<char>(DEREGISTERED_CLIENT));
    }

    // Remove from clientStorage
    auto itCS = clientStorageMap.find(name);
    if (itCS != clientStorageMap.end()) {
        auto& csDeque = clientStorages;
        csDeque.erase(
            std::remove_if(csDeque.begin(), csDeque.end(),
                [&name](const clientStorage& cs) { return cs.info.name == name; }),
            csDeque.end()
        );

        clientStorageMap.erase(itCS);
        return std::string(1, static_cast<char>(DEREGISTERED_CLIENT));
    }

    // Not found
    return std::string(1, static_cast<char>(DEREGISTER_DENIED));    // Todo add why


}

std::string Server::backupRequestHandle(const std::string& payload) {

    std::istringstream iss(payload);
    std::vector<std::string> words;
    std::string word;

    while (iss >> word) {
        words.push_back(word);
    }

    if (words.size() != 3) {
        std::string str(1, static_cast<char>(BACKUP_DENIED)); //Todo Add why
        return str;
    }

    std::unique_lock<std::shared_timed_mutex> lock(nodesMutex);

    const std::string& fileName = words[0];
    const int fileSize = std::stoi(words[1]);
    if (fileSize > capacity) {
        return std::string(1, static_cast<char>(BACKUP_DENIED)); // Todo Add why
    }

    const int chunkSize = getChuckSize();
    int chunkCount = fileSize / chunkSize;
    if (fileSize % chunkSize != 0) {
        chunkCount++; // Include the last, smaller chunk
    }

    std::unordered_map<std::string, int> chunkStorageMap;

    for (int i = 0; i < storages.size() && chunkCount > 0 ; i++) {
        if (storages[i].capacity > chunkSize) {
            int storageChunks = storages[i].capacity / chunkSize;
            storageChunks = storageChunks > chunkCount ? chunkCount : storageChunks;
            chunkStorageMap[storages[i].info.name] = storageChunks;
            chunkCount -= storageChunks;
        }
    }

    for (int i=0; i < clientStorages.size() && chunkCount > 0 ; i++) {
        if (clientStorages[i].capacity > chunkSize) {
            int storageChunks = clientStorages[i].capacity / chunkSize;
            storageChunks = storageChunks > chunkCount ? chunkCount : storageChunks;
            chunkStorageMap[clientStorages[i].info.name] = storageChunks;
            chunkCount -= storageChunks;
        }
    }

    if (chunkCount > 0) {
        return std::string(1, static_cast<char>(BACKUP_DENIED));    // Todo Add why
    }

    std::string res = "";

    int chunkId = 0;

    std::vector<tempChunk> tempChunks;
    for (auto it = chunkStorageMap.begin(); it != chunkStorageMap.end(); ++it) {

        if (storageMap.count( it -> first)) {

            storage *st = storageMap[it->first];
            chunkCount += it -> second;
            res += st->info.name + ":" + st->info.address + ":" + std::to_string(st->info.tcpPort) + ":" + std::to_string(chunkCount) + ",";

            for (int k =it->second; k < chunkCount; k++) {

            }
            const tempChunk temp{

            };
            tempChunks.push_back();
        } else {

            auto *cst = clientStorageMap[it->first];
            chunkCount += it -> second;
            res += cst->info.name + ":" + cst->info.address + ":" + std::to_string(cst->info.tcpPort) + ":" + std::to_string(chunkCount) + ",";
        }
    }

    res.pop_back();



    std::thread backupThread([] {


    });

    return std::string(1,static_cast<char>(BACKUP_PLAN)) + fileName + " " + res + " " + std::to_string(chunkSize);
}


std::string Server::getRegisteredNodes() const {
    std::shared_lock<std::shared_timed_mutex> lock(nodesMutex);
    std::string result;
    return result;
}

// it is not perfect but i didn't have time
int Server::getChuckSize() const {
    // if (min_storage > 2024) {
    //     return min_storage;
    // }
    return 10;
}
