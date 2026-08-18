#pragma once
#include <string>
#include <memory>
#include <map>
#include <set>

namespace dls {

/**
 * @brief Base abstract class representation for boolean math formulation expression tree nodes.
 */
class ExpNode {
public:
    virtual ~ExpNode() = default;

    /**
     * @brief Recursively evaluates the boolean output state based on active variable true/false mapping definitions.
     */
    virtual bool evaluate(const std::map<std::string, bool>& env) const = 0;

    /**
     * @brief Recursively traverses the formula mapping nodes to collect all unique input variable name strings.
     */
    virtual void collectVariables(std::set<std::string>& uniqueVars) const = 0;
};

/**
 * @brief Terminal leaf variable node matching string variables directly to true/false logic values.
 */
class VariableNode : public ExpNode {
private:
    std::string m_name;

public:
    explicit VariableNode(std::string name) : m_name(std::move(name)) {}

    bool evaluate(const std::map<std::string, bool>& env) const override {
        auto it = env.find(m_name);
        return (it != env.end()) ? it->second : false;
    }

    void collectVariables(std::set<std::string>& uniqueVars) const override {
        uniqueVars.insert(m_name);
    }
};

/**
 * @brief Internal operator structural node executing standard boolean logic gating calculations.
 */
class OperatorNode : public ExpNode {
private:
    char m_op; // '&' for AND, '|' for OR, '!' for NOT
    std::shared_ptr<ExpNode> m_left;
    std::shared_ptr<ExpNode> m_right;

public:
    OperatorNode(char op, std::shared_ptr<ExpNode> left, std::shared_ptr<ExpNode> right = nullptr)
        : m_op(op), m_left(std::move(left)), m_right(std::move(right)) {}

    bool evaluate(const std::map<std::string, bool>& env) const override {
        if (m_op == '!') {
            return m_left ? !m_left->evaluate(env) : false;
        }
        if (m_op == '&') {
            return (m_left && m_right) ? (m_left->evaluate(env) && m_right->evaluate(env)) : false;
        }
        if (m_op == '|') {
            return (m_left && m_right) ? (m_left->evaluate(env) || m_right->evaluate(env)) : false;
        }
        return false;
    }

    void collectVariables(std::set<std::string>& uniqueVars) const override {
        if (m_left) m_left->collectVariables(uniqueVars);
        if (m_right) m_right->collectVariables(uniqueVars);
    }
};

} // namespace dls
