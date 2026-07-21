#include "render/AsciiTreeRenderer.h"
#include <algorithm>

namespace knurl {
namespace {

const char* BOLD  = "\033[1m";
const char* RESET = "\033[0m";

// distance 1 = brightest, 2 = normal, 3+ = dim -- GitHub-activity-graph style
const char* GREEN_BRIGHT = "\033[1;32m";
const char* GREEN_NORMAL = "\033[32m";
const char* GREEN_DIM    = "\033[2;32m";
const char* RED_BRIGHT   = "\033[1;31m";
const char* RED_NORMAL   = "\033[31m";
const char* RED_DIM      = "\033[2;31m";

}  // namespace

std::vector<std::string> AsciiTreeRenderer::findRoots(const DependencyGraph& graph) {
    std::unordered_set<std::string> hasIncoming;
    for (const auto& [from, tos] : graph.edges) {
        for (const auto& to : tos) hasIncoming.insert(to);
    }

    std::vector<std::string> roots;
    for (const auto& node : graph.nodes) {
        if (!hasIncoming.count(node)) roots.push_back(node);
    }
    std::sort(roots.begin(), roots.end());
    return roots;
}

std::unordered_map<std::string, std::unordered_set<std::string>>
AsciiTreeRenderer::buildReverseIndex(const DependencyGraph& graph) {
    std::unordered_map<std::string, std::unordered_set<std::string>> reverse;
    for (const auto& [from, tos] : graph.edges) {
        for (const auto& to : tos) reverse[to].insert(from);
    }
    return reverse;
}

std::string AsciiTreeRenderer::colorFor(int distance, bool isReverseDirection, bool useColor) {
    if (!useColor) return "";
    if (isReverseDirection) {
        if (distance <= 1) return RED_BRIGHT;
        if (distance == 2) return RED_NORMAL;
        return RED_DIM;
    }
    if (distance <= 1) return GREEN_BRIGHT;
    if (distance == 2) return GREEN_NORMAL;
    return GREEN_DIM;
}

void AsciiTreeRenderer::walk(
    const std::string& node,
    const std::string& prefix,
    int distance,
    bool isReverseDirection,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& adjacency,
    std::unordered_set<std::string>& renderedOnce,
    std::vector<std::string>& out,
    bool useColor) {

    auto it = adjacency.find(node);
    if (it == adjacency.end()) return;

    std::vector<std::string> children(it->second.begin(), it->second.end());
    std::sort(children.begin(), children.end());

    for (size_t i = 0; i < children.size(); ++i) {
        bool isLast = (i == children.size() - 1);
        const std::string& child = children[i];

        std::string branch = isLast ? "└── " : "├── ";
        std::string childPrefix = prefix + (isLast ? "    " : "│   ");
        std::string color = colorFor(distance, isReverseDirection, useColor);
        std::string reset = useColor ? RESET : "";

        std::string label = prefix + branch + color + child + reset
                           + " (" + std::to_string(distance) + ")";

        if (renderedOnce.count(child)) {
            out.push_back(label + " [cycle, see above]");
            continue;  // do not recurse into an already-rendered node
        }

        renderedOnce.insert(child);
        out.push_back(label);
        walk(child, childPrefix, distance + 1, isReverseDirection, adjacency, renderedOnce, out, useColor);
    }
}

std::string AsciiTreeRenderer::render(TreeMode mode,
                                       const DependencyGraph& graph,
                                       const std::string& targetFile,
                                       bool useColor) {
    std::vector<std::string> out;
    std::unordered_set<std::string> renderedOnce;
    std::string bold = useColor ? BOLD : "";
    std::string reset = useColor ? RESET : "";

    if (mode == TreeMode::Forest) {
        auto roots = findRoots(graph);
        for (const auto& root : roots) {
            if (renderedOnce.count(root)) continue;
            if (!out.empty()) out.push_back("");  // blank line between top-level trees
            out.push_back(bold + root + reset);
            renderedOnce.insert(root);
            walk(root, "", 1, /*isReverseDirection=*/false, graph.edges, renderedOnce, out, useColor);
        }

        // Leftover nodes belong to cycles with no external importer (e.g. a
        // self-contained ring like cyclic3/x<->y<->z) -- they'd never
        // qualify as a zero-indegree root above and would silently vanish
        // from the forest without this pass. Pick the lexicographically
        // smallest unvisited node as a synthetic root for each such group.
        std::vector<std::string> allNodes(graph.nodes.begin(), graph.nodes.end());
        std::sort(allNodes.begin(), allNodes.end());
        for (const auto& node : allNodes) {
            if (renderedOnce.count(node)) continue;
            if (!out.empty()) out.push_back("");
            out.push_back(bold + node + reset + " [cycle root]");
            renderedOnce.insert(node);
            walk(node, "", 1, false, graph.edges, renderedOnce, out, useColor);
        }
    } else if (mode == TreeMode::ImpactOnly) {
        auto reverseIndex = buildReverseIndex(graph);
        out.push_back(bold + targetFile + reset + " [target]");
        renderedOnce.insert(targetFile);
        walk(targetFile, "", 1, /*isReverseDirection=*/true, reverseIndex, renderedOnce, out, useColor);
    } else {  // DependencyBoth
        auto reverseIndex = buildReverseIndex(graph);
        out.push_back(bold + targetFile + reset + " [target]");
        renderedOnce.insert(targetFile);

        out.push_back("├── Imports");
        walk(targetFile, "│   ", 1, /*isReverseDirection=*/false, graph.edges, renderedOnce, out, useColor);

        out.push_back("└── Imported by");
        walk(targetFile, "    ", 1, /*isReverseDirection=*/true, reverseIndex, renderedOnce, out, useColor);
    }

    std::string result;
    for (const auto& line : out) {
        result += line + "\n";
    }
    return result;
}

}  // namespace knurl
