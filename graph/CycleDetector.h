#pragma once

#include <vector>
#include <string>
#include "graph/DependencyGraph.h"

namespace knurl{

struct Cycle {
    std::vector<std::string> files;
};

class CycleDetector {
public:
    static std::vector<Cycle> findCycles(const knurl::DependencyGraph& graph);

private:
    enum class Color { White, Gray, Black };

    static void dfs(const std::string& node, const knurl::DependencyGraph& graph, std::unordered_map<std::string, Color>& color, std::vector<std::string>& pathStack, std::vector<std::vector<std::string>>& rawCycles);

    static std::vector<std::string> canonicalize(std::vector<std::string> cycle);
};

} // namespace knurl
