#include "graph/DependencyGraph.h"
#include "scanning/FileWalker.h"
#include "scanning/PyImportExtractor.h"

namespace knurl {

void DependencyGraph::addNode(const std::string& file) { nodes.insert(file); }
void DependencyGraph::addEdge(const std::string& from, const std::string& to) { edges[from].insert(to); }
void DependencyGraph::addExternal(const std::string& from, const std::string& moduleName) {
    externals[from].insert(moduleName);
}

DependencyGraph DependencyGraph::build(const std::string& rootDir) {
    DependencyGraph graph;

    std::vector<std::string> files = FileWalker::findPythonFiles(rootDir);
    auto moduleMap = ModuleResolver::buildModuleMap(files, rootDir);

    for (const auto& file : files) {
        graph.addNode(file);
        std::vector<RawImport> imports = PyImportExtractor::extractImports(file);

        for (const auto& imp : imports) {
            std::string resolved = ModuleResolver::resolve(imp, rootDir, moduleMap);
            if (!resolved.empty()) {
                graph.addEdge(file, resolved);
            } else {
                graph.addExternal(file, imp.importedModule);
            }
        }
    }

    return graph;
}

} // namespace knurl
