#ifndef PREF_HPP_
#define PREF_HPP_

#include "Log.hpp"
#include "Opts.hpp"
#include "NielsLohmann/json.hpp"

#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>

using json = nlohmann::json;

class Pref {
public:
    Pref(Log* logger, Opts* args);

    bool Reload();
    bool Save();

    struct {
        std::string nickname;
        std::string username;
        std::string realname;
        std::string pass;

        std::string serverHost;
        int serverPort;

        std::string prefix;
        std::vector<std::string> startupCommands;
    } Config;
    struct {
        std::map<std::string, int> access;
        std::map<std::string, std::string> apiKeys;
        std::map<std::string, std::string> channels;
        std::vector<std::string> modules;
    } Assets;

    Opts* Args;
private:
    bool ReloadConfig(json& prf);
    bool ReloadAssets(json& prf);

    Log* Logger;
};

#endif
