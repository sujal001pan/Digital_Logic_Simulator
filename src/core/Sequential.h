#pragma once

#include <string>
#include <utility>
#include "dls/Types.h"

namespace dls {

//=====================================================================
// Base Flip-Flop
//=====================================================================
class FlipFlop : public ISignalSource {
protected:
    std::string m_id;
    bool m_q = false;

public:
    explicit FlipFlop(std::string id)
        : m_id(std::move(id)) {}

    virtual ~FlipFlop() = default;

    bool getOutput() const override {
        return m_q;
    }

    bool getInvertedOutput() const {
        return !m_q;
    }

    const std::string& getId() const {
        return m_id;
    }

    virtual void clockPulse() = 0;
};

//=====================================================================
// D Flip-Flop
//=====================================================================
class DFlipFlop : public FlipFlop {
private:
    bool m_d = false;

public:
    using FlipFlop::FlipFlop;

    void setInput(bool d) {
        m_d = d;
    }

    void clockPulse() override {
        m_q = m_d;
    }
};

//=====================================================================
// JK Flip-Flop
//=====================================================================
class JKFlipFlop : public FlipFlop {
private:
    bool m_j = false;
    bool m_k = false;

public:
    using FlipFlop::FlipFlop;

    void setInputs(bool j, bool k) {
        m_j = j;
        m_k = k;
    }

    void clockPulse() override {

        if (!m_j && !m_k) {
            return;                 // Hold
        }
        else if (!m_j && m_k) {
            m_q = false;            // Reset
        }
        else if (m_j && !m_k) {
            m_q = true;             // Set
        }
        else {
            m_q = !m_q;             // Toggle
        }
    }
};

//=====================================================================
// SR Flip-Flop
//=====================================================================
class SRFlipFlop : public FlipFlop {
private:
    bool m_s = false;
    bool m_r = false;

public:
    using FlipFlop::FlipFlop;

    void setInputs(bool s, bool r) {
        m_s = s;
        m_r = r;
    }

    void clockPulse() override {

        if (m_s && !m_r)
            m_q = true;

        else if (!m_s && m_r)
            m_q = false;

        // S=R=0 → Hold
        // S=R=1 → Invalid; preserve previous state.
    }
};

//=====================================================================
// T Flip-Flop
//=====================================================================
class TFlipFlop : public FlipFlop {
private:
    bool m_t = false;

public:
    using FlipFlop::FlipFlop;

    void setInput(bool t) {
        m_t = t;
    }

    void clockPulse() override {

        if (m_t)
            m_q = !m_q;
    }
};

} // namespace dls