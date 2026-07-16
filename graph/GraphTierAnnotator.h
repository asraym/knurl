#pragma once
#include <string>
#include <vector>
#include "graph/DependencyGraph.h"
#include "graph/UsageAnalyzer.h"

namespace knurl {

struct TieredEdge {
    std::string from;
    std::string to;
    UsageTier tier;
};

class GraphTierAnnotator {
public:
    static std::vector<TieredEdge> annotate(const DependencyGraph& graph, const std::string& rootDir);
};

}  // namespace knurl
