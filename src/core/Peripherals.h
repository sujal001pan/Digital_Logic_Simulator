#pragma once

#include <string>
#include <utility>
#include "dls/Types.h"

namespace dls {

//=====================================================================
// Input Switch
//=====================================================================
class InputSwitch : public ISignalSource {
private:
    std::string m_id;
    bool m_state = false;

public:
    explicit InputSwitch(std::string id)
        : m_id(std::move(id)) {}

    bool getOutput() const override {
        return m_state;
    }

    void toggle() {
        m_state = !m_state;
    }

    void setState(bool state) {
        m_state = state;
    }

    const std::string& getId() const {
        return m_id;
    }
};

//=====================================================================
// LED Output Device
//=====================================================================
class LED {
private:
    std::string m_id;
    ISignalSource* m_inputPin = nullptr;
    bool m_cachedState = false;

public:
    explicit LED(std::string id)
        : m_id(std::move(id)) {}

    // Connect the LED to any signal-producing component.
    void connect(ISignalSource* src) {
        m_inputPin = src;
    }

    // Backward compatibility if older code calls connectInput().
    void connectInput(ISignalSource* src) {
        connect(src);
    }
    ISignalSource* getInputPin() const {
    return m_inputPin;
}

    void disconnect() {
        m_inputPin = nullptr;
        m_cachedState = false;
    }

    // Refresh the LED state from its connected signal.
    bool evaluate() {
        m_cachedState = (m_inputPin != nullptr)
                            ? m_inputPin->getOutput()
                            : false;
        return m_cachedState;
    }

    bool isOn() const {
        return m_cachedState;
    }

    // Backward compatibility.
    bool isLit() const {
        return isOn();
    }

    const std::string& getId() const {
        return m_id;
    }
};

} // namespace dls