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

    bool isVirtualEnvRoot(const fs::path & dir){
        std::error_code ec;
        return fs::exists(dir / "pyvenv.cfg", ec);
    }
}

std::vector<std::string> FileWalker::findPythonFiles(const std::string& rootDir){
    std::vector<std::string> result;

    for(auto it = fs::recursive_directory_iterator(rootDir);
        it != fs::recursive_directory_iterator(); ){
            const fs::path& path = it->path();

            if(it->is_directory() ){
                bool nameMatch = kSkipDirs.count(path.filename().string()) > 0;
                if(nameMatch || isVirtualEnvRoot(path)){
                    it.disable_recursion_pending();
                    ++it;
                    continue;                    
                }

            }
            if(it->is_regular_file() && path.extension() == ".py"){
                result.push_back(path.string());
            }
            std::error_code ec;
            it.increment(ec);
            if(ec){
                break;
            }
        }
        return result;
}

} // namespace knurl
