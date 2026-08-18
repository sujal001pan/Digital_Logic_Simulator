// ============================================================================
// File: GraphicalWire.h
// Path: src/gui/GraphicalWire.h
// Purpose: Professional orthogonal wire rendering with glow and junctions.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include "../core/Wire.h"

namespace dls {

class GraphicalWire {
private:
    Wire* m_wire = nullptr;

    sf::Vector2f m_start;
    sf::Vector2f m_end;

    bool m_preview = false;
    bool m_selected = false;

    float m_wireThickness = 3.f;
    float m_glowThickness = 7.f;

    const sf::Color OFF_COLOR = sf::Color(100,116,139);
    const sf::Color ON_COLOR  = sf::Color(34,197,94);
    const sf::Color GLOW_COLOR = sf::Color(34,197,94,70);

private:

    bool signalHigh() const {

        if(!m_wire)
            return false;

        auto* src = m_wire->getSource();

        return src ? src->getOutput() : false;
    }

    std::vector<sf::Vector2f> routePoints() const {

        std::vector<sf::Vector2f> pts;

        pts.push_back(m_start);

        float midX=(m_start.x+m_end.x)/2.f;

        pts.push_back({midX,m_start.y});
        pts.push_back({midX,m_end.y});

        pts.push_back(m_end);

        return pts;
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

    void drawSegment(sf::RenderWindow& window,
                     sf::Vector2f a,
                     sf::Vector2f b,
                     float thickness,
                     sf::Color color) const {

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

public:

    explicit GraphicalWire(Wire* wire)
        :m_wire(wire){}

    void setEndpoints(sf::Vector2f start,
                      sf::Vector2f end){

        m_start=start;
        m_end=end;
    }

    void setPreview(bool preview){
        m_preview=preview;
    }

    void setSelected(bool selected){
        m_selected=selected;
    }

    // True if p is within `threshold` pixels of any segment of this wire.
    bool hitTest(sf::Vector2f p, float threshold=6.f) const {

        auto pts=routePoints();

        for(size_t i=0;i+1<pts.size();++i){

            if(distanceToSegment(p,pts[i],pts[i+1])<threshold)
                return true;
        }

        return false;
    }

    void render(sf::RenderWindow& window){

        bool high=signalHigh();

        sf::Color base=high?ON_COLOR:OFF_COLOR;

        if(m_preview)
            base=sf::Color(56,189,248);

        auto pts=routePoints();

        for(size_t i=0;i+1<pts.size();i++){

            if(high)
                drawSegment(window,
                            pts[i],
                            pts[i+1],
                            m_glowThickness,
                            GLOW_COLOR);

            drawSegment(window,
                        pts[i],
                        pts[i+1],
                        m_wireThickness,
                        base);
        }

        for(size_t i=1;i+1<pts.size();i++){

            sf::CircleShape node(4.f);

            node.setOrigin(4.f,4.f);
            node.setPosition(pts[i]);
            node.setFillColor(base);

            window.draw(node);
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

    Wire* getLogicalWire() const{
        return m_wire;
    }

    const sf::Vector2f& getStartPoint() const{
        return m_start;
    }

    const sf::Vector2f& getEndPoint() const{
        return m_end;
    }
};

}