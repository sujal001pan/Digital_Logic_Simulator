// ============================================================================
// File: InteractionPanel.h
// Path: src/gui/InteractionPanel.h
// Purpose: Modern component palette for the Digital Logic Simulator.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "dls/Types.h"

namespace dls {

struct PanelButton{

    sf::RectangleShape shadow;
    sf::RectangleShape body;
    sf::CircleShape icon;
    sf::Text label;

    Op operation=Op::AND;

    bool peripheral=false;
    std::string peripheralType;

    bool hovered=false;
    bool selected=false;
};

class InteractionPanel{

private:

    sf::RectangleShape m_background;
    sf::RectangleShape m_divider;

    sf::Text m_title;
    sf::Text m_gateHeader;
    sf::Text m_ioHeader;

    const sf::Font& m_font;

    std::vector<PanelButton> m_buttons;

    float m_width=220.f;

private:

    void createButton(const std::string& text,
                      Op op,
                      float y,
                      sf::Color iconColor,
                      bool peripheral=false,
                      const std::string& type=""){

        PanelButton btn;

        btn.shadow.setSize({170.f,42.f});
        btn.shadow.setPosition({25.f,y+3.f});
        btn.shadow.setFillColor(sf::Color(0,0,0,35));

        btn.body.setSize({170.f,42.f});
        btn.body.setPosition({25.f,y});
        btn.body.setFillColor(sf::Color(30,41,59));
        btn.body.setOutlineThickness(2.f);
        btn.body.setOutlineColor(sf::Color::Transparent);

        btn.icon.setRadius(7.f);
        btn.icon.setOrigin(7.f,7.f);
        btn.icon.setPosition({45.f,y+21.f});
        btn.icon.setFillColor(iconColor);

        btn.label.setFont(m_font);
        btn.label.setCharacterSize(15);
        btn.label.setFillColor(sf::Color(248,250,252));
        btn.label.setString(text);
        btn.label.setPosition({60.f,y+10.f});

        btn.operation=op;
        btn.peripheral=peripheral;
        btn.peripheralType=type;

        m_buttons.push_back(btn);
    }

public:

    InteractionPanel(const sf::Font& font,float height)
        :m_font(font){

        m_background.setSize({m_width,height});
        m_background.setFillColor(sf::Color(17,24,39));

        m_divider.setSize({1,height});
        m_divider.setPosition({m_width-1,0});
        m_divider.setFillColor(sf::Color(51,65,85));

        m_title.setFont(font);
        m_title.setCharacterSize(22);
        m_title.setFillColor(sf::Color(248,250,252));
        m_title.setString("Logic Studio");
        m_title.setPosition({24.f,18.f});

        m_gateHeader.setFont(font);
        m_gateHeader.setCharacterSize(13);
        m_gateHeader.setFillColor(sf::Color(148,163,184));
        m_gateHeader.setString("LOGIC GATES");
        m_gateHeader.setPosition({24.f,70.f});

        float y=95.f;

        createButton("AND",Op::AND,y,sf::Color(56,189,248));
        y+=52.f;

        createButton("OR",Op::OR,y,sf::Color(56,189,248));
        y+=52.f;

        createButton("NOT",Op::NOT,y,sf::Color(56,189,248));
        y+=52.f;

        createButton("NAND",Op::NAND,y,sf::Color(56,189,248));
        y+=52.f;

        createButton("NOR",Op::NOR,y,sf::Color(56,189,248));
        y+=52.f;

        createButton("XOR",Op::XOR,y,sf::Color(56,189,248));
        y+=52.f;

        createButton("XNOR",Op::XNOR,y,sf::Color(56,189,248));
        y+=65.f;

        m_ioHeader.setFont(font);
        m_ioHeader.setCharacterSize(13);
        m_ioHeader.setFillColor(sf::Color(148,163,184));
        m_ioHeader.setString("INPUT / OUTPUT");
        m_ioHeader.setPosition({24.f,y-30.f});

        createButton("Input Switch",
                     Op::AND,
                     y,
                     sf::Color(251,191,36),
                     true,
                     "SWITCH");
        y+=52.f;

        createButton("LED",
                     Op::AND,
                     y,
                     sf::Color(239,68,68),
                     true,
                     "LED");

                y+=52.f;

        createButton("Delete Selected",
                    Op::AND,
                    y,
                    sf::Color(248,113,113),
                    true,
                    "DELETE");
    }

    void updateHover(sf::Vector2f mouse){

        for(auto& b:m_buttons){

            b.hovered=b.body.getGlobalBounds().contains(mouse);

            if(b.selected)
                b.body.setOutlineColor(sf::Color(34,211,238));

            else if(b.hovered)
                b.body.setOutlineColor(sf::Color(56,189,248));

            else
                b.body.setOutlineColor(sf::Color::Transparent);
        }
    }

    bool checkClick(sf::Vector2f mouse,
                    Op& op,
                    std::string& type){

        for(auto& b:m_buttons){

            if(b.body.getGlobalBounds().contains(mouse)){

                for(auto& x:m_buttons)
                    x.selected=false;

                b.selected=true;

                if(b.peripheral){

                    type=b.peripheralType;
                }
                else{

                    op=b.operation;
                    type="GATE";
                }

                return true;
            }
        }

        return false;
    }

    void render(sf::RenderWindow& window){

        window.draw(m_background);
        window.draw(m_divider);

        window.draw(m_title);
        window.draw(m_gateHeader);
        window.draw(m_ioHeader);

        for(auto& b:m_buttons){

            window.draw(b.shadow);
            window.draw(b.body);
            window.draw(b.icon);
            window.draw(b.label);
        }
    }

    float getWidth() const{
        return m_width;
    }
};

}