#pragma once

#include <vector>
#include <algorithm>

#include "Circuit.h"
#include "dls/Types.h"

namespace dls {

/**
 * @brief Coordinates circuit simulation and notifies GUI listeners.
 */
class SimulationEngine {
private:
    Circuit* m_activeCircuit = nullptr;
    std::vector<ISimulationListener*> m_listeners;

public:
    SimulationEngine() = default;
    ~SimulationEngine() = default;

    // Attach the circuit that this engine controls.
    void setCircuit(Circuit* circuit) {
        m_activeCircuit = circuit;
    }

    // Register a GUI listener.
    void addListener(ISimulationListener* listener) {

        if (!listener)
            return;

        if (std::find(m_listeners.begin(),
                      m_listeners.end(),
                      listener) == m_listeners.end()) {

            m_listeners.push_back(listener);
        }
    }

    void removeListener(ISimulationListener* listener) {

        m_listeners.erase(
            std::remove(m_listeners.begin(),
                        m_listeners.end(),
                        listener),
            m_listeners.end());
    }

    /**
     * @brief Re-evaluates the active circuit and refreshes LEDs.
     */
    void step() {

        if (!m_activeCircuit)
            return;

        // Propagate signals through all gates.
        m_activeCircuit->evaluateAll();

        // Refresh LED states.
        for (const auto& led : m_activeCircuit->getLeds()) {

            if (led)
                led->evaluate();
        }

        notifyListeners();
    }

    /**
     * @brief Issues one manual clock pulse to every flip-flop.
     */
    void pulseClock() {

        if (!m_activeCircuit)
            return;

        m_activeCircuit->pulseClock();

        // Refresh LEDs after sequential logic updates.
        for (const auto& led : m_activeCircuit->getLeds()) {

            if (led)
                led->evaluate();
        }

        notifyListeners();
    }

private:

    void notifyListeners() {

        for (auto* listener : m_listeners) {

            if (listener)
                listener->onCircuitChanged();
        }
    }
};

} // namespace dls