#include <iostream>

#include "scanning/FileWalker.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage:knurl <path-to-python-repo>\n";
        return 1;
    }

    std::string rootDir = argv[1];
    auto files = knurl::FileWalker::findPythonFiles(rootDir);

    std::cout << "Found " << files.size() << " Python Files:\n";
    for(const auto& f: files){
        std::cout << " " << f << "\n";
    }

    return 0;
}
