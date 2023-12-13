//
// Created by Yan Kai Lim on 9/12/23.
//


#include <string>
#include <vector>
#include <sstream>


namespace Utils {
    std::vector<std::string> split(std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream stream(str);
        std::string token;

        while (std::getline(stream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    std::pair<std::string, std::string> splitFirst(const std::string& str, char delimiter) {
        size_t pos = str.find(delimiter);

        if (pos != std::string::npos) {
            // Found the delimiter
            std::string part1 = str.substr(0, pos);
            std::string part2 = str.substr(pos + 1);  // Skip the delimiter
            return std::make_pair(part1, part2);
        } else {
            // Delimiter not found, return the whole string
            return std::make_pair(str, "");
        }
    }

    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }
}