#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "graph/DependencyGraph.h"
#include "graph/CycleDetector.h"

namespace knurl {

enum class TreeMode { Forest, ImpactOnly, DependencyBoth };

class AsciiTreeRenderer {
public:
    // targetFile is ignored for TreeMode::Forest.
    // useColor is decided by the caller (e.g. isatty(stdout) in main.cpp) --
    // this class never queries the terminal itself, so it stays testable
    // without a real tty.
    static std::string render(TreeMode mode,
                               const DependencyGraph& graph,
                               const std::string& targetFile,
                               bool useColor);

private:
    static std::vector<std::string> findRoots(const DependencyGraph& graph);

    static std::unordered_map<std::string, std::unordered_set<std::string>>
        buildReverseIndex(const DependencyGraph& graph);

    static std::string colorFor(int distance, bool isReverseDirection, bool useColor);

    // Recursive box-drawing walker. Appends one line per child to `out`,
    // recursing into unvisited children and stopping (with a
    // "[cycle, see above]" stub) at anything already in `renderedOnce`.
    static void walk(const std::string& node,
                      const std::string& prefix,
                      int distance,
                      bool isReverseDirection,
                      const std::unordered_map<std::string, std::unordered_set<std::string>>& adjacency,
                      std::unordered_set<std::string>& renderedOnce,
                      std::vector<std::string>& out,
                      bool useColor);
};

}  // namespace knurl
