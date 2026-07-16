#pragma once
#include <string>
#include <unordered_map>
#include "graph/UsageAnalyzer.h"

namespace knurl {

class EdgeTierClassifier {
public:
    // Classifies one DependencyGraph edge (from -> to): looks at every
    // LocalBinding in `from` that resolves to `to`, classifies each one's
    // usage tier, and combines them worst-tier-wins (Tier1 beats Tier2 beats
    // Tier3) -- if any name pulled from `to` blows up at import time, the
    // edge as a whole is dangerous regardless of how the other names are used.
    static UsageTier classifyEdge(const std::string& from,
                                   const std::string& to,
                                   const std::string& rootDir,
                                   const std::unordered_map<std::string, std::string>& moduleMap);

private:
    static int severityRank(UsageTier t);  
};

}  // namespace knurl
