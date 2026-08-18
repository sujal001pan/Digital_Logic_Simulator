// ============================================================================
// File: GraphicalLED.h
// Path: src/gui/GraphicalLED.h
// Purpose: Visual wrapper for an LED output device.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include "../core/Peripherals.h"

namespace dls {

class GraphicalLED {
private:
    LED* m_led=nullptr;

    sf::Vector2f m_position;

    sf::CircleShape m_shadow;
    sf::CircleShape m_body;
    sf::CircleShape m_highlight;

    sf::CircleShape m_input;

    sf::Text m_idText;

    bool m_hover=false;

    static constexpr float RADIUS=28.f;
    static constexpr float PIN_R=5.f;

    void updateLayout(){

        m_shadow.setPosition(m_position+sf::Vector2f(4.f,4.f));
        m_body.setPosition(m_position);
        m_highlight.setPosition(m_position);

        m_idText.setPosition(
            m_position.x+RADIUS,
            m_position.y+RADIUS*2.f+12.f
        );

        m_input.setPosition(
            m_position.x-PIN_R,
            m_position.y+RADIUS-PIN_R
        );
    }

public:

    GraphicalLED(LED* led,
                const sf::Font& font,
                sf::Vector2f pos)
        :m_led(led),
         m_position(pos)
    {
        m_shadow.setRadius(RADIUS);
        m_shadow.setFillColor(sf::Color(0,0,0,50));

        m_body.setRadius(RADIUS);
        m_body.setOutlineThickness(2.f);
        m_body.setOutlineColor(sf::Color(71,85,105));

        m_highlight.setRadius(RADIUS);
        m_highlight.setFillColor(sf::Color::Transparent);
        m_highlight.setOutlineThickness(2.f);

        m_input.setRadius(PIN_R);
        m_input.setFillColor(sf::Color(100,116,139));

        m_idText.setFont(font);
        m_idText.setCharacterSize(11);
        m_idText.setFillColor(sf::Color(148,163,184));
        m_idText.setString(m_led->getId());

        auto ib=m_idText.getLocalBounds();
        m_idText.setOrigin(
            ib.left+ib.width/2.f,
            ib.top+ib.height/2.f
        );

        updateLayout();
    }

    void setPosition(sf::Vector2f p){
        m_position=p;
        updateLayout();
    }

    sf::Vector2f getPosition() const{return m_position;}

    void setHovered(bool h){m_hover=h;}

    bool contains(sf::Vector2f p) const{
        return m_body.getGlobalBounds().contains(p);
    }

    sf::Vector2f getInputPinPosition() const{
        return {
            m_input.getPosition().x+PIN_R,
            m_input.getPosition().y+PIN_R
        };
    }

    LED* getLogicalLED() const{return m_led;}

    void render(sf::RenderWindow& window){

        m_highlight.setOutlineColor(
            m_hover?sf::Color(56,189,248):sf::Color::Transparent);

        bool on=m_led->isOn();

        m_body.setFillColor(
            on?sf::Color(34,197,94):sf::Color(51,25,25));

        window.draw(m_shadow);
        window.draw(m_body);
        window.draw(m_highlight);
        window.draw(m_input);
        window.draw(m_idText);
    }
};

}