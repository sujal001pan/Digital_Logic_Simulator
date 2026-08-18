#pragma once

#include <string>
#include <utility>

#include "Gate.h"
#include "dls/Types.h"

namespace dls {

/**
 * @brief Represents a connection between one signal source and one gate input.
 */
class Wire {
private:
    std::string m_id;
    ISignalSource* m_source = nullptr;
    Gate* m_destination = nullptr;
    int m_pinIndex = 0;

public:
    Wire(std::string id,
         ISignalSource* source,
         Gate* destination,
         int pinIndex)
        : m_id(std::move(id)),
          m_source(source),
          m_destination(destination),
          m_pinIndex(pinIndex) {}

    ~Wire() = default;

    // Physically connect the wire to the destination gate.
    void connect() {

        if (m_destination)
            m_destination->connectInput(m_source);
    }

    // Disconnect the source.
    void disconnect() {
        m_source = nullptr;
    }

    // Getters
    ISignalSource* getSource() const {
        return m_source;
    }

    Gate* getDestination() const {
        return m_destination;
    }

    int getPinIndex() const {
        return m_pinIndex;
    }

    const std::string& getId() const {
        return m_id;
    }

    bool isConnected() const {
        return m_source != nullptr && m_destination != nullptr;
    }
};

} // namespace dls