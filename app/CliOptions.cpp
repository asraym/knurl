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
        } else if (arg == "--ftree") {
            outOptions.ftree = true;
        } else if (arg == "--itree") {
            outOptions.itree = true;
        } else if (arg == "--deptree") {
            outOptions.deptree = true;
        } else if (arg == "--dot") {
            if (i + 1 >= args.size()) {
                errorMessage = "--dot requires a file path argument";
                return false;
            }
            outOptions.dotOutPath = args[++i];
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

    // Tree-mode validation: at most one mode, and each mode's target
    // requirement must hold. Checked after the full pass so combinations
    // are validated regardless of flag order on the command line.
    int treeModeCount = (outOptions.ftree ? 1 : 0) + (outOptions.itree ? 1 : 0) + (outOptions.deptree ? 1 : 0);
    if (treeModeCount > 1) {
        errorMessage = "Only one of --ftree, --itree, --deptree may be given at a time";
        return false;
    }
    if ((outOptions.itree || outOptions.deptree) && !outOptions.hasTarget()) {
        errorMessage = std::string("--") + (outOptions.itree ? "itree" : "deptree") + " requires --target <file>";
        return false;
    }
    if (outOptions.ftree && outOptions.hasTarget()) {
        errorMessage = "--ftree cannot be combined with --target (it renders the whole repo)";
        return false;
    }

    return true;
}
void CliOptionsParser::printUsage(const std::string& programName) {
    std::cerr << "Usage: " << programName
              << " <root_dir> [--target <file>] [--top <N>]"
                 " [--ftree | --itree | --deptree] [--dot <path>]\n"
              << "  <root_dir>       directory to scan for Python files\n"
              << "  --target <file>  also report impact analysis for this file\n"
              << "  --top <N>        number of files to show in the risk ranking (default 10)\n"
              << "  --ftree          render the whole repo as an ASCII forest (no --target)\n"
              << "  --itree          render an ASCII tree of what depends on --target (requires --target)\n"
              << "  --deptree        render both directions from --target: imports and importers (requires --target)\n"
              << "  --dot <path>     write the whole internal dependency graph as a .dot file\n";
}
}  // namespace knurl
