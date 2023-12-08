//
// Created by Yan Kai Lim on 7/12/23.
//

#ifndef MULTITHREADED_SERVER_HTTP_H
#define MULTITHREADED_SERVER_HTTP_H

#include <string>
#include <unordered_map>
#include <BaseSocket.h>
#include <memory>

namespace Application {
    const int MAX_EVENTS = 1000;

    struct HttpRequest {
        std::string method;
        std::string path;
        std::string httpVersion;
        std::string host;
        std::string userAgent;
        std::string accept;
        std::string acceptLanguage;
        std::string acceptEncoding;
        std::string connection;
        std::string upgradeInsecureRequests;
        std::string secFetchDest;
        std::string secFetchMode;
        std::string secFetchSite;
        std::string secFetchUser;
    };

    class HTTP {
    private:
        std::unique_ptr<Transport::BaseSocket> serverSocket;
        std::unordered_map<std::string, std::string> headers;
        int kq = -1;
        struct kevent events[MAX_EVENTS]{};
        ThreadPool threadpool;
        std::unordered_map<std::string, std::function<std::string(HTTP*, HttpRequest&)>> router;

        static void handleSIGINT(int signal);

        static HttpRequest parse(std::istringstream iss);
        int initKq();
        int registerServerFdToKq() const;
        int handleAcceptEvent() const;
        int handleReadEvent(int clientFd, std::function<void(HTTP*, int, std::istringstream&&)>& handler);
        int listenToEvent(std::function<void(HTTP*, int, std::istringstream&&)>&& handler);
        void cleanup() const;
    public:
        HTTP(int port, int workers);
        void registerEndpoint(const std::string& method, const std::string& path, std::function<std::string(HTTP*, HttpRequest&)> handler);
        void startServer();
    };

} // Application

#endif //MULTITHREADED_SERVER_HTTP_H
