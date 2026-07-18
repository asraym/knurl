#pragma once
#include <string>

namespace knurl {

struct CliOptions {
    std::string rootDir;
    std::string targetFile;
    int topK = 10;
    bool hasTarget() const { return !targetFile.empty(); }
};

class CliOptionsParser {
public:
    static bool parse(int argc, char** argv, CliOptions& outOptions, std::string& errorMessage);
    static void printUsage(const std::string& programName);
};

}  // namespace knurl
