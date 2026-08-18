#include "AnalyticalEngine.h"
#include "Parser.h"
#include "Circuit.h"
#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace dls {

// -----------------------------------------------------------------------------
// Helper: Build K-Map Grid (Supports 1 to 4 variables)
// -----------------------------------------------------------------------------
static std::vector<std::vector<KMapCell>> buildKMapGrid(size_t numVars, const std::vector<TruthRow>& truthTable) {
    std::vector<std::vector<KMapCell>> kmap;
    if (numVars == 0 || truthTable.empty()) return kmap;

    std::vector<int> gray1 = {0, 1};
    std::vector<int> gray2 = {0, 1, 3, 2}; // Gray code sequence: 00, 01, 11, 10

    size_t rows = 0, cols = 0;
    std::vector<int> rowGray, colGray;

    if (numVars == 1) {
        rows = 1; cols = 2;
        rowGray = {0};
        colGray = gray1;
    } else if (numVars == 2) {
        rows = 2; cols = 2;
        rowGray = gray1;
        colGray = gray1;
    } else if (numVars == 3) {
        rows = 2; cols = 4;
        rowGray = gray1;
        colGray = gray2;
    } else if (numVars == 4) {
        rows = 4; cols = 4;
        rowGray = gray2;
        colGray = gray2;
    } else {
        return kmap;
    }

    kmap.resize(rows, std::vector<KMapCell>(cols));

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            int mintermIndex = 0;
            if (numVars == 1) {
                mintermIndex = colGray[c];
            } else if (numVars == 2) {
                mintermIndex = (rowGray[r] << 1) | colGray[c];
            } else if (numVars == 3) {
                mintermIndex = (rowGray[r] << 2) | colGray[c];
            } else if (numVars == 4) {
                mintermIndex = (rowGray[r] << 2) | colGray[c];
            }

            bool val = false;
            if (mintermIndex >= 0 && mintermIndex < static_cast<int>(truthTable.size())) {
                val = truthTable[mintermIndex].output;
            }

            kmap[r][c] = KMapCell{static_cast<int>(r), static_cast<int>(c), mintermIndex, val};
        }
    }

    return kmap;
}

// -----------------------------------------------------------------------------
// Helper: Formatter for Canonical Expressions
// -----------------------------------------------------------------------------
static std::string generateCanonicalSOP(const std::vector<TruthRow>& table, const std::vector<std::string>& vars) {
    std::stringstream ss;
    bool first = true;

    for (size_t r = 0; r < table.size(); ++r) {
        if (table[r].output) {
            if (!first) ss << " + ";
            ss << "(";
            for (size_t i = 0; i < vars.size(); ++i) {
                if (i > 0) ss << "&";
                if (!table[r].inputs[i]) ss << "!";
                ss << vars[i];
            }
            ss << ")";
            first = false;
        }
    }
    return first ? "0" : ss.str();
}

static std::string generateCanonicalPOS(const std::vector<TruthRow>& table, const std::vector<std::string>& vars) {
    std::stringstream ss;
    bool first = true;

    for (size_t r = 0; r < table.size(); ++r) {
        if (!table[r].output) {
            if (!first) ss << " . ";
            ss << "(";
            for (size_t i = 0; i < vars.size(); ++i) {
                if (i > 0) ss << "|";
                if (table[r].inputs[i]) ss << "!";
                ss << vars[i];
            }
            ss << ")";
            first = false;
        }
    }
    return first ? "1" : ss.str();
}

// -----------------------------------------------------------------------------
// Helper: Quine-McCluskey Minimization Algorithm
// -----------------------------------------------------------------------------
struct Implicant {
    std::string mask;
    std::vector<int> minterms;
    bool used = false;
};

static bool canCombine(const std::string& a, const std::string& b, std::string& result) {
    int diff = 0;
    result = a;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            diff++;
            result[i] = '-';
        }
    }
    return diff == 1;
}

