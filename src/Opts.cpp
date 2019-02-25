#include <Opts.hpp>

int Opts::ParseOptions(const int argc, char const* const* argv, Log& logger) {
    for (int i = 1; i < argc; ++i) {
        if ((std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--default") == 0) && argc - 1 >= i + 1) {
            struct stat info;
            if (stat(argv[i + 1], &info) != 0) {
                WritePrefFile(argv[i + 1]);
                logger.Write("Wrote default preferences to ", argv[i + 1]);
                return 0;
            }
            logger.Write(Log::Level::Error, "File: ", argv[i + 1], " already exists!");
            return 1;
        } else if ((std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0) && argc - 1 >= i) {
            ShowHelp(logger, argv[0]);
            return 0;
        } else if ((std::strcmp(argv[i], "-r") == 0 || std::strcmp(argv[i], "--run") == 0) && argc - 1 >= i + 2) {
            Options.PreferencesFile = argv[i + 1];
            Options.ModuleDirectory = argv[i + 2];
            struct stat info;

            if (stat(Options.ModuleDirectory.c_str(), &info) != 0) {
                logger.Write(Log::Level::Error, "Cannot access directory ", Options.ModuleDirectory);
                return 1;
            }
            if (!(info.st_mode & S_IFDIR)) {
                logger.Write(Log::Level::Error, Options.ModuleDirectory, " is a file, not a directory!");
                return 1;
            }

            return 2;
        }
    }

    ShowHelp(logger, argv[0]);
    return 1;
}

void Opts::ShowHelp(Log& logger, const char* programName) {
    logger.Write("Usage: ", programName, " <option>");
    logger.Write("    -r, --run     <preferences_file> <modules_dir>");
    logger.Write("        Run the bot");
    logger.Write("    -d, --default <file>");
    logger.Write("        Write the default example preference file");
    logger.Write("    -h, --help");
    logger.Write("        Show this help");
}

void Opts::WritePrefFile(const char* filename) {
    std::ofstream out(filename);
    out << "{\n    \"names\": {\n        \"nick\": \"BipBop\",\n        \"user\": \"BeepBoop\",\n        \"real\": \"Beep Boop I'm a bot!\"\n    },\n\n    \"pass\": \"None\",\n    \"prefix\": \"!\",\n\n    \"server\": {\n        \"host\": \"irc.network.com\",\n        \"port\": 6667\n    },\n\n    \"startup\": [\n    ],\n\n    \"assets\": {\n        \"access\": {\n            \":nickname!username@host\": 0\n        },\n        \"apiKeys\": {\n        },\n        \"channels\": {\n        },\n        \"modules\": [\n        ]\n    }\n}\n";
    out.close();
}
