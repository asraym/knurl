#pragma once
#include <string>
#include <vector>

namespace knurl {

struct LocalBinding {
    std::string sourceFile;
    std::string importedModule;  // same value RawImport would carry for this statement
    std::string localName;       // the actual token this file can reference
    int relativeDots = 0;
};

class LocalBindingExtractor {
public:
    static std::vector<LocalBinding> extractBindings(const std::string& filePath);
};

}  // namespace knurl
