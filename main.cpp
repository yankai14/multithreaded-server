//
// Created by Yan Kai Lim on 3/12/23.
//

#include "transport/BaseSocket.h"


int main() {
    Transport::BaseSocket baseSocket(8080, Transport::TCP);
    baseSocket.initSocket();
}