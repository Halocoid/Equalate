#include "Helpers.hpp"

void Helpers::Split(std::vector<std::string>& vec, std::string str, const std::string token) {
    while (str.size()) {
        int idx = str.find(token);
        if (idx == -1) {
            vec.push_back(str);
            break;
        } else {
            vec.push_back(str.substr(0, idx));
            str = str.substr(idx + token.size());
        }
    }
}
