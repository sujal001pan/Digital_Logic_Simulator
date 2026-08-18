#pragma once

#include <vector>
#include <string>
#include "dls/Types.h"

namespace dls {

//=====================================================================
// Base Gate Class
//=====================================================================
class Gate : public ISignalSource {
protected:
    std::string m_id;
    std::vector<ISignalSource*> m_inputPins;
    bool m_cachedOutput = false;

public:
    explicit Gate(const std::string& id)
        : m_id(id) {}

    virtual ~Gate() = default;

    virtual bool evaluate() = 0;

    bool getOutput() const override {
        return m_cachedOutput;
    }

    virtual void connectInput(ISignalSource* src) {
        m_inputPins.push_back(src);     // nullptr handled during evaluate()
    }
    
    virtual void disconnectInput(ISignalSource* src) {
    for (auto& pin : m_inputPins) {
        if (pin == src)
            pin = nullptr;
    }
}

    void clearInputs() {
        m_inputPins.clear();
    }

    const std::string& getId() const {
        return m_id;
    }

    virtual Op getType() const = 0;
};

//=====================================================================
// AND Gate
//=====================================================================
class ANDGate : public Gate {
public:
    explicit ANDGate(const std::string& id)
        : Gate(id) {}

    bool evaluate() override;
    Op getType() const override { return Op::AND; }
};

//=====================================================================
// OR Gate
//=====================================================================
class ORGate : public Gate {
public:
    explicit ORGate(const std::string& id)
        : Gate(id) {}

    bool evaluate() override;
    Op getType() const override { return Op::OR; }
};

//=====================================================================
// NOT Gate
//=====================================================================
class NOTGate : public Gate {
public:
    explicit NOTGate(const std::string& id)
        : Gate(id) {}

    void connectInput(ISignalSource* src) override;
    bool evaluate() override;
    Op getType() const override { return Op::NOT; }
};

//=====================================================================
// NAND Gate
//=====================================================================
class NANDGate : public Gate {
public:
    explicit NANDGate(const std::string& id)
        : Gate(id) {}

    bool evaluate() override;
    Op getType() const override { return Op::NAND; }
};

//=====================================================================
// NOR Gate
//=====================================================================
class NORGate : public Gate {
public:
    explicit NORGate(const std::string& id)
        : Gate(id) {}

    bool evaluate() override;
    Op getType() const override { return Op::NOR; }
};

//=====================================================================
// XOR Gate
//=====================================================================
class XORGate : public Gate {
public:
    explicit XORGate(const std::string& id)
        : Gate(id) {}

    bool evaluate() override;
    Op getType() const override { return Op::XOR; }
};

//=====================================================================
// XNOR Gate
//=====================================================================
class XNORGate : public Gate {
public:
    explicit XNORGate(const std::string& id)
        : Gate(id) {}

    bool evaluate() override;
    Op getType() const override { return Op::XNOR; }
};

} // namespace dls