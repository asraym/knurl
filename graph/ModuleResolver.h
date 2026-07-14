#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "scanning/PyImportExtractor.h"

namespace knurl {

class ModuleResolver {
public:
    static std::unordered_map<std::string, std::string> buildModuleMap(
        const std::vector<std::string>& allFiles, const std::string& rootDir);

    static std::string filePathToModule(const std::string& filePath, const std::string& rootDir);

    static std::string resolve(const RawImport& imp, const std::string& rootDir,
                                const std::unordered_map<std::string, std::string>& moduleMap);

private:
    static bool isPackageFile(const std::string& filePath);
    static std::optional<std::string> stripLastSegment(const std::string& module);
    static std::optional<std::string> ancestorOf(const std::string& module, int levels);
};

} // namespace knurl
