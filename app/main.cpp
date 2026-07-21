#include <iostream>
#include <iomanip>
#include <unistd.h>
#include "app/CliOptions.h"
#include "app/Engine.h"
#include "render/AsciiTreeRenderer.h"
#include "render/DotExporter.h"
namespace {
std::string tierName(knurl::UsageTier t) {
    switch (t) {
        case knurl::UsageTier::Tier1_TopLevelExecution: return "Tier1";
        case knurl::UsageTier::Tier2_ReachableOnly:      return "Tier2";
        case knurl::UsageTier::Tier3_Unused:             return "Tier3";
    }
    return "?";
}

void printRiskList(const std::vector<knurl::FileRisk>& list, int topK) {
    if (list.empty()) {
        std::cout << "  none\n";
        return;
    }
    int shown = 0;
    for (const auto& r : list) {
        if (shown >= topK) break;
        std::cout << "  " << std::left << std::setw(30) << r.file
                  << " structural=" << std::fixed << std::setprecision(3) << r.structuralRisk
                  << "  cycleBonus=" << r.cycleBonus
                  << "  total=" << r.totalRisk << "\n";
        shown++;
    }
}

void printCycles(const std::vector<knurl::Cycle>& cycles) {
    std::cout << "=== Cycles ===\n";
    if (cycles.empty()) {
        std::cout << "  none found\n";
        return;
    }
    for (const auto& c : cycles) {
        std::cout << "  ";
        for (size_t i = 0; i < c.files.size(); ++i) {
            std::cout << c.files[i] << " -> ";
        }
        std::cout << c.files.front() << " (closes loop)\n";
    }
}
}
int main(int argc, char** argv) {
    knurl::CliOptions options;
    std::string error;
    if (!knurl::CliOptionsParser::parse(argc, argv, options, error)) {
        std::cerr << "Error: " << error << "\n\n";
        knurl::CliOptionsParser::printUsage(argv[0]);
        return 1;
    }
    auto result = knurl::Engine::run(options);
    std::cout << "=== knurl: " << options.rootDir << " ===\n";
    std::cout << "Files scanned: " << result.graph.nodes.size() << "\n\n";

    if (options.hasDot()) {
        bool ok = knurl::DotExporter::exportDot(options.dotOutPath, result.graph, result.cycles, result.ranked);
        if (ok) {
            std::cout << "Wrote dependency graph to " << options.dotOutPath << "\n\n";
        } else {
            std::cerr << "Error: could not write .dot file to " << options.dotOutPath << "\n\n";
        }
    }

    printCycles(result.cycles);

    if (options.hasTreeMode()) {
        // A tree view replaces the flat ranking/impact listings below --
        // it's a different presentation of the same underlying data, not
        // an addition to it.
        bool useColor = isatty(fileno(stdout));
        knurl::TreeMode mode = options.ftree   ? knurl::TreeMode::Forest
                              : options.itree   ? knurl::TreeMode::ImpactOnly
                                                  : knurl::TreeMode::DependencyBoth;
        std::cout << "\n=== "
                  << (options.ftree ? "Repo Tree" : options.itree ? "Impact Tree" : "Dependency Tree")
                  << " ===\n";
        std::cout << knurl::AsciiTreeRenderer::render(mode, result.graph, options.targetFile, useColor);
        return 0;
    }

    std::cout << "\n=== Risk Ranking — Production (top " << options.topK << ") ===\n";
    printRiskList(result.ranked.production, options.topK);

    std::cout << "\n=== Risk Ranking — Tests (top " << options.topK << ") ===\n";
    printRiskList(result.ranked.tests, options.topK);

    if (result.targetQueried) {
        std::cout << "\n=== Impact Analysis (target: " << options.targetFile << ") ===\n";
        if (result.impact.empty()) {
            std::cout << "  nothing depends on this file\n";
        } else {
            for (const auto& f : result.impact) {
                std::cout << "  " << std::left << std::setw(30) << f.file
                          << " distance=" << f.distance
                          << "  inCycleWithQuery=" << (f.inCycleWithQuery ? "true" : "false") << "\n";
            }
        }
    }
    return 0;
}
