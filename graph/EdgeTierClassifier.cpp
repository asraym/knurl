#include "graph/EdgeTierClassifier.h"
#include "scanning/LocalBindingExtractor.h"
#include "scanning/UsageIndexer.h"
#include "graph/ModuleResolver.h"
#include "scanning/PyImportExtractor.h"  // RawImport

namespace knurl {

int EdgeTierClassifier::severityRank(UsageTier t) {
    switch (t) {
        case UsageTier::Tier1_TopLevelExecution: return 0;
        case UsageTier::Tier2_ReachableOnly:      return 1;
        case UsageTier::Tier3_Unused:             return 2;
    }
    return 2;
}

UsageTier EdgeTierClassifier::classifyEdge(const std::string& from,
                                            const std::string& to,
                                            const std::string& rootDir,
                                            const std::unordered_map<std::string, std::string>& moduleMap) {
    auto bindings = LocalBindingExtractor::extractBindings(from);
    auto index = UsageIndexer::buildIndex(from);

    UsageTier worst = UsageTier::Tier3_Unused;

    for (const auto& b : bindings) {
        RawImport asImport{from, b.importedModule, b.relativeDots};
        std::string resolved = ModuleResolver::resolve(asImport, rootDir, moduleMap);
        if (resolved != to) continue;

        UsageTier tier = UsageAnalyzer::classify(b.localName, index);
        if (severityRank(tier) < severityRank(worst)) {
            worst = tier;
        }
    }

    return worst;
}

}  // namespace knurl
