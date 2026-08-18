#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>

#include "GridRenderer.h"
#include "InteractionController.h"

namespace dls {

/**
 * @brief Infinite zoomable and pannable workspace for circuit editing.
 */
class WorkspaceCanvas {
private:
    sf::View m_view;

    GridRenderer* m_grid = nullptr;
    InteractionController* m_controller = nullptr;

    bool m_panning = false;
    sf::Vector2i m_lastMouse;

    float m_zoom = 1.0f;

    const float MIN_ZOOM = 0.35f;
    const float MAX_ZOOM = 3.0f;

public:
    WorkspaceCanvas(sf::RenderWindow& window,
                    GridRenderer* grid,
                    InteractionController* controller)
        : m_grid(grid),
          m_controller(controller)
    {
        m_view = window.getDefaultView();
    }

    sf::View& getView() {
        return m_view;
    }

    float getZoom() const {
        return m_zoom;
    }

    /**
     * @brief Handles zooming and panning events.
     */
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) {

        // -------------------------
        // Mouse Wheel Zoom
        // -------------------------
        if (event.type == sf::Event::MouseWheelScrolled) {

            sf::Vector2f before =
                window.mapPixelToCoords(
                    sf::Vector2i(event.mouseWheelScroll.x,
                                 event.mouseWheelScroll.y),
                    m_view);

            float factor =
                (event.mouseWheelScroll.delta > 0.f) ? 0.9f : 1.1f;

            float newZoom =
                std::clamp(m_zoom * factor,
                           MIN_ZOOM,
                           MAX_ZOOM);

            factor = newZoom / m_zoom;
            m_zoom = newZoom;

            m_view.zoom(factor);

            sf::Vector2f after =
                window.mapPixelToCoords(
                    sf::Vector2i(event.mouseWheelScroll.x,
                                 event.mouseWheelScroll.y),
                    m_view);

            m_view.move(before - after);
        }

        // -------------------------
        // Middle Mouse Press
        // -------------------------
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Middle) {

            m_panning = true;
            m_lastMouse = sf::Mouse::getPosition(window);
        }

        // -------------------------
        // Middle Mouse Release
        // -------------------------
        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Middle) {

            m_panning = false;
        }

        // -------------------------
        // Mouse Move While Panning
        // -------------------------
        if (event.type == sf::Event::MouseMoved && m_panning) {

            sf::Vector2i current = sf::Mouse::getPosition(window);

            sf::Vector2f delta =
                window.mapPixelToCoords(m_lastMouse, m_view) -
                window.mapPixelToCoords(current, m_view);

            m_view.move(delta);

            m_lastMouse = current;
        }
    }

    /**
     * @brief Draws the workspace contents.
     */
    void render(sf::RenderWindow& window) {

        window.setView(m_view);

        if (m_grid)
            m_grid->render(&window);

        if (m_controller)
            m_controller->render(&window);
    }

    /**
     * @brief Restores the default camera.
     */
    void resetView(sf::RenderWindow& window) {

        m_view = window.getDefaultView();
        m_zoom = 1.0f;
    }
};

} // namespace dls