#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "graph/ModuleResolver.h"

namespace knurl {

struct DependencyGraph {
    std::unordered_set<std::string> nodes;
    std::unordered_map<std::string, std::unordered_set<std::string>> edges;
    std::unordered_map<std::string, std::unordered_set<std::string>> externals;

    void addNode(const std::string& file);
    void addEdge(const std::string& from, const std::string& to);
    void addExternal(const std::string& from, const std::string& moduleName);

    static DependencyGraph build(const std::string& rootDir);
};

} // namespace knurl
