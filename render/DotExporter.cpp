#include "render/DotExporter.h"
#include <fstream>
#include <algorithm>
#include <unordered_map>

namespace knurl {
namespace {

// Light -> dark, 5 discrete buckets. Chosen over a continuous gradient so
// the file stays legible at a glance without a computed legend.
const char* RED_SHADES[5] = {
    "#fff5f5",  // near-zero risk
    "#feb2b2",
    "#fc8181",
    "#e53e3e",
    "#9b2c2c"   // highest risk in this repo
};

const char* TEST_FILL = "#e2e8f0";  // muted gray, matches RiskRanker's own
                                     // decision not to blend test scores
                                     // into the production scale

}  // namespace

std::string DotExporter::riskFillColor(double totalRisk, double maxProductionRisk) {
    if (maxProductionRisk <= 0.0) return RED_SHADES[0];
    double fraction = totalRisk / maxProductionRisk;
    int bucket = static_cast<int>(fraction * 5.0);
    if (bucket < 0) bucket = 0;
    if (bucket > 4) bucket = 4;
    return RED_SHADES[bucket];
}

bool DotExporter::isCycleClosingEdge(const std::string& from, const std::string& to,
                                      const std::vector<Cycle>& cycles) {
    for (const auto& cycle : cycles) {
        size_t n = cycle.files.size();
        for (size_t i = 0; i < n; ++i) {
            const std::string& a = cycle.files[i];
            const std::string& b = cycle.files[(i + 1) % n];
            if (a == from && b == to) return true;
        }
    }
    return false;
}

bool DotExporter::exportDot(const std::string& outPath,
                             const DependencyGraph& graph,
                             const std::vector<Cycle>& cycles,
                             const RiskRankingResult& ranked) {
    std::ofstream out(outPath);
    if (!out.is_open()) return false;

    std::unordered_map<std::string, double> riskOf;
    double maxProductionRisk = 0.0;
    for (const auto& r : ranked.production) {
        riskOf[r.file] = r.totalRisk;
        if (r.totalRisk > maxProductionRisk) maxProductionRisk = r.totalRisk;
    }

    out << "digraph knurl {\n";
    out << "    rankdir=LR;\n";
    out << "    node [shape=box, style=filled, fontname=\"Helvetica\"];\n\n";

    std::vector<std::string> nodes(graph.nodes.begin(), graph.nodes.end());
    std::sort(nodes.begin(), nodes.end());

    for (const auto& node : nodes) {
        std::string fill;
        if (RiskRanker::isTestFile(node)) {
            fill = TEST_FILL;
        } else {
            auto it = riskOf.find(node);
            double risk = (it != riskOf.end()) ? it->second : 0.0;
            fill = riskFillColor(risk, maxProductionRisk);
        }
        out << "    \"" << node << "\" [fillcolor=\"" << fill << "\"];\n";
    }
    out << "\n";

    std::vector<std::pair<std::string, std::string>> edges;
    for (const auto& [from, tos] : graph.edges) {
        for (const auto& to : tos) edges.emplace_back(from, to);
    }
    std::sort(edges.begin(), edges.end());

    for (const auto& [from, to] : edges) {
        if (isCycleClosingEdge(from, to, cycles)) {
            out << "    \"" << from << "\" -> \"" << to << "\" [color=red, style=dashed];\n";
        } else {
            out << "    \"" << from << "\" -> \"" << to << "\";\n";
        }
    }

    out << "}\n";
    return true;
}

}  // namespace knurl
