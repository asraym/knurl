#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "graph/DependencyGraph.h"
#include "graph/CycleDetector.h"
#include "graph/GraphTierAnnotator.h"

namespace knurl {

struct FileRisk {
    std::string file;
    double structuralRisk;
    double cycleBonus;
    double totalRisk;
};

struct RiskRankingResult {
    std::vector<FileRisk> production;
    std::vector<FileRisk> tests;
};

class RiskRanker {
public:
    static RiskRankingResult rankRisk(const DependencyGraph& graph,
                                       const std::vector<Cycle>& cycles,
                                       const std::vector<TieredEdge>& tieredEdges,
                                       double cycleBonusWeight = 1.0,
                                       double testWeight = 0.2);

    // Exposed for testing/verification against fixtures, not just internal use.
    static bool isTestFile(const std::string& path);

private:
    static double propagationWeight(UsageTier tier);
    static double structuralRiskOf(
        const std::string& target,
        const DependencyGraph& graph,
        const std::unordered_map<std::string, std::unordered_map<std::string, UsageTier>>& tierLookup,
        double testWeight);
    static int cycleMateCountOf(const std::string& target, const std::vector<Cycle>& cycles);
};

}  // namespace knurl
