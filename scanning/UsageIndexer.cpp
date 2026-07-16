#include "scanning/UsageIndexer.h"
#include <fstream>
#include <cctype>

namespace knurl {
namespace {

int leadingIndent(const std::string& raw) {
    int n = 0;
    for (char c : raw) {
        if (c == ' ' || c == '\t') n++;
        else break;
    }
    return n;
}

std::string trimLeft(const std::string& s) {
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    return s.substr(i);
}

bool isIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

void collectIdentifiers(const std::string& line, int indent, FileUsageIndex& index) {
    size_t i = 0;
    while (i < line.size()) {
        if (isIdentChar(line[i]) && !std::isdigit(static_cast<unsigned char>(line[i]))) {
            size_t start = i;
            while (i < line.size() && isIdentChar(line[i])) i++;
            std::string tok = line.substr(start, i - start);
            auto it = index.minIndentOf.find(tok);
            if (it == index.minIndentOf.end() || indent < it->second) {
                index.minIndentOf[tok] = indent;
            }
        } else {
            i++;
        }
    }
}

}  // namespace

FileUsageIndex UsageIndexer::buildIndex(const std::string& filePath) {
    FileUsageIndex index;
    std::ifstream in(filePath);
    if (!in.is_open()) return index;

    bool insideDocstring = false;
    bool insideParenImport = false;

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = trimLeft(line);
        if (trimmed.empty()) continue;

        size_t tripleCount = 0;
        for (size_t i = 0; i + 2 < trimmed.size(); ++i) {
            if ((trimmed[i] == '"' && trimmed[i+1] == '"' && trimmed[i+2] == '"') ||
                (trimmed[i] == '\'' && trimmed[i+1] == '\'' && trimmed[i+2] == '\'')) {
                tripleCount++;
                i += 2;
            }
        }
        if (tripleCount % 2 == 1) {
            insideDocstring = !insideDocstring;
            continue;
        }
        if (insideDocstring) continue;

        std::string codeOnly = trimmed;
        if (codeOnly[0] == '#') continue;
        size_t hashPos = codeOnly.find('#');
        if (hashPos != std::string::npos) codeOnly = codeOnly.substr(0, hashPos);

        // Import lines (and their paren continuations) are skipped entirely --
        // a name's own import line/binding isn't "usage" of that name.
        if (insideParenImport) {
            if (codeOnly.find(')') != std::string::npos) insideParenImport = false;
            continue;
        }
        bool isImportLine = codeOnly.rfind("import ", 0) == 0 || codeOnly.rfind("from ", 0) == 0;
        if (isImportLine) {
            bool opensParen = codeOnly.find('(') != std::string::npos;
            bool closesParen = codeOnly.find(')') != std::string::npos;
            if (opensParen && !closesParen) insideParenImport = true;
            continue;
        }

        int indent = leadingIndent(line);
        collectIdentifiers(codeOnly, indent, index);
    }

    return index;
}

}  // namespace knurl
