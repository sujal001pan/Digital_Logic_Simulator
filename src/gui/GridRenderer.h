// ============================================================================
// File: GridRenderer.h
// Path: src/gui/GridRenderer.h
// Purpose: Professional infinite blueprint grid renderer.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

namespace dls {

class GridRenderer {
private:
    float m_gridSize = 40.f;

    const sf::Color BACKGROUND   = sf::Color(15,23,42);   // #0F172A
    const sf::Color MINOR_GRID   = sf::Color(30,41,59);   // #1E293B
    const sf::Color MAJOR_GRID   = sf::Color(51,65,85);   // #334155
    const sf::Color AXIS_COLOR   = sf::Color(56,189,248,90);

public:

    explicit GridRenderer(float gridSize=40.f)
        :m_gridSize(gridSize){}

    void render(sf::RenderWindow& window,
                const sf::View& view){

        // -----------------------------------------------------------------
        // Background
        // -----------------------------------------------------------------

        sf::RectangleShape bg;

        bg.setSize({
            view.getSize().x,
            view.getSize().y
        });

        bg.setPosition(
            view.getCenter()-view.getSize()/2.f
        );

        bg.setFillColor(BACKGROUND);

        window.draw(bg);

        // -----------------------------------------------------------------
        // Infinite Grid
        // -----------------------------------------------------------------

        sf::FloatRect visible(
            view.getCenter()-view.getSize()/2.f,
            view.getSize()
        );

        float startX=std::floor(visible.left/m_gridSize)*m_gridSize;
        float endX=visible.left+visible.width;

        float startY=std::floor(visible.top/m_gridSize)*m_gridSize;
        float endY=visible.top+visible.height;

        sf::VertexArray lines(sf::Lines);

        // Vertical lines

        for(float x=startX;x<=endX;x+=m_gridSize){

            int index=(int)std::round(x/m_gridSize);

            sf::Color c=
                (index%5==0)
                ?MAJOR_GRID
                :MINOR_GRID;

            lines.append({
                sf::Vector2f(x+0.5f,startY),
                c
            });

            lines.append({
                sf::Vector2f(x+0.5f,endY),
                c
            });
        }

        // Horizontal lines

        for(float y=startY;y<=endY;y+=m_gridSize){

            int index=(int)std::round(y/m_gridSize);

            sf::Color c=
                (index%5==0)
                ?MAJOR_GRID
                :MINOR_GRID;

            lines.append({
                sf::Vector2f(startX,y+0.5f),
                c
            });

            lines.append({
                sf::Vector2f(endX,y+0.5f),
                c
            });
        }

        window.draw(lines);

        // -----------------------------------------------------------------
        // Center Axes
        // -----------------------------------------------------------------

        sf::Vertex axes[]={
            {
                sf::Vector2f(0,startY),
                AXIS_COLOR
            },
            {
                sf::Vector2f(0,endY),
                AXIS_COLOR
            },
            {
                sf::Vector2f(startX,0),
                AXIS_COLOR
            },
            {
                sf::Vector2f(endX,0),
                AXIS_COLOR
            }
        };

        window.draw(axes,2,sf::Lines);
        window.draw(axes+2,2,sf::Lines);
    }

    sf::Vector2f snapToGrid(sf::Vector2f p) const{

        return{
            std::round(p.x/m_gridSize)*m_gridSize,
            std::round(p.y/m_gridSize)*m_gridSize
        };
    }

    void setGridSize(float size){

        if(size>=20.f && size<=80.f)
            m_gridSize=size;
    }

    float getGridSize() const{
        return m_gridSize;
    }
};

}