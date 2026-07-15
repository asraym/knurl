#include "graph/ImpactAnalyzer.h"
#include <algorithm>
#include <queue>

namespace knurl {

std::unordered_map<std::string, std::unordered_set<std::string>>
ImpactAnalyzer::buildReverseIndex(const DependencyGraph& graph) {
    std::unordered_map<std::string, std::unordered_set<std::string>> reverse;
    for (const auto& node : graph.nodes) {
        reverse[node];
    }
    for (const auto& [from, tos] : graph.edges) {
        for (const auto& to : tos) {
            reverse[to].insert(from);
        }
    }
    return reverse;
}

std::unordered_set<std::string> ImpactAnalyzer::cycleMatesOf(
    const std::string& file,
    const std::vector<Cycle>& cycles) {
    std::unordered_set<std::string> mates;
    for (const auto& cycle : cycles) {
        bool contains = std::find(cycle.files.begin(), cycle.files.end(), file)
                         != cycle.files.end();
        if (!contains) {
            continue;
        }
        for (const auto& f : cycle.files) {
            if (f != file) {
                mates.insert(f);
            }
        }
    }
    return mates;
}

std::vector<ImpactedFile> ImpactAnalyzer::analyzeImpact(
    const std::string& targetFile,
    const DependencyGraph& graph,
    const std::vector<Cycle>& cycles) {
    auto reverseIndex = buildReverseIndex(graph);
    auto cycleMates = cycleMatesOf(targetFile, cycles);
    std::vector<ImpactedFile> result;
    std::unordered_set<std::string> visited{targetFile};
    std::queue<std::pair<std::string, int>> q;
    q.push({targetFile, 0});

    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();
        auto it = reverseIndex.find(current);
        if (it == reverseIndex.end()) {
            continue;
        }
        std::vector<std::string> importers(it->second.begin(), it->second.end());
        std::sort(importers.begin(), importers.end());
        for (const auto& importer : importers) {
            if (visited.count(importer)) {
                continue;
            }
            visited.insert(importer);
            result.push_back(ImpactedFile{
                importer,
                dist + 1,
                cycleMates.count(importer) > 0
            });
            q.push({importer, dist + 1});
        }
    }

    std::sort(result.begin(), result.end(),
        [](const ImpactedFile& a, const ImpactedFile& b) {
            if (a.distance != b.distance) return a.distance < b.distance;
            return a.file < b.file;
        });

    return result;
}

} // namespace knurl

