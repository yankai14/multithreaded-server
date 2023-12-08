//
// Created by Yan Kai Lim on 3/12/23.
//

#ifndef MULTITHREADED_SERVER_BASESOCKET_H
#define MULTITHREADED_SERVER_BASESOCKET_H

#include "../ThreadPool/ThreadPool.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <unistd.h>
#include <sys/event.h>
#include <sstream>
#include <csignal>

namespace Transport {
    enum TransportProtocol {UDP, TCP};
    const int MAX_EVENTS = 1000;
    extern volatile sig_atomic_t g_stopSignal;

    class BaseSocket {
    private:
        int port = 8080;
        TransportProtocol protocol;

    public:
        int serverSocketFd = -1;
        BaseSocket(int port, TransportProtocol protocol);

        int initSocket();
        void cleanup();

        ~BaseSocket();
    };

} // Transport

#endif //MULTITHREADED_SERVER_BASESOCKET_H
