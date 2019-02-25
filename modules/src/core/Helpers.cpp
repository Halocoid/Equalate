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

std::string Helpers::GetDest(const std::vector<std::string>& ex) {
    std::string dest(ex[2]);
    if (ex[2][0] != '#' && ex[2][0] != '&') {
        dest = ex[0].substr(1, ex[0].find('!') - 1);
    }
    dest = "PRIVMSG " + dest + " :";
    return dest;
}
