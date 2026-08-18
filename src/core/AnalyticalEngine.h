#pragma once

#include <vector>
#include <string>
#include "dls/Types.h"

namespace dls {

class Circuit;

struct AnalyticalResults {
    std::vector<TruthRow> truthTable;
    std::vector<std::vector<KMapCell>> kmap;
    std::vector<Grouping> groupings;
    std::vector<std::string> variables;
    std::string canonicalSOP;
    std::string canonicalPOS;
    std::string simplifiedSOP;
};

class AnalyticalEngine {
public:
    static AnalyticalResults analyze(Circuit* circuit);
    static AnalyticalResults processExpression(const std::string& exprStr);
};

} // namespace dls