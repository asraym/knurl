#pragma once
#include <vector>
#include "app/CliOptions.h"
#include "graph/DependencyGraph.h"
#include "graph/CycleDetector.h"
#include "graph/GraphTierAnnotator.h"
#include "graph/RiskRanker.h"
#include "graph/ImpactAnalyzer.h"

namespace knurl {

struct EngineResult {
    DependencyGraph graph;
    std::vector<Cycle> cycles;
    std::vector<TieredEdge> tieredEdges;
    std::vector<FileRisk> ranked;
    std::vector<ImpactedFile> impact;
    bool targetQueried = false;
};

class Engine {
public:
    static EngineResult run(const CliOptions& options);
};

}  // namespace knurl
