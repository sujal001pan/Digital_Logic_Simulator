// ============================================================================
// File: GraphicalSwitch.h
// Path: src/gui/GraphicalSwitch.h
// Purpose: Visual wrapper for an InputSwitch — click on its body to toggle.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include "../core/Peripherals.h"

namespace dls {

class GraphicalSwitch {
private:
    InputSwitch* m_switch=nullptr;

    sf::Vector2f m_position;

    sf::RectangleShape m_shadow;
    sf::RectangleShape m_body;
    sf::RectangleShape m_highlight;

    sf::Text m_idText;

    sf::CircleShape m_output;
    sf::CircleShape m_outputGlow;

    bool m_hover=false;

    static constexpr float WIDTH=72.f;
    static constexpr float HEIGHT=48.f;
    static constexpr float PIN_R=5.f;

    void updateLayout(){

        m_shadow.setPosition(m_position+sf::Vector2f(4.f,4.f));
        m_body.setPosition(m_position);
        m_highlight.setPosition(m_position);

        m_idText.setPosition(
            m_position.x+WIDTH/2.f,
            m_position.y+HEIGHT/2.f
        );

        sf::Vector2f outPos(
            m_position.x+WIDTH-PIN_R,
            m_position.y+HEIGHT/2.f-PIN_R
        );

        m_output.setPosition(outPos);
        m_outputGlow.setPosition(outPos-sf::Vector2f(4.f,4.f));
    }

public:

    GraphicalSwitch(InputSwitch* sw,
                    const sf::Font& font,
                    sf::Vector2f pos)
        :m_switch(sw),
         m_position(pos)
    {
        m_shadow.setSize({WIDTH,HEIGHT});
        m_shadow.setFillColor(sf::Color(0,0,0,50));

        m_body.setSize({WIDTH,HEIGHT});
        m_body.setFillColor(sf::Color(30,41,59));
        m_body.setOutlineThickness(2.f);
        m_body.setOutlineColor(sf::Color(71,85,105));

        m_highlight.setSize({WIDTH,HEIGHT});
        m_highlight.setFillColor(sf::Color::Transparent);
        m_highlight.setOutlineThickness(2.f);

        m_idText.setFont(font);
        m_idText.setCharacterSize(13);
        m_idText.setFillColor(sf::Color(248,250,252));
        m_idText.setString(m_switch->getId());

        auto ib=m_idText.getLocalBounds();
        m_idText.setOrigin(
            ib.left+ib.width/2.f,
            ib.top+ib.height/2.f
        );

        m_output.setRadius(PIN_R);
        m_output.setFillColor(sf::Color(239,68,68));

        m_outputGlow.setRadius(PIN_R+4.f);
        m_outputGlow.setFillColor(sf::Color(34,197,94,0));

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

    sf::Vector2f getOutputPinPosition() const{
        return {
            m_output.getPosition().x+PIN_R,
            m_output.getPosition().y+PIN_R
        };
    }

    void toggle(){
        m_switch->toggle();
    }

    InputSwitch* getLogicalSwitch() const{return m_switch;}

    void render(sf::RenderWindow& window){

        m_highlight.setOutlineColor(
            m_hover?sf::Color(56,189,248):sf::Color::Transparent);

        bool on=m_switch->getOutput();

        m_body.setFillColor(on?sf::Color(51,65,40):sf::Color(30,41,59));

        if(on){
            m_output.setFillColor(sf::Color(34,197,94));
            m_outputGlow.setFillColor(sf::Color(34,197,94,90));
        }
        else{
            m_output.setFillColor(sf::Color(239,68,68));
            m_outputGlow.setFillColor(sf::Color(34,197,94,0));
        }

        window.draw(m_shadow);
        window.draw(m_body);
        window.draw(m_highlight);

        window.draw(m_outputGlow);
        window.draw(m_output);

        window.draw(m_idText);
    }
};

}