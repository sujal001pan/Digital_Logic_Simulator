#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>

#include "ExpressionTree.h"
#include "dls/Types.h"          // ADDED — brings in the canonical TruthRow

namespace dls {

// The local "struct TruthRow {...}" block is REMOVED entirely —
// we use the one from Types.h now.

class TruthTableGenerator {
private:
    std::vector<TruthRow> m_matrix;
    std::set<std::string> m_variables;

    void generateRows(
        const std::shared_ptr<ExpNode>& root,
        const std::vector<std::string>& varList,
        std::map<std::string, bool>& assignment,
        size_t index)
    {
        if (index == varList.size()) {
            TruthRow row;

            // Build row.inputs as a vector in varList order (Types.h's
            // TruthRow wants vector<bool>, not the map we use internally
            // for evaluation).
            row.inputs.reserve(varList.size());
            for (const auto& v : varList)
                row.inputs.push_back(assignment[v]);

            row.output = root->evaluate(assignment);   // was: row.result
            m_matrix.push_back(row);
            return;
        }

        assignment[varList[index]] = false;
        generateRows(root, varList, assignment, index + 1);

        assignment[varList[index]] = true;
        generateRows(root, varList, assignment, index + 1);
    }

public:
    TruthTableGenerator() = default;

    void generate(const std::shared_ptr<ExpNode>& root) {
        if (!root)
            return;

        m_matrix.clear();
        m_variables.clear();

        root->collectVariables(m_variables);

        std::vector<std::string> varList(
            m_variables.begin(),
            m_variables.end());

        std::map<std::string, bool> assignment;

        generateRows(root, varList, assignment, 0);
    }

    void display() const {
        if (m_matrix.empty())
            return;

        std::vector<std::string> varList(
            m_variables.begin(),
            m_variables.end());

        for (const auto& var : varList)
            std::cout << var << " ";

        std::cout << "| F\n";

        for (size_t i = 0; i < varList.size() * 2 + 3; ++i)
            std::cout << "-";

        std::cout << "\n";

        for (const auto& row : m_matrix) {
            for (size_t i = 0; i < row.inputs.size(); ++i)   // was: row.inputs.at(var)
                std::cout << row.inputs[i] << " ";

            std::cout << "| " << row.output << "\n";          // was: row.result
        }
    }

    const std::vector<TruthRow>& getMatrix() const {
        return m_matrix;
    }

    std::vector<std::string> getVariables() const {
        return std::vector<std::string>(
            m_variables.begin(),
            m_variables.end());
    }
};

} // namespace dls