#pragma once
#include <string>
namespace knurl {
struct CliOptions {
    std::string rootDir;
    std::string targetFile;
    int topK = 10;
    bool ftree = false;
    bool itree = false;
    bool deptree = false;
    std::string dotOutPath;  // empty = not requested

    bool hasTarget() const { return !targetFile.empty(); }
    bool hasDot() const { return !dotOutPath.empty(); }
    bool hasTreeMode() const { return ftree || itree || deptree; }
};
class CliOptionsParser {
public:
    static bool parse(int argc, char** argv, CliOptions& outOptions, std::string& errorMessage);
    static void printUsage(const std::string& programName);
};
}  // namespace knurl
