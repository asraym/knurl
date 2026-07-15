#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "graph/DependencyGraph.h"
#include "graph/CycleDetector.h"

namespace knurl {

struct ImpactedFile {
    std::string file;
    int distance;
    bool inCycleWithQuery;
};

class ImpactAnalyzer{
public:
    static std::vector<ImpactedFile> analyzeImpact(
            const std::string& targetFile,
            const DependencyGraph& graph,
            const std::vector<Cycle>& cycles);
private:
    static std::unordered_map<std::string, std::unordered_set<std::string>>
        buildReverseIndex(const DependencyGraph& graph);

    static std::unordered_set<std::string> cycleMatesOf(
            const std::string& file,
            const std::vector<Cycle>& cycles);
};

} //namespace knurl
