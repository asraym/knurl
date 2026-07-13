#pragma once 

#include<string>
#include<vector>

namespace knurl {

class FileWalker {
public:
    //walks rootdir recursively and returns paths of every .py file found.
    static std::vector<std::string> findPythonFiles(const std::string& rootDir);
};

} //namespace knurl
