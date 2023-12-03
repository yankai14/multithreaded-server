//
// Created by Yan Kai Lim on 3/12/23.
//

#include "BaseSocket.h"

namespace Transport {
    BaseSocket::BaseSocket(int port, TransportProtocol protocol): port(port), protocol(protocol) {}

    int BaseSocket::initSocket() {
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        int serverSocket = -1;
        if (protocol == TransportProtocol::UDP) {
            serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        } else if (protocol == TransportProtocol::TCP) {
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        }

        if (serverSocket == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return -1;
        }

        if (bind(serverSocket, (struct sockaddr*)&(serverAddress), sizeof(serverAddress)) == -1) {
            std::cerr << "Error binding socket" << std::endl;
            close(serverSocket);
            return -1;
        }

        if (listen(serverSocket, 10) == -1) {
            std::cerr << "Error binding socket" << std::endl;
            close(serverSocket);
            return -1;
        }

        std::cout << "Listening to " << port << std::endl;

        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            close(serverSocket);
            return -1;
        }

        char buffer[2048];
        while (recv(clientSocket, buffer, static_cast<size_t >(sizeof(buffer)), 0) > 0) {
            std::cout << "Received: " << buffer << std::endl;
        }

        close(serverSocket);

        return 1;
    }

} // Transport