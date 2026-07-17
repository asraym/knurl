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

class RiskRanker {
public:
    static std::vector<FileRisk> rankRisk(const DependencyGraph& graph,
                                           const std::vector<Cycle>& cycles,
                                           const std::vector<TieredEdge>& tieredEdges,
                                           double cycleBonusWeight = 1.0);

private:
    static double propagationWeight(UsageTier tier);
    static double structuralRiskOf(
        const std::string& target,
        const DependencyGraph& graph,
        const std::unordered_map<std::string, std::unordered_map<std::string, UsageTier>>& tierLookup);
    static int cycleMateCountOf(const std::string& target, const std::vector<Cycle>& cycles);
};

}  // namespace knurl
