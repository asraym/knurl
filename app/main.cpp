#include <iostream>
#include <iomanip>
#include "app/CliOptions.h"
#include "app/Engine.h"
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
    std::cout << "=== Cycles ===\n";
    if (result.cycles.empty()) {
        std::cout << "  none found\n";
    } else {
        for (const auto& c : result.cycles) {
            std::cout << "  ";
            for (size_t i = 0; i < c.files.size(); ++i) {
                std::cout << c.files[i] << " -> ";
            }
            std::cout << c.files.front() << " (closes loop)\n";
        }
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
