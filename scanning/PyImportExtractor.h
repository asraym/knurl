#pragma once

#include <string>
#include <vector>

namespace knurl {

struct RawImport {
    std::string sourceFile;
    std::string importedModule;
    int relativeDots = 0;         //0 = absolute, 1+ = relatve depth
};

class PyImportExtractor {
public: 
    static std::vector<RawImport> extractImports(const std::string& sourceFile);
};

} // namespace knurl
