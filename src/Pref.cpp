#include "Pref.hpp"

Pref::Pref(Log* logger, Opts* args)
: Args(args)
, Logger(logger) {}

bool Pref::Reload() {
    struct stat info;
    if (stat(Args->Options.PreferencesFile.c_str(), &info) != 0) {
        Logger->Write(Log::Level::Error, "Cannot access file ", Args->Options.PreferencesFile);
        return false;
    }
    if (info.st_mode & S_IFDIR) {
        Logger->Write(Log::Level::Error, Args->Options.PreferencesFile, " is a directory, not a file!");
        return false;
    }

    std::ifstream t(Args->Options.PreferencesFile);
    json prf;
    try {
        t >> prf;
    } catch (json::parse_error& e) {
        Logger->Write(e.what());
        return false;
    }

    if (!prf.is_object()) {
        Logger->Write(Log::Level::Error, "Preferences file is not an object");
        return false;
    }

    return (ReloadConfig(prf) && ReloadAssets(prf));
}

bool Pref::ReloadConfig(json& prf) {
    bool wronged = false;
    Logger->CurrentLevel = Log::Level::Error;

    if ((prf.count("names") == 1) && prf["names"].is_object()) {
        if ((prf["names"].count("nick") == 1) && prf["names"]["nick"].is_string()) {
            Config.nickname = prf["names"]["nick"].get<std::string>();
        } else {
            Logger->Write("No 'names/nick' field provided");
            wronged = true;
        }
        if ((prf["names"].count("user") == 1) && prf["names"]["user"].is_string()) {
            Config.username = prf["names"]["user"].get<std::string>();
        } else {
            Logger->Write("No 'names/user' field provided");
            wronged = true;
        }
        if ((prf["names"].count("real") == 1) && prf["names"]["real"].is_string()) {
            Config.realname = prf["names"]["real"].get<std::string>();
        } else {
            Logger->Write("No 'names/real' field provided");
            wronged = true;
        }
    } else {
        Logger->Write("No 'names' object provided");
        wronged = true;
    }

    if ((prf.count("pass") == 1) && prf["pass"].is_string()) {
        Config.pass = prf["pass"].get<std::string>();
    } else {
        Logger->Write(Log::Level::Warning, "No 'pass' field provided, assuming empty");
        Config.pass = "";
    }

    if ((prf.count("prefix") == 1) && prf["prefix"].is_string()) {
        Config.prefix = prf["prefix"].get<std::string>();
    } else {
        Logger->Write(Log::Level::Warning, "No 'prefix' field provided, assuming '!'");
        Config.prefix = "!";
    }

    if ((prf.count("server") == 1) && prf["server"].is_object()) {
        if ((prf["server"].count("host") == 1) && prf["server"]["host"].is_string()) {
            Config.serverHost = prf["server"]["host"].get<std::string>();
        } else {
            Logger->Write("No 'server/host' field provided");
            wronged = true;
        }
        if ((prf["server"].count("port") == 1) && prf["server"]["port"].is_number()) {
            Config.serverPort = prf["server"]["port"].get<int>();
        } else {
            Logger->Write("No 'server/port' field provided");
            wronged = true;
        }
    } else {
        Logger->Write("No 'server' object provided");
        wronged = true;
    }

    if ((prf.count("startup") == 1) && prf["startup"].is_array()) {
        bool wrongedd = false;
        for (json::iterator it = prf["startup"].begin(); it != prf["startup"].end(); ++it) {
            if (it->is_string()) {
                Config.startupCommands.push_back(it->get<std::string>());
            } else {
                wrongedd = true;
            }
        }
        if (wrongedd) {
            Logger->Write("'startup' array contains non-strings");
            wronged = true;
        }
    } else {
        Logger->Write(Log::Level::Warning, "No 'startup' array provided, assuming empty");
    }

    Logger->CurrentLevel = Log::Level::Trace;
    return !wronged;
}

bool Pref::ReloadAssets(json& prf) {
    Logger->CurrentLevel = Log::Level::Error;
    bool wrong = false;

    if ((prf.count("assets") == 1) && prf["assets"].is_object()) {
        if ((prf["assets"].count("access") == 1) && prf["assets"]["access"].is_object()) {
            bool wrongtype = false;
            for (json::iterator it = prf["assets"]["access"].begin(); it != prf["assets"]["access"].end(); ++it) {
                if (it.value().is_number()) {
                    Assets.access[it.key()] = it.value().get<int>();
                } else {
                    wrongtype = true;
                }
            }
            if (wrongtype) {
                Logger->Write("'assets/access' object should be a string:int pair");
                wrong = true;
            }
        } else {
            Logger->Write(Log::Level::Warning, "No 'assets/access' object provided, assuming empty");
        }

        if ((prf["assets"].count("apiKeys") == 1) && prf["assets"]["apiKeys"].is_object()) {
            bool wrongtype = false;
            for (json::iterator it = prf["assets"]["apiKeys"].begin(); it != prf["assets"]["apiKeys"].end(); ++it) {
                if (it.value().is_string()) {
                    Assets.apiKeys[it.key()] = it.value().get<std::string>();
                } else {
                    wrongtype = true;
                }
            }
            if (wrongtype) {
                Logger->Write("'assets/apiKeys' object should be a string:string pair");
                wrong = true;
            }
        } else {
            Logger->Write(Log::Level::Warning, "No 'assets/apiKeys' object provided, assuming empty");
        }

        if ((prf["assets"].count("channels") == 1) && prf["assets"]["channels"].is_object()) {
            bool wrongtype = false;
            for (json::iterator it = prf["assets"]["channels"].begin(); it != prf["assets"]["channels"].end(); ++it) {
                if (it.value().is_string()) {
                    Assets.channels[it.key()] = it.value().get<std::string>();
                } else {
                    wrongtype = true;
                }
            }
            if (wrongtype) {
                Logger->Write("'assets/channels' object should be a string:string pair");
                wrong = true;
            }
        } else {
            Logger->Write(Log::Level::Warning, "No 'assets/channels' object provided, assuming empty");
        }

        if ((prf["assets"].count("modules") == 1) && prf["assets"]["modules"].is_array()) {
            bool wrongtype = false;
            for (json::iterator it = prf["assets"]["modules"].begin(); it != prf["assets"]["modules"].end(); ++it) {
                if (it->is_string()) {
                    Assets.modules.push_back(it->get<std::string>());
                } else {
                    wrongtype = true;
                }
            }
            if (wrongtype) {
                Logger->Write("'assets/modules' array contains non-strings");
                wrong = true;
            }
        } else {
            Logger->Write(Log::Level::Warning, "No 'assets/modules' array provided, assuming empty");
        }
    }

    Logger->CurrentLevel = Log::Level::Trace;
    return !wrong;
}

bool Pref::Save() {
    std::ofstream out(Args->Options.PreferencesFile);
    json prf;
    prf["names"]["nick"] = Config.nickname;
    prf["names"]["user"] = Config.username;
    prf["names"]["real"] = Config.realname;
    prf["pass"] = Config.pass;
    prf["prefix"] = Config.prefix;
    prf["server"]["host"] = Config.serverHost;
    prf["server"]["port"] = Config.serverPort;
    prf["startup"] = Config.startupCommands;

    prf["assets"]["access"] = Assets.access;
    prf["assets"]["apiKeys"] = Assets.apiKeys;
    prf["assets"]["channels"] = Assets.channels;
    prf["assets"]["modules"] = Assets.modules;

    out << std::setw(4) << prf;
    out.close();
    return true;
}
