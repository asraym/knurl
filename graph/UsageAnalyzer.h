#pragma once
#include <string>
#include "scanning/UsageIndexer.h"

namespace knurl {

enum class UsageTier {
    Tier1_TopLevelExecution,  // seen at indent 0 -- runs the moment the file is imported
    Tier2_ReachableOnly,      // seen only inside indented blocks (def/class/if/etc.)
    Tier3_Unused              // never referenced again after the import
};

class UsageAnalyzer {
public:
    static UsageTier classify(const std::string& localName, const FileUsageIndex& index);
};

}  // namespace knurl
