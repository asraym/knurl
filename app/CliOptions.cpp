#include "app/CliOptions.h"
#include <iostream>
#include <vector>

namespace knurl {

bool CliOptionsParser::parse(int argc, char** argv, CliOptions& outOptions, std::string& errorMessage) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) args.push_back(argv[i]);

    bool rootDirSet = false;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "--target") {
            if (i + 1 >= args.size()) {
                errorMessage = "--target requires a file path argument";
                return false;
            }
            outOptions.targetFile = args[++i];
        } else if (arg == "--top") {
            if (i + 1 >= args.size()) {
                errorMessage = "--top requires a numeric argument";
                return false;
            }
            const std::string& val = args[++i];
            try {
                size_t consumed = 0;
                int parsed = std::stoi(val, &consumed);
                if (consumed != val.size() || parsed <= 0) {
                    errorMessage = "--top must be a positive integer, got '" + val + "'";
                    return false;
                }
                outOptions.topK = parsed;
            } catch (...) {
                errorMessage = "--top must be a positive integer, got '" + val + "'";
                return false;
            }
        } else if (arg.rfind("--", 0) == 0) {
            errorMessage = "Unrecognized flag: " + arg;
            return false;
        } else {
            if (rootDirSet) {
                errorMessage = "Unexpected extra positional argument: " + arg;
                return false;
            }
            outOptions.rootDir = arg;
            rootDirSet = true;
        }
    }

    if (!rootDirSet) {
        errorMessage = "Missing required <root_dir> argument";
        return false;
    }

    return true;
}

void CliOptionsParser::printUsage(const std::string& programName) {
    std::cerr << "Usage: " << programName << " <root_dir> [--target <file>] [--top <N>]\n"
              << "  <root_dir>       directory to scan for Python files\n"
              << "  --target <file>  also report impact analysis for this file\n"
              << "  --top <N>        number of files to show in the risk ranking (default 10)\n";
}

}  // namespace knurl
