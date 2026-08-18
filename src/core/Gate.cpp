#include "Gate.h"

namespace dls {

// ============================================================================
// AND Gate
// ============================================================================
bool ANDGate::evaluate() {

    if (m_inputPins.empty()) {
        m_cachedOutput = false;
        return m_cachedOutput;
    }

    bool result = true;

    for (const auto* pin : m_inputPins) {
        result &= (pin != nullptr) ? pin->getOutput() : false;
    }

    m_cachedOutput = result;
    return m_cachedOutput;
}

// ============================================================================
// OR Gate
// ============================================================================
bool ORGate::evaluate() {

    if (m_inputPins.empty()) {
        m_cachedOutput = false;
        return m_cachedOutput;
    }

    bool result = false;

    for (const auto* pin : m_inputPins) {
        result |= (pin != nullptr) ? pin->getOutput() : false;
    }

    m_cachedOutput = result;
    return m_cachedOutput;
}

// ============================================================================
// NOT Gate
// ============================================================================

void NOTGate::connectInput(ISignalSource* src) {

    m_inputPins.clear();

    if (src)
        m_inputPins.push_back(src);
}

bool NOTGate::evaluate() {

    bool input = false;

    if (!m_inputPins.empty() && m_inputPins[0] != nullptr)
        input = m_inputPins[0]->getOutput();

    m_cachedOutput = !input;
    return m_cachedOutput;
}

// ============================================================================
// NAND Gate
// ============================================================================
bool NANDGate::evaluate() {

    if (m_inputPins.empty()) {
        m_cachedOutput = true;
        return m_cachedOutput;
    }

    bool result = true;

    for (const auto* pin : m_inputPins) {
        result &= (pin != nullptr) ? pin->getOutput() : false;
    }

    m_cachedOutput = !result;
    return m_cachedOutput;
}

// ============================================================================
// NOR Gate
// ============================================================================
bool NORGate::evaluate() {

    if (m_inputPins.empty()) {
        m_cachedOutput = true;
        return m_cachedOutput;
    }

    bool result = false;

    for (const auto* pin : m_inputPins) {
        result |= (pin != nullptr) ? pin->getOutput() : false;
    }

    m_cachedOutput = !result;
    return m_cachedOutput;
}

// ============================================================================
// XOR Gate
// ============================================================================
bool XORGate::evaluate() {

    if (m_inputPins.empty()) {
        m_cachedOutput = false;
        return m_cachedOutput;
    }

    bool result = false;

    for (const auto* pin : m_inputPins) {

        if (pin && pin->getOutput())
            result = !result;
    }

    m_cachedOutput = result;
    return m_cachedOutput;
}

// ============================================================================
// XNOR Gate
// ============================================================================
bool XNORGate::evaluate() {

    if (m_inputPins.empty()) {
        m_cachedOutput = true;
        return m_cachedOutput;
    }

    bool result = false;

    for (const auto* pin : m_inputPins) {

        if (pin && pin->getOutput())
            result = !result;
    }

    m_cachedOutput = !result;
    return m_cachedOutput;
}

} // namespace dls