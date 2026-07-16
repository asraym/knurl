#include "graph/UsageAnalyzer.h"

namespace knurl {

UsageTier UsageAnalyzer::classify(const std::string& localName, const FileUsageIndex& index) {
    auto it = index.minIndentOf.find(localName);
    if (it == index.minIndentOf.end()) return UsageTier::Tier3_Unused;
    if (it->second == 0) return UsageTier::Tier1_TopLevelExecution;
    return UsageTier::Tier2_ReachableOnly;
}

}  // namespace knurl
