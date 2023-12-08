//
// Created by Yan Kai Lim on 3/12/23.
//

#include "BaseSocket.h"

namespace Transport {
    volatile sig_atomic_t g_stopSignal = 0;

    BaseSocket::BaseSocket(int port, TransportProtocol protocol): port(port), protocol(protocol) {}

    int BaseSocket::initSocket() {
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        if (protocol == TransportProtocol::UDP) {
            serverSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
        } else if (protocol == TransportProtocol::TCP) {
            serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        }

        if (serverSocketFd == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return -1;
        }

        int reuse = 1;
        if (setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
            std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;
            return -1;
        }

        if (bind(serverSocketFd, (struct sockaddr*)&(serverAddress), sizeof(serverAddress)) == -1) {
            std::cerr << "Error binding socket" << std::endl;
            return -1;
        }

        if (listen(serverSocketFd, 10) == -1) {
            std::cerr << "Error binding socket" << std::endl;
            return -1;
        }

        std::cout << "Listening to " << port << std::endl;

        return 1;
    }


    BaseSocket::~BaseSocket() {
        cleanup();
    }


    void BaseSocket::cleanup() {
        if (serverSocketFd != -1) {
            std::cout << "Closing server socket" << std::endl;
            serverSocketFd = -1;
            close(serverSocketFd);
        }
    }
} // Transport