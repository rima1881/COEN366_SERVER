#include <iostream>
#include "header/Globals.h"
#include "header/Server.h"

void wsaInit() {
    // Winsock init
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        throw std::runtime_error("WSA initialization failed");
    }
}


int main() {


    wsaInit();
    Server s(SECRET_CODE);

    while (true) {}

    return 0;


}