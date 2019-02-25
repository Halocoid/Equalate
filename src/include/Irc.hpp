#ifndef IRC_HPP_
#define IRC_HPP_

#include "Log.hpp"
#include "Pref.hpp"
#include "Helpers.hpp"

#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <error.h>
#include <unistd.h>

class Irc {
public:
    Irc(Log* logger, Pref* preferences);
    bool Connect();
    void Send(std::string toWrite);
    void Register();
    bool Receive(std::string& rec, std::vector<std::string>& vec);
    void Terminate();

    bool IsUp;
    bool RWait;

private:
    Log* Logger;
    Pref* Preferences;

    std::vector<std::string> ReceiveBuffer;
    char Buffer[4096];
    int Socket;
};

#endif
