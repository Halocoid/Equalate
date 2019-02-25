#ifndef MODS_HPP
#define MODS_HPP

#include "Module.hpp"
#include "Pref.hpp"
#include "Log.hpp"
#include "Irc.hpp"
#include "Helpers.hpp"
#include "Opts.hpp"

#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>


class Mods {
public:
    bool Start(const std::string& moduleName);
    void SendParse(const std::string& msg, Pref* info, Irc* con, Opts* args);
    void Refresh();
    void ParseGets(Pref* info, Log* logger, Irc* con, Opts* args);
    void SaveMods(Pref* info);
    void Terminate();

private:
    std::vector<Module*> modules;
};

#endif
