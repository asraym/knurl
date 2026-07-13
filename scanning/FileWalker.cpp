#include "FileWalker.h"

#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;

namespace knurl {

namespace {
    const std::unordered_set<std::string> kSkipDirs = {
        "venv", ".venv", "env", ".git", "__pycache__",
        ".pytest_cache", "node_modules"
    };

    bool containSkippedDir(const fs::path& path){
        for(const auto& part: path){
            if(kSkipDirs.count(part.string()) > 0){
                return true;
            }
        }
        return false;
    }
}

std::vector<std::string> FileWalker::findPythonFiles(const std::string& rootDir){
    std::vector<std::string> result;

    for(const auto& entry: fs::recursive_directory_iterator(rootDir)){
        if(!entry.is_regular_file()){
            continue;
        }

        const fs::path& path = entry.path();
        if(containSkippedDir(path)) {
            continue;
        }
        if(path.extension() == ".py"){
            result.push_back(path.string());
        }
    }
    return result;
}

} // namespace knurl
