#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include "Helpers.hpp"

class Manager {
public:
    Manager(bool prefix, bool modules);
    bool Get(std::string& rec, bool blocking);

    int GetPerms(const std::string& mask);
    void GetKey(const std::string& service, std::string& vec);

    void IrcSend(const std::string& str);
    void Off();
    void MimicIrcSend(std::string& str);

    struct {
        std::string Prefix;
        std::vector<std::string> Modules;
    } Data;

private:
    void Reget();
    std::vector<std::string> RecBuf;
    bool prefix;
    bool modules;
};

#endif
