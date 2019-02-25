#include "Log.hpp"
#include "Opts.hpp"
#include "Pref.hpp"
#include "Irc.hpp"
#include "Mods.hpp"
#include "Helpers.hpp"

#include <unistd.h>

int main(int argc, char** argv) {
    Log* logger = new Log();
    Opts* args = new Opts();

    int out = args->ParseOptions(argc, argv, *logger);
    if (out == 0 || out == 1) {
        return out;
    }

    Pref* preferences = new Pref(logger, args);
    if (!preferences->Reload()) {
        return 1;
    }

    Mods* modules = new Mods();
    int started = 0;
    for (std::vector<std::string>::iterator it = preferences->Assets.modules.begin(); it != preferences->Assets.modules.end(); ) {
        if (modules->Start(args->Options.ModuleDirectory + "/" + (*it))) {
            started++;
            logger->Write("Started module ", (*it));
            ++it;
        } else {
            logger->Write(Log::Level::Error, "Failed to start module ", (*it));
            it = preferences->Assets.modules.erase(it);
        }
    }
    logger->Write("Started ", started, "/", preferences->Assets.modules.size(), " modules");

    Irc* connection = new Irc(logger, preferences);
    if (!connection->Connect()) {
        return 1;
    }

    connection->Register();
    connection->RWait = false;

    std::string temp;
    std::vector<std::string> ex;
    while (connection->IsUp) {
        if (connection->Receive(temp, ex)) {
            ex.clear();
            modules->SendParse(temp, preferences, connection, args);
        } else {
            modules->ParseGets(preferences, logger, connection, args);
            usleep(200000);
        }
    }

    preferences->Save();
    modules->Terminate();
    connection->Terminate();

    delete connection;
    delete preferences;
    delete args;
    delete logger;
    delete modules;

    return 0;
}
