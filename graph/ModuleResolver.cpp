#include "graph/ModuleResolver.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace knurl {

bool ModuleResolver::isPackageFile(const std::string& filePath) {
    return fs::path(filePath).filename() == "__init__.py";
}

std::string ModuleResolver::filePathToModule(const std::string& filePath, const std::string& rootDir) {
    fs::path rel = fs::relative(fs::path(filePath), fs::path(rootDir));
    std::vector<std::string> parts;
    for (const auto& part : rel) {
        parts.push_back(part.string());
    }
    if (!parts.empty()) {
        std::string& last = parts.back();
        if (last == "__init__.py") {
            parts.pop_back();
        } else if (last.size() > 3 && last.substr(last.size() - 3) == ".py") {
            last = last.substr(0, last.size() - 3);
        }
    }
    std::string module;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) module += ".";
        module += parts[i];
    }
    return module;
}

std::unordered_map<std::string, std::string> ModuleResolver::buildModuleMap(
    const std::vector<std::string>& allFiles, const std::string& rootDir) {
    std::unordered_map<std::string, std::string> map;
    for (const auto& file : allFiles) {
        std::string mod = filePathToModule(file, rootDir);
        map.emplace(mod, file);
    }
    return map;
}

std::optional<std::string> ModuleResolver::stripLastSegment(const std::string& module) {
    if (module.empty()) return std::nullopt;
    auto pos = module.rfind('.');
    return pos == std::string::npos ? std::string("") : module.substr(0, pos);
}

std::optional<std::string> ModuleResolver::ancestorOf(const std::string& module, int levels) {
    std::optional<std::string> current = module;
    for (int i = 0; i < levels; ++i) {
        current = stripLastSegment(*current);
        if (!current) return std::nullopt;
    }
    return current;
}

std::string ModuleResolver::resolve(const RawImport& imp, const std::string& rootDir,
                                     const std::unordered_map<std::string, std::string>& moduleMap) {
    std::string target;

    if (imp.relativeDots == 0) {
        target = imp.importedModule;
    } else {
        std::string sourceModule = filePathToModule(imp.sourceFile, rootDir);
        bool isPkg = isPackageFile(imp.sourceFile);
        std::string currentPackage = isPkg ? sourceModule : stripLastSegment(sourceModule).value_or("");

        auto base = ancestorOf(currentPackage, imp.relativeDots - 1);
        if (!base) return "";

        if (imp.importedModule.empty()) {
            target = *base;
        } else if (base->empty()) {
            target = imp.importedModule;
        } else {
            target = *base + "." + imp.importedModule;
        }
    }

    auto it = moduleMap.find(target);
    return it != moduleMap.end() ? it->second : "";
}

} // namespace knurl
