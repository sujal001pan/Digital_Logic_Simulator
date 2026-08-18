#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <cctype>

#include "ExpressionTree.h"

namespace dls {

/**
 * @brief Parses Boolean expressions into an expression tree.
 *
 * Supported operators:
 *  - !  (NOT)
 *  - &  (AND)
 *  - |  (OR)
 *  - ( )
 *
 * Example:
 *   !(A&B)|C
 */
class FormulaParser {
private:
    std::string m_tokens;
    size_t m_pos = 0;

    char peek() const {
        return (m_pos < m_tokens.size()) ? m_tokens[m_pos] : '\0';
    }

    char get() {
        return (m_pos < m_tokens.size()) ? m_tokens[m_pos++] : '\0';
    }

    void skipWhitespace() {
        while (m_pos < m_tokens.size() &&
               std::isspace(static_cast<unsigned char>(m_tokens[m_pos]))) {
            ++m_pos;
        }
    }

public:
    FormulaParser() = default;
    ~FormulaParser() = default;

    /**
     * @brief Parse a Boolean expression into an expression tree.
     */
    std::shared_ptr<ExpNode> parse(const std::string& expression) {
        m_tokens = expression;
        m_pos = 0;

        auto root = parseExpression();

        skipWhitespace();

        if (m_pos != m_tokens.size()) {
            throw std::runtime_error("Unexpected token in expression.");
        }

        return root;
    }

private:
    /**
     * Expression := Term { '|' Term }
     */
    std::shared_ptr<ExpNode> parseExpression() {
        auto left = parseTerm();

        skipWhitespace();

        while (peek() == '|') {
            get();
            auto right = parseTerm();
            left = std::make_shared<OperatorNode>('|', left, right);

            skipWhitespace();
        }

        return left;
    }

    /**
     * Term := Factor { '&' Factor }
     */
    std::shared_ptr<ExpNode> parseTerm() {
        auto left = parseFactor();

        skipWhitespace();

        while (peek() == '&') {
            get();
            auto right = parseFactor();
            left = std::make_shared<OperatorNode>('&', left, right);

            skipWhitespace();
        }

        return left;
    }

    /**
     * Factor := '!' Factor
     *         | '(' Expression ')'
     *         | Variable
     */
    std::shared_ptr<ExpNode> parseFactor() {
        skipWhitespace();

        char c = peek();

        if (c == '!') {
            get();
            auto operand = parseFactor();
            return std::make_shared<OperatorNode>('!', operand);
        }

        if (c == '(') {
            get();

            auto expr = parseExpression();

            skipWhitespace();

            if (get() != ')') {
                throw std::runtime_error("Missing closing parenthesis.");
            }

            return expr;
        }

        if (std::isalpha(static_cast<unsigned char>(c))) {

            std::string varName;

            while (std::isalnum(static_cast<unsigned char>(peek()))) {
                varName += get();
            }

            return std::make_shared<VariableNode>(varName);
        }

        throw std::runtime_error("Invalid token in Boolean expression.");
    }
};

} // namespace dls