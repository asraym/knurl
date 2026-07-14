#include "PyImportExtractor.h"

#include <algorithm>
#include <fstream>

namespace knurl {

namespace {

// --- small string helpers -----------------------------------------

std::string trim(const std::string& s){
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos){
        return "";
    }
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

// ts for removing everything from the first '#' onward. 
std::string stripInlineComment(const std::string& line){
    size_t hashPos = line.find('#');
    if(hashPos == std::string::npos){
        return line;
    }
    return line.substr(0, hashPos);
}

// counting the occurrences of """ or ''' in a line.
int countMarkers(const std::string& line, const std::string& marker){
    int count = 0;
    size_t pos = 0;
    while((pos = line.find(marker, pos)) != std::string::npos){
        ++count;
        pos+=marker.size();
    }
    return count;
}

// for removing '(' and ')' entirely. 
std::string stripParens(const std::string& s){
    std::string result;
    result.reserve(s.size());
    for(char c: s){
        if(c != '(' && c != ')'){
            result += c;
        }
    }
    return result;
}

//splits on ',' and trims each piece
std::vector<std::string> splitOnComma(const std::string& s){
    std::vector<std::string> pieces;
    size_t start = 0;
    while(start <= s.size()){
        size_t comma = s.find(',', start);
        std::string piece = (comma == std::string::npos)
            ? s.substr(start)
            : s.substr(start, comma - start);
        piece = trim(piece);
        if(!(piece.empty())){
            pieces.push_back(piece);
        }
        if(comma == std::string::npos){
            break;
        }
        start = comma + 1;
    }
    return pieces;
}

// cuts off " as <alias>" if present
std::string stripAsAlias(const std::string& s){
    size_t asPos = s.find(" as ");
    if(asPos == std::string::npos){
        return s;
    }
    return trim(s.substr(0, asPos));
}

// for counting dots in the string. 
std::pair<int, std::string> splitLeadingDots(const std::string& s){
    int dotCount = 0;
    while(dotCount < static_cast<int>(s.size()) && s[dotCount] == '.'){
        ++dotCount;
    }
    return {dotCount, s.substr(dotCount)};
}

} // namespace

std::vector<RawImport> PyImportExtractor::extractImports(const std::string& sourceFile){
    std::vector<RawImport> result;
    std::ifstream file(sourceFile);
    if(!file.is_open()){
        return result;
    }

    bool insideDocstring = false;

    //Multi-line "from X import (...)" state.
    bool insideParenImport = false;
    bool continuationCapturesNames = false;
    int continuationDots = 0;

    std::string rawLine;
    while(std::getline(file, rawLine)){

            // --- 1. Continuing a multi-line parenthesized import? ---
            if(insideParenImport){
                std::string cleaned = trim(stripInlineComment(rawLine));
                bool hasClose = cleaned.find(')') != std::string::npos;
                std::string namesOnly = stripParens(cleaned);

                if(continuationCapturesNames){
                    for(const auto& name : splitOnComma(namesOnly)){
                        result.push_back({sourceFile, name, continuationDots});
                    }
                }

                if(hasClose){
                    insideParenImport = false;
                    continuationCapturesNames = false;
                }
                continue;
            }

            // --- 2. Docstring tracking ---
            int markerCount = countMarkers(rawLine, "\"\"\"") + countMarkers(rawLine, "'''");
            if(markerCount % 2 == 1){
                insideDocstring = !insideDocstring;
                continue;
            }
            if(insideDocstring){
                continue;
            }

            // --- 3. Cleaning the line ---
            std::string line = trim(rawLine);
            if(line.empty()){
                continue;
            }
            if(line[0] == '#'){
                continue;
            }
            line = trim(stripInlineComment(line));
            if(line.empty()){
                continue;
            }

            // --- 4. Match "import ..." ---
            if(line.rfind("import ", 0) == 0){
                std::string remainder = line.substr(7);
                for(const auto& token: splitOnComma(remainder)){
                    std::string moduleName = trim(stripAsAlias(token));
                    if(!moduleName.empty()){
                        result.push_back({sourceFile, moduleName, 0});
                    }
                }
                continue;
            }

            // --- 5. Match "from ... import ..." ---
            if(line.rfind("from", 0) == 0){
                size_t importPos = line.find(" import ");
                if(importPos == std::string::npos){
                    continue;
                }

                std::string modulePart = line.substr(5, importPos - 5);
                auto [dotCount, syntax] = splitLeadingDots(trim(modulePart));

                std::string namesPart = line.substr(importPos + 8);
                long openCount = std::count(namesPart.begin(), namesPart.end(), '(');
                long closeCount = std::count(namesPart.begin(), namesPart.end(), ')');
                bool startsMultiline = openCount > closeCount;

                if(!syntax.empty()){
                    //Dotted module case 
                    result.push_back({sourceFile, syntax, dotCount});
                    if(startsMultiline){
                        insideParenImport = true;
                        continuationCapturesNames = false;
                    }
                } else{
                    //Pure dot case 
                    std::string cleanNames = stripParens(namesPart);
                    for(const auto& name: splitOnComma(cleanNames)){
                        result.push_back({sourceFile, name, dotCount});
                    }
                    if(startsMultiline){
                        insideParenImport = true;
                        continuationCapturesNames = true;
                        continuationDots = dotCount;
                    }
                }
            }
    }

    return result;
 }

 } // namespace knurl



