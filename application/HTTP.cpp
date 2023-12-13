//
// Created by Yan Kai Lim on 7/12/23.
//

#include "HTTP.h"


namespace Application {
    volatile sig_atomic_t g_stopSignal = 0;

    std::string OK = "HTTP/1.1 200 OK\r\n";
    std::string CREATED = "HTTP/1.1 201 Created\r\n";
    std::string BAD_REQUEST = "HTTP/1.1 400 Bad Request\r\n";
    std::string NOT_FOUND = "HTTP/1.1 404 Not Found\r\n";

    HTTP::HTTP(int port, int workers): serverSocket(std::make_unique<Transport::BaseSocket>(port, Transport::TransportProtocol::TCP)),
                                       threadpool(workers) {
        struct sigaction sigIntHandler{};
        sigIntHandler.sa_handler = handleSIGINT;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, nullptr);
        sigaction(SIGTERM, &sigIntHandler, nullptr);
    }

    void HTTP::parse(std::istringstream iss, std::unordered_map<std::string, std::string>& req) {
        std::string word;
        std::vector<std::string> rawContent;
        HttpRequest request;

        std::vector<std::string> rawReq;
        while (std::getline(iss, word, '\n')) {
            word = Utils::split(word, '\r')[0];
            rawReq.push_back(std::move(word));
        }

        if (rawReq.begin() == rawReq.end()) return;

        std::vector<std::string> methodAndHTTPVersion = Utils::split(*rawReq.begin(), '/');
        if (methodAndHTTPVersion.size() != 3) return;
        req["Method"] = methodAndHTTPVersion[0];
        req["HttpVersion"] = methodAndHTTPVersion[1] + '/' + methodAndHTTPVersion[2];
        Utils::trim(req["Method"]);
        Utils::trim(req["HttpVersion"]);

        for (auto it=rawReq.begin()+1; it!=rawReq.end(); it++) {
            auto [headerName, headerValue] = Utils::splitFirst(*it, ':');
            if (headerValue.empty()) return;
            req[headerName] = headerValue;
            Utils::trim(req[headerName]);
        }

        assert(!req["Host"].empty());
        assert(!req["Accept"].empty());
        assert(!req["Connection"].empty());
    }

    int HTTP::initKq() {
        kq = kqueue();
        if (kq == -1) {
            std::cerr << "Error creating kqueue" << std::endl;
            return -1;
        }
        return kq;
    }

    int HTTP::registerServerFdToKq() const {
        struct kevent event{};
        EV_SET(&event, serverSocket->serverSocketFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1) {
            std::cerr << "Error registering server socket" << std::endl;
            return -1;
        }

        return 1;
    }

    int HTTP::handleAcceptEvent() const {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(serverSocket->serverSocketFd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            std::cerr << "Unable to accept client connection" << std::endl;
            return -1;
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

    int HTTP::handleReadEvent(int clientFd, std::function<void(HTTP*, int, std::istringstream&&)>& handler) {
        char buffer[30000];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientFd, buffer, 30000, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client " << clientFd << " disconnected, cleaning up...." << std::endl;
            struct kevent event{};
            EV_SET(&event, clientFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(kq, &event, 1, nullptr, 0, nullptr);
            close(clientFd);
        } else {
            // Parse out the client's request string e.g. GET /index.html HTTP/1.1
            std::istringstream iss(buffer);
            // Use a lambda to wrap the handler function
            auto task = [this, handler = std::move(handler), clientFd, iss=std::move(iss)]() mutable {
                handler(this, clientFd, std::move(iss));
            };
            threadpool.enqueueTask(std::move(task));
        }
        return 1;
    }

    int HTTP::listenToEvent(std::function<void(HTTP*, int, std::istringstream&&)>&& handler) {
        while (!g_stopSignal) {
            int num_events = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
            if (num_events == -1) {
                if (errno == EINTR) break;
                std::cerr << "kevent wait failed" << std::endl;
                return -1;
            }

            if (g_stopSignal) {
                break;  // Check the condition before processing events
            }

            for (int i=0; i<num_events; i++) {
                int fd = static_cast<int>(events[i].ident);
                if (fd == serverSocket->serverSocketFd) handleAcceptEvent();
                else handleReadEvent(fd, handler);
            }
        }

        return 1;
    }

    void HTTP::handleSIGINT(int signal) {
        g_stopSignal = 1;
    }

    void HTTP::cleanup() const {
        if (serverSocket->serverSocketFd != -1) {
            std::cout << "Closing server socket" << std::endl;
            close(serverSocket->serverSocketFd);
        }

        // Close and deregister all client sockets
        for (const auto & i : events) {
            int fd = static_cast<int>(i.ident);
            if (fd > 0) {
                std::cout << "Cleaning up client socket " << fd << std::endl;
                struct kevent event{};
                EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                kevent(kq, &event, 1, nullptr, 0, nullptr);
                close(fd);
            }
        }

        if (kq != -1) {
            std::cout << "Closing kq" << std::endl;
            close(kq);
        }
    }

    void HTTP::registerEndpoint(const std::string& method, const std::string& path, std::function<std::string(HTTP*, std::unordered_map<std::string, std::string>&)> handler) {
        router[method + " " + path] = std::move(handler);
    }

    void HTTP::startServer() {
        serverSocket->initSocket();
        initKq();
        registerServerFdToKq();
        listenToEvent([](HTTP* http, int clientFd, std::istringstream&& iss)->void {
            if (http == nullptr) {
                std::cerr << "http instance argument is NULL" << std::endl;
                return;
            };
            std::unordered_map<std::string, std::string> req;
            http->parse(std::move(iss), req);
            std::string path = req["Method"] + " " + req["Host"];
            if (http->router.find(path) != http->router.end()) {
                std::string content = http->router[path](http, req);
                std::string contentLengthHeader = "Content-Length: " + std::to_string(content.size());
                std::string header =
                        "Content-Type: text/html\r\n"
                        "Accept-Ranges: bytes\r\n"
                        "Connection: keep-alive\r\n"
                        + contentLengthHeader + "\r\n";
                std::string msg = OK + header + "\r\n" + content;
                send(clientFd, msg.c_str(), msg.size(), 0);
            } else {
                std::string contentLengthHeader = "Content-Length: 0";
                std::string header =
                        "Content-Type: text/html\r\n"
                        "Accept-Ranges: bytes\r\n"
                        "Connection: keep-alive\r\n"
                        + contentLengthHeader + "\r\n";
                std::string msg = NOT_FOUND + header + "\r\n";
                send(clientFd, msg.c_str(), msg.size(), 0);
            }
        });
        cleanup();
        exit(1);
    }
} // Application