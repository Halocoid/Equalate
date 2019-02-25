#ifndef OPTS_HPP_
#define OPTS_HPP_

#include "Log.hpp"

#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

class Opts {
public:
    struct {
        std::string PreferencesFile;
        std::string ModuleDirectory;
    } Options;

    int ParseOptions(const int argc, char const* const* argv, Log& logger);

private:
    static void ShowHelp(Log& logger, const char* programName);
    static void WritePrefFile(const char* filename);
};

#endif
