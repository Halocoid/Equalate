#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "Log.hpp"

#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>

class Module {
public:
    Module();
    bool Start(const std::string& file);
    void Send(const std::string& str);
    void Refresh();
    int Get(std::string& rec);
    void Terminate();
    bool Alive();

    std::string name;

private:
    pid_t pid;

    int writefd;
    int readfd;
};

#endif
