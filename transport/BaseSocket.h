//
// Created by Yan Kai Lim on 3/12/23.
//

#ifndef MULTITHREADED_SERVER_BASESOCKET_H
#define MULTITHREADED_SERVER_BASESOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <unistd.h>
#include <sys/event.h>

namespace Transport {
    enum TransportProtocol {UDP, TCP};
    const int MAX_EVENTS = 1000;

    class BaseSocket {
    private:
        int port = 8080;
        TransportProtocol protocol;
        int serverSocketFd = -1;

        int kq = -1;
        struct kevent events[MAX_EVENTS]{};
    public:
        BaseSocket(int port, TransportProtocol protocol);

        int initSocket();

        int initKq();
        int registerServerFdToKq() const;
        int handleAcceptEvent() const;
        int handleReadEvent(int clientFd) const;
        int listenToEvent();

        void cleanup() const;
        ~BaseSocket();
    };

} // Transport

#endif //MULTITHREADED_SERVER_BASESOCKET_H
