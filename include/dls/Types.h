#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>

namespace dls {

// Supported gate types
enum class Op {
    AND,
    OR,
    NOT,
    NAND,
    NOR,
    XOR,
    XNOR
};

// Any component capable of producing a logic signal
class ISignalSource {
public:
    virtual ~ISignalSource() = default;
    virtual bool getOutput() const = 0;
};

// Truth table row
struct TruthRow {
    std::vector<bool> inputs;
    bool output;
};

// Karnaugh map cell
struct KMapCell {
    int row;
    int col;
    int mintermIndex;
    bool value;
};

// Group produced by K-map simplification
struct Grouping {
    std::vector<int> mintermsCovered;
    std::string termExpression;
};

// Observer interface for GUI updates
class ISimulationListener {
public:
    virtual ~ISimulationListener() = default;
    virtual void onCircuitChanged() = 0;
};

// Parser exception
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg)
        : std::runtime_error(msg) {}
};

} // namespace dls