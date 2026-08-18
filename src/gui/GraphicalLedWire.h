// ============================================================================
// File: GraphicalLedWire.h
// Path: src/gui/GraphicalLedWire.h
// Purpose: Visual-only wire for connections that terminate at an LED.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include "dls/Types.h"
#include "../core/Peripherals.h"

namespace dls {

class GraphicalLedWire {
private:
    ISignalSource* m_source=nullptr;
    LED* m_led=nullptr;

    sf::Vector2f m_start;
    sf::Vector2f m_end;

    bool m_selected=false;

    float m_wireThickness=3.f;
    float m_glowThickness=7.f;

    const sf::Color OFF_COLOR=sf::Color(100,116,139);
    const sf::Color ON_COLOR=sf::Color(34,197,94);
    const sf::Color GLOW_COLOR=sf::Color(34,197,94,70);

    void drawSegment(sf::RenderWindow& window,
                     sf::Vector2f a,
                     sf::Vector2f b,
                     float thickness,
                     sf::Color color) const{

        sf::Vector2f d=b-a;
        float len=std::sqrt(d.x*d.x+d.y*d.y);

        if(len<1.f)
            return;

        sf::RectangleShape line({len,thickness});
        line.setOrigin(0,thickness/2.f);
        line.setPosition(a);

        float angle=std::atan2(d.y,d.x)*180.f/3.14159265f;
        line.setRotation(angle);
        line.setFillColor(color);

        window.draw(line);
    }

    static float distanceToSegment(sf::Vector2f p,
                                   sf::Vector2f a,
                                   sf::Vector2f b){

        sf::Vector2f ab=b-a;
        float lenSq=ab.x*ab.x+ab.y*ab.y;

        float t=(lenSq>0.0001f)
            ?((p.x-a.x)*ab.x+(p.y-a.y)*ab.y)/lenSq
            :0.f;

        t=std::max(0.f,std::min(1.f,t));

        sf::Vector2f proj=a+ab*t;
        sf::Vector2f d=p-proj;

        return std::sqrt(d.x*d.x+d.y*d.y);
    }

public:

    GraphicalLedWire(ISignalSource* source,
                     LED* led,
                     sf::Vector2f start,
                     sf::Vector2f end)
        :m_source(source),m_led(led),m_start(start),m_end(end){}

    ISignalSource* getSource() const{return m_source;}
    LED* getLED() const{return m_led;}

    void setSelected(bool s){m_selected=s;}

    bool hitTest(sf::Vector2f p, float threshold=6.f) const{

        float midX=(m_start.x+m_end.x)/2.f;
        sf::Vector2f mid1{midX,m_start.y};
        sf::Vector2f mid2{midX,m_end.y};

        sf::Vector2f pts[4]={m_start,mid1,mid2,m_end};

        for(int i=0;i<3;i++){
            if(distanceToSegment(p,pts[i],pts[i+1])<threshold)
                return true;
        }

        return false;
    }

    void render(sf::RenderWindow& window){

        bool high=m_source?m_source->getOutput():false;

        sf::Color base=high?ON_COLOR:OFF_COLOR;

        float midX=(m_start.x+m_end.x)/2.f;
        sf::Vector2f mid1{midX,m_start.y};
        sf::Vector2f mid2{midX,m_end.y};

        sf::Vector2f pts[4]={m_start,mid1,mid2,m_end};

        for(int i=0;i<3;i++){

            if(high)
                drawSegment(window,pts[i],pts[i+1],m_glowThickness,GLOW_COLOR);

            drawSegment(window,pts[i],pts[i+1],m_wireThickness,base);
        }

        if(m_selected){

            sf::CircleShape halo(8.f);
            halo.setOrigin(8.f,8.f);
            halo.setFillColor(sf::Color::Transparent);
            halo.setOutlineThickness(2.f);
            halo.setOutlineColor(sf::Color(34,211,238));

            halo.setPosition(m_start);
            window.draw(halo);

            halo.setPosition(m_end);
            window.draw(halo);
        }
    }
};

}