static std::pair<std::string, std::vector<Grouping>> simplifyBooleanExpression(
    const std::vector<int>& minterms,
    const std::vector<std::string>& vars) 
{
    if (minterms.empty()) {
        return {"0", {}};
    }
    if (minterms.size() == (1ULL << vars.size())) {
        return {"1", {}};
    }

    int nVars = static_cast<int>(vars.size());
    std::vector<Implicant> currentLevel;

    for (int m : minterms) {
        std::string mask = "";
        for (int i = nVars - 1; i >= 0; --i) {
            mask += ((m >> i) & 1) ? '1' : '0';
        }
        currentLevel.push_back({mask, {m}, false});
    }

    std::vector<Implicant> primeImplicants;

    while (!currentLevel.empty()) {
        std::vector<Implicant> nextLevel;
        std::set<std::string> addedMasks;

        for (size_t i = 0; i < currentLevel.size(); ++i) {
            for (size_t j = i + 1; j < currentLevel.size(); ++j) {
                std::string combined;
                if (canCombine(currentLevel[i].mask, currentLevel[j].mask, combined)) {
                    currentLevel[i].used = true;
                    currentLevel[j].used = true;

                    if (addedMasks.find(combined) == addedMasks.end()) {
                        std::vector<int> mergedMinterms = currentLevel[i].minterms;
                        mergedMinterms.insert(mergedMinterms.end(), currentLevel[j].minterms.begin(), currentLevel[j].minterms.end());
                        std::sort(mergedMinterms.begin(), mergedMinterms.end());
                        mergedMinterms.erase(std::unique(mergedMinterms.begin(), mergedMinterms.end()), mergedMinterms.end());

                        nextLevel.push_back({combined, mergedMinterms, false});
                        addedMasks.insert(combined);
                    }
                }
            }
        }

        for (const auto& imp : currentLevel) {
            if (!imp.used) {
                primeImplicants.push_back(imp);
            }
        }

        currentLevel = nextLevel;
    }

    // Deduplicate Prime Implicants
    std::vector<Implicant> uniquePIs;
    for (const auto& pi : primeImplicants) {
        bool exists = false;
        for (const auto& u : uniquePIs) {
            if (u.mask == pi.mask) { exists = true; break; }
        }
        if (!exists) uniquePIs.push_back(pi);
    }

    // Set Cover Strategy to select Essential Implicants
    std::vector<Implicant> chosenPIs;
    std::set<int> uncovered(minterms.begin(), minterms.end());

    while (!uncovered.empty()) {
        int bestIdx = -1;
        int maxCovered = -1;

        for (size_t i = 0; i < uniquePIs.size(); ++i) {
            int count = 0;
            for (int m : uniquePIs[i].minterms) {
                if (uncovered.count(m)) count++;
            }
            if (count > maxCovered) {
                maxCovered = count;
                bestIdx = static_cast<int>(i);
            }
        }

        if (bestIdx == -1 || maxCovered == 0) break;

        chosenPIs.push_back(uniquePIs[bestIdx]);
        for (int m : uniquePIs[bestIdx].minterms) {
            uncovered.erase(m);
        }
    }

    // Generate output string and grouping structures
    std::stringstream ss;
    std::vector<Grouping> groupings;
    bool first = true;

    for (const auto& pi : chosenPIs) {
        if (!first) ss << " + ";
        first = false;

        std::string termStr = "";
        for (size_t i = 0; i < pi.mask.size(); ++i) {
            if (pi.mask[i] == '0') {
                termStr += "!" + vars[i];
            } else if (pi.mask[i] == '1') {
                termStr += vars[i];
            }
        }
        if (termStr.empty()) termStr = "1";
        ss << termStr;

        Grouping g;
        g.mintermsCovered = pi.minterms;
        groupings.push_back(g);
    }

    return {ss.str(), groupings};
}

// -----------------------------------------------------------------------------
// AnalyticalEngine Member Implementation
// -----------------------------------------------------------------------------
AnalyticalResults AnalyticalEngine::analyze(Circuit* circuit) {
    AnalyticalResults results;
    if (!circuit) return results;
    return results;
}

AnalyticalResults AnalyticalEngine::processExpression(const std::string& exprStr) {
    AnalyticalResults results;
    if (exprStr.empty()) return results;

    FormulaParser parser;
    std::shared_ptr<ExpNode> root = parser.parse(exprStr);
    if (!root) return results;

    std::set<std::string> varSet;
    root->collectVariables(varSet);

    std::vector<std::string> variables(varSet.begin(), varSet.end());
    if (variables.empty() || variables.size() > 4) return results;

    results.variables = variables;

    size_t numVars = variables.size();
    size_t numRows = 1ULL << numVars;
    std::vector<int> minterms;

    for (size_t r = 0; r < numRows; ++r) {
        TruthRow row;
        std::map<std::string, bool> env;
        for (size_t i = 0; i < numVars; ++i) {
            bool val = (r >> (numVars - 1 - i)) & 1;
            row.inputs.push_back(val);
            env[variables[i]] = val;
        }

        row.output = root->evaluate(env);
        results.truthTable.push_back(row);

        if (row.output) {
            minterms.push_back(static_cast<int>(r));
        }
    }

    results.canonicalSOP = generateCanonicalSOP(results.truthTable, variables);
    results.canonicalPOS = generateCanonicalPOS(results.truthTable, variables);
    results.kmap = buildKMapGrid(numVars, results.truthTable);

    auto [simplifiedStr, groups] = simplifyBooleanExpression(minterms, variables);
    results.simplifiedSOP = simplifiedStr;
    results.groupings = groups;

    return results;
}

} // namespace dls