#pragma once
#include <string>
#include <unordered_map>

namespace knurl {

struct FileUsageIndex {
    // identifier -> minimum indentation column at which it was seen on a
    // non-import, non-docstring line. Absent from the map = never referenced
    // again anywhere in the file after its import.
    std::unordered_map<std::string, int> minIndentOf;
};

class UsageIndexer {
public:
    static FileUsageIndex buildIndex(const std::string& filePath);
};

}  // namespace knurl
