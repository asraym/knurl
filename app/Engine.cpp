#include "app/Engine.h"

namespace knurl {

EngineResult Engine::run(const CliOptions& options) {
    EngineResult result;

    result.graph = DependencyGraph::build(options.rootDir);
    result.cycles = CycleDetector::findCycles(result.graph);
    result.tieredEdges = GraphTierAnnotator::annotate(result.graph, options.rootDir);
    result.ranked = RiskRanker::rankRisk(result.graph, result.cycles, result.tieredEdges);

    if (options.hasTarget()) {
        result.targetQueried = true;
        result.impact = ImpactAnalyzer::analyzeImpact(options.targetFile, result.graph, result.cycles);
    }

    return result;
}

}  // namespace knurl
