// ============================================================================
// File: GraphicalGate.h
// Path: src/gui/GraphicalGate.h
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../core/Gate.h"

namespace dls {

class GraphicalGate {
private:
    Gate* m_gate=nullptr;

    sf::Vector2f m_position;

    sf::RectangleShape m_shadow;
    sf::RectangleShape m_body;
    sf::RectangleShape m_highlight;

    sf::Text m_gateText;
    sf::Text m_idText;

    const sf::Font& m_font;

    std::vector<sf::CircleShape> m_inputs;
    sf::CircleShape m_output;
    sf::CircleShape m_outputGlow;

    bool m_hover=false;
    bool m_selected=false;
    bool m_dragging=false;

    static constexpr float WIDTH=96.f;
    static constexpr float HEIGHT=64.f;
    static constexpr float PIN_R=5.f;

private:

    std::string gateName() const{

        switch(m_gate->getType()){

        case Op::AND: return "AND";
        case Op::OR: return "OR";
        case Op::NOT: return "NOT";
        case Op::NAND: return "NAND";
        case Op::NOR: return "NOR";
        case Op::XOR: return "XOR";
        case Op::XNOR: return "XNOR";

        default: return "GATE";
        }
    }

    void updateLayout(){

        m_shadow.setPosition(m_position+sf::Vector2f(4.f,4.f));
        m_body.setPosition(m_position);
        m_highlight.setPosition(m_position);

        m_gateText.setPosition(
            m_position.x+WIDTH/2.f,
            m_position.y+22.f
        );

        m_idText.setPosition(
            m_position.x+WIDTH/2.f,
            m_position.y+47.f
        );

        size_t n=m_inputs.size();

        if(n==1){

            m_inputs[0].setPosition(
                m_position.x-PIN_R,
                m_position.y+HEIGHT/2.f-PIN_R
            );
        }
        else{

            for(size_t i=0;i<n;i++){

                float y=(HEIGHT*(i+1))/(n+1);

                m_inputs[i].setPosition(
                    m_position.x-PIN_R,
                    m_position.y+y-PIN_R
                );
            }
        }

        sf::Vector2f outPos(
            m_position.x+WIDTH-PIN_R,
            m_position.y+HEIGHT/2.f-PIN_R
        );

        m_output.setPosition(outPos);
        m_outputGlow.setPosition(outPos-sf::Vector2f(4.f,4.f));
    }

public:

    GraphicalGate(Gate* gate,
                  const sf::Font& font,
                  sf::Vector2f pos)
        :m_gate(gate),
         m_font(font),
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

        m_gateText.setFont(font);
        m_gateText.setCharacterSize(18);
        m_gateText.setFillColor(sf::Color(248,250,252));
        m_gateText.setString(gateName());

        auto gb=m_gateText.getLocalBounds();
        m_gateText.setOrigin(
            gb.left+gb.width/2.f,
            gb.top+gb.height/2.f
        );

        m_idText.setFont(font);
        m_idText.setCharacterSize(11);
        m_idText.setFillColor(sf::Color(148,163,184));
        m_idText.setString(m_gate->getId());

        auto ib=m_idText.getLocalBounds();
        m_idText.setOrigin(
            ib.left+ib.width/2.f,
            ib.top+ib.height/2.f
        );

        size_t count=(gate->getType()==Op::NOT)?1:2;

        m_inputs.resize(count);

        for(auto& pin:m_inputs){

            pin.setRadius(PIN_R);
            pin.setFillColor(sf::Color(100,116,139));
        }

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
    sf::Vector2f getPosition() const { return m_position; }

    void setHovered(bool h){m_hover=h;}
    void setSelected(bool s){m_selected=s;}
    void setDragging(bool d){m_dragging=d;}

    sf::Vector2f getInputPinPosition(size_t i) const{

        return {
            m_inputs[i].getPosition().x+PIN_R,
            m_inputs[i].getPosition().y+PIN_R
        };
    }

    sf::Vector2f getOutputPinPosition() const{

        return {
            m_output.getPosition().x+PIN_R,
            m_output.getPosition().y+PIN_R
        };
    }
    size_t getInputCount() const{
        return m_inputs.size();
    }

    bool contains(sf::Vector2f p) const{

        return m_body.getGlobalBounds().contains(p);
    }

    Gate* getLogicalGate() const{return m_gate;}

    void render(sf::RenderWindow& window){

        if(m_hover)
            m_highlight.setOutlineColor(sf::Color(56,189,248));
        else if(m_selected)
            m_highlight.setOutlineColor(sf::Color(34,211,238));
        else
            m_highlight.setOutlineColor(sf::Color::Transparent);

        bool high=m_gate->getOutput();

        if(high){

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

        for(auto& p:m_inputs)
            window.draw(p);

        window.draw(m_outputGlow);
        window.draw(m_output);

        window.draw(m_gateText);
        window.draw(m_idText);
    }
};

}