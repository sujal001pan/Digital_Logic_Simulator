#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <stdexcept>
#include<algorithm>

#include "Gate.h"
#include "Wire.h"
#include "Peripherals.h"
#include "Sequential.h"

namespace dls {

class Circuit {
private:
    std::vector<std::unique_ptr<Gate>> m_gates;
    std::vector<std::unique_ptr<Wire>> m_wires;
    std::vector<std::unique_ptr<InputSwitch>> m_switches;
    std::vector<std::unique_ptr<LED>> m_leds;
    std::vector<std::unique_ptr<FlipFlop>> m_flipFlops;

    std::map<std::string, Gate*> m_gateMap;
    std::map<std::string, InputSwitch*> m_switchMap;
    std::map<std::string, LED*> m_ledMap;
    std::map<std::string, FlipFlop*> m_flipFlopMap;

public:
    Circuit() = default;
    ~Circuit() = default;

    // ---------------- ADD COMPONENTS ----------------

    Gate* addGate(std::unique_ptr<Gate> gate, const std::string& id) {
        if (m_gateMap.count(id))
            throw std::runtime_error("Duplicate Gate ID: " + id);

        Gate* ptr = gate.get();
        m_gates.push_back(std::move(gate));
        m_gateMap[id] = ptr;
        return ptr;
    }

    InputSwitch* addSwitch(std::unique_ptr<InputSwitch> sw,
                           const std::string& id) {
        if (m_switchMap.count(id))
            throw std::runtime_error("Duplicate Switch ID: " + id);

        InputSwitch* ptr = sw.get();
        m_switches.push_back(std::move(sw));
        m_switchMap[id] = ptr;
        return ptr;
    }

    LED* addLED(std::unique_ptr<LED> led, const std::string& id) {
        if (m_ledMap.count(id))
            throw std::runtime_error("Duplicate LED ID: " + id);

        LED* ptr = led.get();
        m_leds.push_back(std::move(led));
        m_ledMap[id] = ptr;
        return ptr;
    }

    FlipFlop* addFlipFlop(std::unique_ptr<FlipFlop> ff,
                          const std::string& id) {
        if (m_flipFlopMap.count(id))
            throw std::runtime_error("Duplicate FlipFlop ID: " + id);

        FlipFlop* ptr = ff.get();
        m_flipFlops.push_back(std::move(ff));
        m_flipFlopMap[id] = ptr;
        return ptr;
    }

    Wire* addWire(std::unique_ptr<Wire> wire) {
        Wire* ptr = wire.get();

        // Physically connect the wire immediately.
        ptr->connect();

        m_wires.push_back(std::move(wire));
        return ptr;
    }
    // ---------------- REMOVE COMPONENTS ----------------

void removeWire(Wire* wire) {

    if (!wire)
        return;

    if (wire->getDestination() && wire->getSource())
        wire->getDestination()->disconnectInput(wire->getSource());

    m_wires.erase(
        std::remove_if(m_wires.begin(), m_wires.end(),
            [wire](const std::unique_ptr<Wire>& w) {
                return w.get() == wire;
            }),
        m_wires.end());
}

void removeGate(Gate* gate) {

    if (!gate)
        return;

    for (auto& g : m_gates) {
        if (g.get() != gate)
            g->disconnectInput(gate);
    }

    for (auto& led : m_leds) {
        if (led->getInputPin() == gate)
            led->disconnect();
    }

    m_wires.erase(
        std::remove_if(m_wires.begin(), m_wires.end(),
            [gate](const std::unique_ptr<Wire>& w) {
                return w->getSource() == gate || w->getDestination() == gate;
            }),
        m_wires.end());

    for (auto it = m_gateMap.begin(); it != m_gateMap.end(); ++it) {
        if (it->second == gate) {
            m_gateMap.erase(it);
            break;
        }
    }

    m_gates.erase(
        std::remove_if(m_gates.begin(), m_gates.end(),
            [gate](const std::unique_ptr<Gate>& g) {
                return g.get() == gate;
            }),
        m_gates.end());
}

void removeSwitch(InputSwitch* sw) {

    if (!sw)
        return;

    for (auto& g : m_gates)
        g->disconnectInput(sw);

    for (auto& led : m_leds) {
        if (led->getInputPin() == sw)
            led->disconnect();
    }

    m_wires.erase(
        std::remove_if(m_wires.begin(), m_wires.end(),
            [sw](const std::unique_ptr<Wire>& w) {
                return w->getSource() == sw;
            }),
        m_wires.end());

    for (auto it = m_switchMap.begin(); it != m_switchMap.end(); ++it) {
        if (it->second == sw) {
            m_switchMap.erase(it);
            break;
        }
    }

    m_switches.erase(
        std::remove_if(m_switches.begin(), m_switches.end(),
            [sw](const std::unique_ptr<InputSwitch>& s) {
                return s.get() == sw;
            }),
        m_switches.end());
}

void removeLED(LED* led) {

    if (!led)
        return;

    for (auto it = m_ledMap.begin(); it != m_ledMap.end(); ++it) {
        if (it->second == led) {
            m_ledMap.erase(it);
            break;
        }
    }

    m_leds.erase(
        std::remove_if(m_leds.begin(), m_leds.end(),
            [led](const std::unique_ptr<LED>& l) {
                return l.get() == led;
            }),
        m_leds.end());
}

    // ---------------- LOOKUP FUNCTIONS ----------------

    Gate* findGate(const std::string& id) const {
        auto it = m_gateMap.find(id);
        return (it != m_gateMap.end()) ? it->second : nullptr;
    }

    InputSwitch* findSwitch(const std::string& id) const {
        auto it = m_switchMap.find(id);
        return (it != m_switchMap.end()) ? it->second : nullptr;
    }

    LED* findLED(const std::string& id) const {
        auto it = m_ledMap.find(id);
        return (it != m_ledMap.end()) ? it->second : nullptr;
    }

    FlipFlop* findFlipFlop(const std::string& id) const {
        auto it = m_flipFlopMap.find(id);
        return (it != m_flipFlopMap.end()) ? it->second : nullptr;
    }

    // Universal signal-source lookup.
    ISignalSource* findSignalSource(const std::string& id) const {

        if (auto s = findSwitch(id))
            return s;

        if (auto g = findGate(id))
            return g;

        if (auto f = findFlipFlop(id))
            return f;

        return nullptr;
    }

    // ---------------- SIMULATION ----------------

    void evaluateAll() {

        if (m_gates.empty())
            return;

        // Relaxation passes.
        for (size_t pass = 0; pass < m_gates.size(); ++pass) {

            for (auto& gate : m_gates)
                gate->evaluate();
        }
    }

    void pulseClock() {

        for (auto& ff : m_flipFlops)
            ff->clockPulse();

        evaluateAll();
    }

    // ---------------- GETTERS ----------------

    const std::vector<std::unique_ptr<Gate>>& getGates() const {
        return m_gates;
    }

    const std::vector<std::unique_ptr<Wire>>& getWires() const {
        return m_wires;
    }

    const std::vector<std::unique_ptr<InputSwitch>>& getSwitches() const {
        return m_switches;
    }

    const std::vector<std::unique_ptr<LED>>& getLeds() const {
        return m_leds;
    }

    const std::vector<std::unique_ptr<FlipFlop>>& getFlipFlops() const {
        return m_flipFlops;
    }
};

} // namespace dls