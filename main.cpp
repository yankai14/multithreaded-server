//
// Created by Yan Kai Lim on 3/12/23.
//

#include "application/HTTP.h"

int main() {
    // TODO: Rule of 5
    Application::HTTP http(8080, 4);
//    http.registerEndpoint("GET", "127.0.0.1:8080", [] (Application::HTTP* http, std::unordered_map<std::string, std::string>& req) -> std::string {
//        return "Hello world";
//    });
    http.startServer();
}