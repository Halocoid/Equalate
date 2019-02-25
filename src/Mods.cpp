#include "Mods.hpp"

// All messages sent from modules should be in the format:
// <type><type><dependant>
//
// All messages sent to modules will be in the format:
// <return code><type><type><response>
// Return code is 0 for success, 1 for failure
//
// The type characters are as follows:
// Type | Description                 | dependant             | response
// --------------------------------------------------------------------------------
// gp   | get the prefix the bot uses |                       | prefix
// ga   | get permissions of a mask   | mask to check         | perms - 0 failure, -1 for actual
// gk   | get the api key of a service| service               | key - nothing if service not found
// gm   | get all active modules      |                       | modules separared by space
// --------------------------------------------------------------------------------
// is   | sends message to irc        | message to send       |
// it   | turns the module off        |                       |
// im   | send a message to all module|                       |
//      | s, pretending to be from irc| message to send       |
// --------------------------------------------------------------------------------
//
// Additionally, modules may receive:
// 0rc with an irc message
// 0rf - a request to reget values from core - also sent after turning on

bool Mods::Start(const std::string& moduleName) {
    Module* gen = new Module();
    if (gen->Start(moduleName)) {
        modules.push_back(gen);
        Refresh();
        return true;
    } else {
        gen->Terminate();
        delete gen;
        return false;
    }
}

void Mods::SaveMods(Pref* info) {
    info->Assets.modules.clear();
    for (auto const& mod: modules) {
        info->Assets.modules.push_back(basename(mod->name.c_str()));
    }
}

void Mods::SendParse(const std::string& msg, Pref* info, Irc* con, Opts* args) {
    std::vector<std::string> ex;
    std::string send = "0rc" + msg + "\n";
    Helpers::Split(ex, msg, " ");
    for (auto const& mod : modules) {
        mod->Send(send);
    }

    ex.resize(7);

    if (ex[1] != "PRIVMSG") {
        return;
    }
    std::string dest(ex[2]);
    if (ex[2][0] != '#' && ex[2][0] != '&') {
        dest = ex[0].substr(1, ex[0].find('!') - 1);
    }
    dest = "PRIVMSG " + dest + " :";
    std::transform(ex[3].begin(), ex[3].end(), ex[3].begin(), ::tolower);

    if (info->Assets.access.find(ex[0]) != info->Assets.access.end()) {
        switch (info->Assets.access[ex[0]]) {
            case 0: {
                if (ex[3] == ":" + info->Config.prefix + "die") {
                    con->IsUp = false;
                }
                __attribute__((fallthrough));
            }
            case 1: {
                if (ex[3] == ":" + info->Config.prefix + "modules") {
                    if (ex[4] == "list") {
                        SaveMods(info);
                        std::string str(dest + std::to_string(info->Assets.modules.size()) + " active modules: ");
                        bool added = false;
                        for (auto const& mod: info->Assets.modules) {
                            added = true;
                            str += mod + ", ";
                        }
                        if (added) {
                            str.pop_back();
                            str.pop_back();
                        } else {
                            str += "none";
                        }
                        con->Send(str);
                    } else if (ex[4] == "delete") {
                        SaveMods(info);
                        std::string out(dest);
                        bool found = false;
                        for (std::vector<std::string>::iterator it = info->Assets.modules.begin(); it != info->Assets.modules.end(); ) {
                            if ((*it) == ex[5]) {
                                found = true;
                                modules[it - info->Assets.modules.begin()]->Terminate();
                                delete modules[it - info->Assets.modules.begin()];
                                modules.erase(modules.begin() + (it - info->Assets.modules.begin()));
                                break;
                            } else {
                                ++it;
                            }
                        }
                        if (found) {
                            out += "Successfully removed module " + ex[5];
                            SaveMods(info);
                        } else {
                            out += "Unable to find such module";
                        }
                        con->Send(out);
                    } else if (ex[4] == "add") {
                        SaveMods(info);
                        std::string out(dest);
                        bool found = false;
                        for (std::vector<std::string>::iterator it = info->Assets.modules.begin(); it != info->Assets.modules.end(); ) {
                            if ((*it) == ex[5]) {
                                found = true;
                                break;
                            } else {
                                ++it;
                            }
                        }
                        if (found) {
                            out += "Module is already started";
                        } else {
                            if (Start(args->Options.ModuleDirectory + "/" + ex[5])) {
                                out += "Started module " + ex[5];
                            } else {
                                out += "Failed to start module";
                            }
                        }
                        SaveMods(info);
                        con->Send(out);
                    } else {
                        std::string out(dest);
                        if (ex[4] != "help") {
                            out += "Unrecognised option " + ex[4];
                            con->Send(out);
                            out = dest;
                        }
                        out += "Usage: {list|delete <module>|add <module>}";
                        con->Send(out);
                    }
                } else if (ex[3] == ":" + info->Config.prefix + "access") {
                    if (ex[4] == "show") {
                        std::string out(dest);
                        if (info->Assets.access.find(ex[5]) != info->Assets.access.end()) {
                            out += "Access for " + ex[5] + ": " + std::to_string(info->Assets.access[ex[5]]);
                        } else {
                            out += "Unable to find mask " + ex[5];
                        }
                        con->Send(out);
                    } else if (ex[4] == "delete") {
                        std::string out(dest);
                        if (info->Assets.access.find(ex[5]) != info->Assets.access.end()) {
                            info->Assets.access.erase(ex[5]);
                            out += "Successfully removed " + ex[5];
                        } else {
                            out += "Unable to find mask " + ex[5];
                        }
                        con->Send(out);
                    } else if (ex[4] == "set") {
                        std::string out(dest);
                        if (ex[6] == "") {
                            out = "Not enough arguments provided";
                        } else {
                            int set;
                            try {
                                set = std::stoi(ex[6]);
                                if (set < 0) {
                                    out += "Number must be positive";
                                } else {
                                    info->Assets.access[ex[5]] = set;
                                    out += "Set mask to " + std::to_string(set);
                                }
                            } catch (std::invalid_argument&) {
                                out += "Access must be a number";
                            } catch (std::out_of_range&) {
                                out += "Number provided is too large";
                            }
                        }
                        con->Send(out);
                    } else {
                        std::string out(dest);
                        if (ex[4] != "help") {
                            out += "Unrecognised option " + ex[4];
                            con->Send(out);
                            out = dest;
                        }
                        out += "Usage: {show <mask>|delete <mask>|set <mask> <access>";
                        con->Send(out);
                    }
                } else if (ex[3] == ":" + info->Config.prefix + "apikey") {
                    if (ex[4] == "show") {
                        std::string out(dest);
                        if (info->Assets.apiKeys.find(ex[5]) != info->Assets.apiKeys.end()) {
                            out += "Api key for " + ex[5] + ": " + info->Assets.apiKeys[ex[5]];
                        } else {
                            out += "Unable to find service " + ex[5];
                        }
                        con->Send(out);
                    } else if (ex[4] == "delete") {
                        std::string out(dest);
                        if (info->Assets.apiKeys.find(ex[5]) != info->Assets.apiKeys.end()) {
                            info->Assets.apiKeys.erase(ex[5]);
                            out += "Successfully removed " + ex[5];
                        } else {
                            out += "Unable to find service " + ex[5];
                        }
                        con->Send(out);
                        info->Save();
                    } else if (ex[4] == "set") {
                        std::string out(dest);
                        if (ex[6] == "") {
                            out = "Not enough arguments provided";
                        } else {
                            info->Assets.apiKeys[ex[5]] = ex[6];
                            out += "Set service " + ex[5] + " key to " + ex[6];
                        }
                        con->Send(out);
                        info->Save();
                    } else {
                        std::string out(dest);
                        if (ex[4] != "help") {
                            out += "Unrecognised option " + ex[4];
                            con->Send(out);
                            out = dest;
                        }
                        out += "Usage: {show <service>|delete <service>|set <service> <key>";
                        con->Send(out);
                    }
                }
                __attribute__((fallthrough));
            }
            default: {
                break;
            }
        }
    }
}

