#pragma once
#include <string>
#include <vector>
#include "graph/DependencyGraph.h"
#include "graph/CycleDetector.h"
#include "graph/RiskRanker.h"

namespace knurl {

class DotExporter {
public:
    // Writes the whole internal dependency graph (no externals) to a .dot
    // file at outPath. Returns false if the file couldn't be opened/written.
    static bool exportDot(const std::string& outPath,
                           const DependencyGraph& graph,
                           const std::vector<Cycle>& cycles,
                           const RiskRankingResult& ranked);

private:
    // Discrete 5-bucket red scale, relative to maxProductionRisk (this
    // repo's own top production score) -- not an absolute hardcoded
    // threshold, so a low-risk repo still shows visual spread.
    static std::string riskFillColor(double totalRisk, double maxProductionRisk);

    static bool isCycleClosingEdge(const std::string& from, const std::string& to,
                                    const std::vector<Cycle>& cycles);
};

}  // namespace knurl
