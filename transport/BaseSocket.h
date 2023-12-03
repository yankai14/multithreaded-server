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

namespace Transport {
    enum TransportProtocol {UDP, TCP};

    class BaseSocket {
    private:
        int port = 8080;
        TransportProtocol protocol;

    public:
        BaseSocket(int port, TransportProtocol protocol);
        int initSocket();
    };

} // Transport

#endif //MULTITHREADED_SERVER_BASESOCKET_H