void Mods::Refresh() {
    for (auto const& mod: modules) {
        mod->Refresh();
    }
}

void Mods::Terminate() {
    for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end(); ) {
        (*it)->Terminate();
        delete *it;
        it = modules.erase(it);
    }
}

void Mods::ParseGets(Pref* info, Log* logger, Irc* con, Opts* args) {
    std::string rbuf;
    for (std::vector<Module*>::iterator it = modules.begin(); it != modules.end(); ) {
        if (!(*it)->Alive()) {
            (*it)->Terminate();
            delete (*it);
            it = modules.erase(it);
            continue;
        }
        rbuf = "";
        int rc = (*it)->Get(rbuf);
        bool trm = false;

        if (rc == 0) {
            ++it;
        } else if (rc == -1) {
            logger->Write(Log::Level::Error, "Encountered unexpected error in module: ", (*it)->name);
            ++it;
        } else {
            std::string rt = "0uu";

            if (rbuf.length() >= 2) {
                rt.replace(1, 2, rbuf.substr(0, 2));
                switch (rbuf[0]) {
                    case 'g': {
                        switch (rbuf[1]) {
                            case 'p': {
                                rt += info->Config.prefix;
                                break;
                            }
                            case 'a': {
                                std::string mask(rbuf.substr(2, rbuf.length() - 2));
                                if (info->Assets.access.find(mask) != info->Assets.access.end()) {
                                    rt += info->Assets.access[mask];
                                } else {
                                    rt += '0';
                                }
                                break;
                            }
                            case 'k': {
                                std::string service(rbuf.substr(2, rbuf.length() - 2));
                                if (info->Assets.apiKeys.find(service) != info->Assets.apiKeys.end()) {
                                    rt += info->Assets.apiKeys[service];
                                }
                                break;
                            }
                            case 'm': {
                                bool added = false;
                                for (auto const& mod: modules) {
                                    added = true;
                                    rt += mod->name + " ";
                                }
                                if (added) {
                                    rt.erase(rt.length() - 2, 2);
                                }
                                break;
                            }
                            default: {
                                rt[0] = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 'i': {
                        switch (rbuf[1]) {
                            case 's': {
                                con->Send(rbuf.substr(2, rbuf.length() - 2));
                                break;
                            }
                            case 't': {
                                (*it)->Terminate();
                                trm = true;
                                break;
                            }
                            case 'm': {
                                SendParse(rbuf.substr(2, rbuf.length() - 2), info, con, args);
                                break;
                            }
                            default: {
                                rt[0] = 1;
                                break;
                            }
                        }
                        break;
                    }
                    default: {
                        rt[0] = 1;
                        break;
                    }
                }
            } else {
                rt[0] = 1;
            }

            if (!(*it)->Alive()) {
                (*it)->Terminate();
                trm = true;
            } else {
                (*it)->Send(rt);
            }

            if (trm) {
                delete (*it);
                it = modules.erase(it);
            } else {
                ++it;
            }
        }
    }
}
