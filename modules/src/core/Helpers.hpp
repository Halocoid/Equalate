#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <string>
#include <vector>

class Helpers {
public:
    static void Split(std::vector<std::string>& vec, std::string str, const std::string token);

    static std::string GetDest(const std::vector<std::string>& ex);
};

#endif
