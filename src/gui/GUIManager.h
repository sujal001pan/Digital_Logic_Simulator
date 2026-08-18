// ============================================================================
// File: GUIManager.h
// Path: src/gui/GUIManager.h
// Purpose: Professional SFML window manager with callback-driven main loop.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace dls {

class GUIManager {
private:
    sf::RenderWindow m_window;
    sf::View m_view;

    bool m_vsync = true;
    bool m_showFPS = false;

public:

    GUIManager(unsigned width,
               unsigned height,
               const std::string& title)
        : m_window(
            sf::VideoMode(width,height),
            title,
            sf::Style::Default),
          m_view(m_window.getDefaultView())
    {
        m_window.setVerticalSyncEnabled(m_vsync);
        m_window.setFramerateLimit(144);

        m_window.setKeyRepeatEnabled(false);
    }

    sf::RenderWindow& window(){
        return m_window;
    }

    sf::View& view(){
        return m_view;
    }

    void setVSync(bool enabled){

        m_vsync=enabled;
        m_window.setVerticalSyncEnabled(enabled);
    }

    void run(
        std::function<void(const sf::Event&)> onEvent,
        std::function<void(float)> onUpdate,
        std::function<void(sf::RenderWindow&)> onDraw)
    {
        sf::Clock deltaClock;

        while(m_window.isOpen()){

            float dt=deltaClock.restart().asSeconds();

            sf::Event event;

            while(m_window.pollEvent(event)){

                switch(event.type){

                case sf::Event::Closed:
                    m_window.close();
                    break;

                case sf::Event::Resized:

                    m_view.setSize(
                        event.size.width,
                        event.size.height);

                    m_window.setView(m_view);
                    break;

                default:
                    break;
                }

                onEvent(event);
            }

            onUpdate(dt);

            m_window.clear();

            m_window.setView(m_view);

            onDraw(m_window);

            m_window.display();
        }
    }
};

}