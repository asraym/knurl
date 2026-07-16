#include "scanning/LocalBindingExtractor.h"
#include <fstream>
#include <sstream>

namespace knurl {
namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

// "numpy as np" -> localName "np"; "os" -> localName "os"
// "pkg.sub.util as psu" -> "psu"; "pkg.sub.util" -> "pkg" (first segment)
std::string bindingFromToken(const std::string& rawToken, bool isPlainImport) {
    std::string tok = trim(rawToken);
    size_t asPos = tok.find(" as ");
    if (asPos != std::string::npos) {
        return trim(tok.substr(asPos + 4));
    }
    if (isPlainImport) {
        size_t dot = tok.find('.');
        return dot == std::string::npos ? tok : tok.substr(0, dot);
    }
    return tok;  // from-import name, no dots possible here
}

std::vector<std::string> splitComma(const std::string& s) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        std::string t = trim(item);
        if (!t.empty()) parts.push_back(t);
    }
    return parts;
}

int countLeadingDots(const std::string& s, size_t& consumedLen) {
    int dots = 0;
    size_t i = 0;
    while (i < s.size() && s[i] == '.') { dots++; i++; }
    consumedLen = i;
    return dots;
}

}  // namespace

std::vector<LocalBinding> LocalBindingExtractor::extractBindings(const std::string& filePath) {
    std::vector<LocalBinding> result;
    std::ifstream in(filePath);
    if (!in.is_open()) return result;

    bool insideDocstring = false;
    bool insideParenImport = false;
    bool continuationIsSiblingNames = false;
    int continuationDots = 0;
    std::string continuationModule;  

    std::string line;
    while (std::getline(in, line)) {
        std::string work = trim(line);
        if (work.empty()) continue;

        size_t tripleCount = 0;
        for (size_t i = 0; i + 2 < work.size(); ++i) {
            if ((work[i] == '"' && work[i+1] == '"' && work[i+2] == '"') ||
                (work[i] == '\'' && work[i+1] == '\'' && work[i+2] == '\'')) {
                tripleCount++;
                i += 2;
            }
        }
        if (tripleCount % 2 == 1) {
            insideDocstring = !insideDocstring;
            continue;
        }
        if (insideDocstring) continue;

        if (insideParenImport) {
            bool closesHere = work.find(')') != std::string::npos;
            std::string content = work.substr(0, work.find(')'));
            for (const auto& tok : splitComma(content)) {
                std::string bound = bindingFromToken(tok, /*isPlainImport=*/false);
                std::string moduleName = continuationIsSiblingNames
                    ? trim(tok.substr(0, tok.find(" as ")))  // token IS the module, alias-stripped
                    : continuationModule;                     // shared dotted parent, set once
                if (!bound.empty()) {
                    result.push_back({filePath, moduleName, bound, continuationDots});
                }
            }
            if (closesHere) insideParenImport = false;
            continue;
        }

        if (work[0] == '#') continue;
        size_t hashPos = work.find('#');
        if (hashPos != std::string::npos) work = trim(work.substr(0, hashPos));

        if (work.rfind("import ", 0) == 0) {
            std::string rest = work.substr(7);
            for (const auto& tok : splitComma(rest)) {
                std::string bound = bindingFromToken(tok, /*isPlainImport=*/true);
                std::string moduleOnly = tok.substr(0, tok.find(" as "));
                if (!bound.empty()) {
                    result.push_back({filePath, trim(moduleOnly), bound, 0});
                }
            }
            continue;
        }

        if (work.rfind("from ", 0) == 0) {
            size_t importPos = work.find(" import ");
            if (importPos == std::string::npos) continue;
            std::string modulePart = trim(work.substr(5, importPos - 5));
            std::string afterImport = trim(work.substr(importPos + 8));

            size_t dotsLen;
            int dotCount = countLeadingDots(modulePart, dotsLen);
            std::string syntax = trim(modulePart.substr(dotsLen));

            bool opensParen = afterImport.find('(') != std::string::npos;
            std::string names = afterImport;
            if (opensParen) {
                names = names.substr(names.find('(') + 1);
            }
            size_t closeParen = names.find(')');
            bool closesOnThisLine = !opensParen || closeParen != std::string::npos;
            std::string namesOnly = closesOnThisLine && opensParen ? names.substr(0, closeParen) : names;

            if (!syntax.empty()) {
                // dotted from-import: names after import are discarded symbols,
                // but each still creates its own local binding (or its alias)
                for (const auto& tok : splitComma(namesOnly)) {
                    std::string bound = bindingFromToken(tok, /*isPlainImport=*/false);
                    if (!bound.empty()) {
                        result.push_back({filePath, syntax, bound, dotCount});
                    }
                }
            } else {
                // pure-dot: names ARE the modules, binding == alias or the name itself
                for (const auto& tok : splitComma(namesOnly)) {
                    std::string bound = bindingFromToken(tok, /*isPlainImport=*/false);
                    std::string moduleName = trim(tok.substr(0, tok.find(" as ")));
                    if (!bound.empty()) {
                        result.push_back({filePath, moduleName, bound, dotCount});
                    }
                }
            }

            if (opensParen && !closesOnThisLine) {
                insideParenImport = true;
                continuationDots = dotCount;
                continuationIsSiblingNames = syntax.empty();
                continuationModule = syntax;  // used only when continuationIsSiblingNames is false
            }
        }
    }

    return result;
}

}  // namespace knurl
