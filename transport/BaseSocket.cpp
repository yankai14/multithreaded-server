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

        if (protocol == TransportProtocol::UDP) {
            serverSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
        } else if (protocol == TransportProtocol::TCP) {
            serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        }

        if (serverSocketFd == -1) {
            std::cerr << "Error creating socket" << std::endl;
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

    int BaseSocket::initKq() {
        kq = kqueue();
        if (kq == -1) {
            std::cerr << "Error creating kqueue" << std::endl;
            return -1;
        }
        return kq;
    }

    int BaseSocket::registerServerFdToKq() const {
        struct kevent event{};
        EV_SET(&event, serverSocketFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
            std::cerr << "Error registering server socket" << std::endl;
            return -1;
        }

        return 1;
    }

    int BaseSocket::handleAcceptEvent() const {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(serverSocketFd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            std::cerr << "Unable to accept client connection" << std::endl;
        }

        std::cout << "Connection with client " << client_fd << " has been established" << std::endl;
        // Register the client socket for read events with kqueue
        struct kevent event{};
        EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
            std::cerr << "Error registering client socket" << std::endl;
            return -1;
        }

        return 1;
    }

    int BaseSocket::handleReadEvent(int clientFd) const {
        char buffer[2048];
        int bytesReceived = recv(clientFd, buffer, static_cast<size_t>(sizeof(buffer)), 0);
        if (bytesReceived <= 0) {
            std::cout << "Client " << clientFd << " disconnected, cleaning up...." << std::endl;
            struct kevent event{};
            EV_SET(&event, clientFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(kq, &event, 1, nullptr, 0, nullptr);
            close(clientFd);
        } else {
            std::cout << "Received: " << buffer << std::endl;
        }
        return 1;
    }

    int BaseSocket::listenToEvent() {
        while (true) {
            int num_events = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
            if (num_events == -1) {
                std::cerr << "kevent wait failed" << std::endl;
                return -1;
            }

            for (int i=0; i<num_events; i++) {
                int fd = static_cast<int>(events[i].ident);
                if (fd == serverSocketFd) handleAcceptEvent();
                else handleReadEvent(fd);
            }
        }
    }

    BaseSocket::~BaseSocket() {
        cleanup();
    }

    void BaseSocket::cleanup() const {
        std::cout << "Cleaning up BaseSocket" << std::endl;
        if (serverSocketFd != -1) {
            std::cout << "Closing server socket" << std::endl;
            close(serverSocketFd);
        }
        if (kq != -1) {
            std::cout << "Closing kq" << std::endl;
            close(kq);
        }
    }
} // Transport