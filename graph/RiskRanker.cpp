// graph/RiskRanker.cpp
#include "graph/RiskRanker.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

namespace knurl {
namespace {

std::unordered_map<std::string, std::unordered_map<std::string, UsageTier>>
buildTierLookup(const std::vector<TieredEdge>& tieredEdges) {
    std::unordered_map<std::string, std::unordered_map<std::string, UsageTier>> lookup;
    for (const auto& e : tieredEdges) lookup[e.from][e.to] = e.tier;
    return lookup;
}

std::unordered_map<std::string, std::unordered_set<std::string>>
buildReverseIndex(const DependencyGraph& graph) {
    std::unordered_map<std::string, std::unordered_set<std::string>> reverse;
    for (const auto& [from, tos] : graph.edges) {
        for (const auto& to : tos) reverse[to].insert(from);
    }
    return reverse;
}

UsageTier tierOfEdge(
    const std::string& from, const std::string& to,
    const std::unordered_map<std::string, std::unordered_map<std::string, UsageTier>>& tierLookup) {
    auto fromIt = tierLookup.find(from);
    if (fromIt == tierLookup.end()) return UsageTier::Tier3_Unused;
    auto toIt = fromIt->second.find(to);
    if (toIt == fromIt->second.end()) return UsageTier::Tier3_Unused;
    return toIt->second;
}

struct QueueItem { std::string node; double severity; };

}  // namespace

double RiskRanker::propagationWeight(UsageTier tier) {
    switch (tier) {
        case UsageTier::Tier1_TopLevelExecution: return 1.0;
        case UsageTier::Tier2_ReachableOnly:      return 0.4;
        case UsageTier::Tier3_Unused:             return 0.05;
    }
    return 0.05;
}

double RiskRanker::structuralRiskOf(
    const std::string& target,
    const DependencyGraph& graph,
    const std::unordered_map<std::string, std::unordered_map<std::string, UsageTier>>& tierLookup) {

    auto reverseIndex = buildReverseIndex(graph);
    std::unordered_set<std::string> visited;
    visited.insert(target);
    std::queue<QueueItem> q;

    auto seed = reverseIndex.find(target);
    if (seed != reverseIndex.end()) {
        std::vector<std::string> importers(seed->second.begin(), seed->second.end());
        std::sort(importers.begin(), importers.end());
        for (const auto& importer : importers) {
            visited.insert(importer);
            double sev = propagationWeight(tierOfEdge(importer, target, tierLookup));
            q.push({importer, sev});
        }
    }

    double total = 0.0;
    while (!q.empty()) {
        QueueItem item = q.front();
        q.pop();
        total += item.severity;

        auto nIt = reverseIndex.find(item.node);
        if (nIt == reverseIndex.end()) continue;

        std::vector<std::string> importers(nIt->second.begin(), nIt->second.end());
        std::sort(importers.begin(), importers.end());
        for (const auto& importer : importers) {
            if (visited.count(importer)) continue;
            visited.insert(importer);
            double nextSeverity = item.severity * propagationWeight(tierOfEdge(importer, item.node, tierLookup));
            q.push({importer, nextSeverity});
        }
    }

    return total;
}

int RiskRanker::cycleMateCountOf(const std::string& target, const std::vector<Cycle>& cycles) {
    std::unordered_set<std::string> mates;
    for (const auto& c : cycles) {
        bool inThisCycle = std::find(c.files.begin(), c.files.end(), target) != c.files.end();
        if (!inThisCycle) continue;
        for (const auto& f : c.files) if (f != target) mates.insert(f);
    }
    return static_cast<int>(mates.size());
}

std::vector<FileRisk> RiskRanker::rankRisk(const DependencyGraph& graph,
                                            const std::vector<Cycle>& cycles,
                                            const std::vector<TieredEdge>& tieredEdges,
                                            double cycleBonusWeight) {
    auto tierLookup = buildTierLookup(tieredEdges);
    std::vector<std::string> allFiles(graph.nodes.begin(), graph.nodes.end());
    std::sort(allFiles.begin(), allFiles.end());

    std::vector<FileRisk> results;
    for (const auto& file : allFiles) {
        double structural = structuralRiskOf(file, graph, tierLookup);
        int mateCount = cycleMateCountOf(file, cycles);
        double bonus = cycleBonusWeight * mateCount;
        results.push_back({file, structural, bonus, structural + bonus});
    }

    std::sort(results.begin(), results.end(), [](const FileRisk& a, const FileRisk& b) {
        if (a.totalRisk != b.totalRisk) return a.totalRisk > b.totalRisk;
        return a.file < b.file;
    });

    return results;
}

}  // namespace knurl
