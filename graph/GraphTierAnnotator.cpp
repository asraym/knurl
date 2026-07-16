#include "graph/GraphTierAnnotator.h"
#include "graph/ModuleResolver.h"
#include "graph/EdgeTierClassifier.h"
#include <vector>
#include <algorithm>

namespace knurl {

std::vector<TieredEdge> GraphTierAnnotator::annotate(const DependencyGraph& graph, const std::string& rootDir) {
    std::vector<std::string> allFiles(graph.nodes.begin(), graph.nodes.end());
    std::sort(allFiles.begin(), allFiles.end());

    auto moduleMap = ModuleResolver::buildModuleMap(allFiles, rootDir);
    std::vector<TieredEdge> result;

    for (const auto& from : allFiles) {
        auto it = graph.edges.find(from);
        if (it == graph.edges.end()) continue;

        std::vector<std::string> targets(it->second.begin(), it->second.end());
        std::sort(targets.begin(), targets.end());

        for (const auto& to : targets) {
            UsageTier tier = EdgeTierClassifier::classifyEdge(from, to, rootDir, moduleMap);
            result.push_back({from, to, tier});
        }
    }

    return result;
}

}  // namespace knurl
