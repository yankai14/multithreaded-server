//
// Created by Yan Kai Lim on 9/12/23.
//

#ifndef MULTITHREADED_SERVER_UTILS_H
#define MULTITHREADED_SERVER_UTILS_H

#include <string>
#include <vector>
#include <sstream>

namespace Utils {
    std::vector<std::string> split(std::string& str, char delimiter);
    std::pair<std::string, std::string> splitFirst(const std::string& str, char delimiter);
    void ltrim(std::string &s);
    void rtrim(std::string &s);
    void trim(std::string &s);
}
#endif //MULTITHREADED_SERVER_UTILS_H
