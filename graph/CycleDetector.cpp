#include "graph/CycleDetector.h"

#include <algorithm>
#include <set>
#include <unordered_map>

namespace knurl {

std::vector<std::string> CycleDetector::canonicalize(std::vector<std::string> cycle){
    //rotate so that the lexicographically-smallest element is  first, so that the same cycle
    //found from different DFS start points compares equal.
    auto minIt = std::min_element(cycle.begin(), cycle.end());
    std::rotate(cycle.begin(), minIt, cycle.end());
    return cycle;
}

void CycleDetector::dfs(const std::string& node, const DependencyGraph& graph,
                        std::unordered_map<std::string, Color>& color,
                        std::vector<std::string>& pathStack,
                        std::vector<std::vector<std::string>>& rawCycles) {
    color[node] = Color::Gray;
    pathStack.push_back(node);

    auto edgeIt = graph.edges.find(node);
    if(edgeIt != graph.edges.end()){
        std::vector<std::string> neighbors(edgeIt->second.begin(), edgeIt->second.end());
        std::sort(neighbors.begin(), neighbors.end());

        for(const auto& neighbor: neighbors){
            auto it = color.find(neighbor);
            Color neighborColor = (it == color.end()) ? Color::White : it->second;

            if(neighborColor == Color::White){
                dfs(neighbor, graph, color, pathStack, rawCycles);
            }else if(neighborColor == Color::Gray){
                auto startIt = std::find(pathStack.begin(), pathStack.end(), neighbor);
                std::vector<std::string> cycle(startIt, pathStack.end());
                rawCycles.push_back(cycle);
            }
        }
    }

    pathStack.pop_back();
    color[node] = Color::Black;
}

std::vector<Cycle> CycleDetector::findCycles(const DependencyGraph& graph){
    std::vector<std::string> sortedNodes(graph.nodes.begin(), graph.nodes.end());
    std::sort(sortedNodes.begin(), sortedNodes.end());

    std::unordered_map<std::string, Color> color;
    std::vector<std::string> pathStack;
    std::vector<std::vector<std::string>> rawCycles;

    for(const auto& node: sortedNodes){
        if(color.find(node) == color.end()){
            dfs(node, graph, color, pathStack, rawCycles);
        }
    }

    // Canonicalize + dedupe.
    std::set<std::vector<std::string>> uniqueCycles;
    for(auto& c: rawCycles){
        uniqueCycles.insert(canonicalize(std::move(c)));
    }

    std::vector<Cycle> result;
    result.reserve(uniqueCycles.size());
    for(const auto& c: uniqueCycles){
        result.push_back(Cycle{c});
    }

    std::sort(result.begin(), result.end(), [](const Cycle& a, const Cycle& b){
            return a.files < b.files ; });

    return result;
}

} // namespace knurl





