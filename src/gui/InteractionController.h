// ============================================================================
// File: InteractionController.h
// Path: src/gui/InteractionController.h
// Purpose: Professional interaction manager for the workspace.
// ============================================================================

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cmath>
#include <algorithm>

#include "../core/Circuit.h"
#include "../core/SimulationEngine.h"
#include "GraphicalGate.h"
#include "GraphicalWire.h"
#include "GraphicalSwitch.h"
#include "GraphicalLED.h"
#include "GraphicalLedWire.h"
#include "GridRenderer.h"
#include "InteractionPanel.h"


namespace dls {
namespace {
    float pinDistance(sf::Vector2f a, sf::Vector2f b){
        sf::Vector2f d=a-b;
        return std::sqrt(d.x*d.x+d.y*d.y);
    }
    constexpr float PIN_HIT_RADIUS=8.f;
}
class InteractionController {

private:

    Circuit* m_circuit;
    SimulationEngine* m_engine;
    InteractionPanel* m_panel;
    GridRenderer* m_grid;

    std::vector<std::unique_ptr<GraphicalGate>> m_gates;
    std::vector<std::unique_ptr<GraphicalWire>> m_wires;
    std::vector<std::unique_ptr<GraphicalSwitch>> m_switches;
    std::vector<std::unique_ptr<GraphicalLED>> m_leds;
    std::vector<std::unique_ptr<GraphicalLedWire>> m_ledWires;

    GraphicalGate* m_selectedGate=nullptr;
    GraphicalGate* m_hoverGate=nullptr;

    bool m_dragging=false;
    sf::Vector2f m_dragOffset;

    GraphicalSwitch* m_selectedSwitch=nullptr;
    bool m_draggingSwitch=false;
    sf::Vector2f m_dragOffsetSwitch;
    sf::Vector2f m_switchPressPos;

    GraphicalLED* m_selectedLED=nullptr;
    bool m_draggingLED=false;
    sf::Vector2f m_dragOffsetLED;

    bool m_creatingWire=false;
    GraphicalGate* m_wireSourceGate=nullptr;
    GraphicalSwitch* m_wireSourceSwitch=nullptr;
    sf::Vector2f m_wirePreviewEnd;
    GraphicalWire* m_selectedWire=nullptr;
GraphicalLedWire* m_selectedLedWire=nullptr;

    int m_gateCounter=0;
    int m_wireCounter=0;
    int m_switchCounter=0;
    int m_ledCounter=0;

public:

    InteractionController(Circuit* circuit,
                          SimulationEngine* engine,
                          InteractionPanel* panel,
                          GridRenderer* grid)
        :m_circuit(circuit),
         m_engine(engine),
         m_panel(panel),
         m_grid(grid){}

    void handleEvent(const sf::Event& event,
                     sf::RenderWindow& window,
                     const sf::Font& font){

        sf::Vector2f mouse=
            window.mapPixelToCoords(
                sf::Mouse::getPosition(window));

        //--------------------------------------------------------------
        // Hover Detection
        //--------------------------------------------------------------

        m_hoverGate=nullptr;

        for(auto& gate:m_gates){

            gate->setHovered(false);

            if(gate->contains(mouse)){

                m_hoverGate=gate.get();
                gate->setHovered(true);
            }
        }

        for(auto& sw:m_switches){
            sw->setHovered(sw->contains(mouse));
        }

        for(auto& led:m_leds){
            led->setHovered(led->contains(mouse));
        }

        //--------------------------------------------------------------
        // Left Mouse Press
        //--------------------------------------------------------------
if(event.type==sf::Event::MouseButtonPressed &&
           event.mouseButton.button==sf::Mouse::Left){

            Op op;
            std::string type;

            if(m_panel->checkClick(mouse,op,type)){

            if(type=="GATE")
                spawnGate(op,font);
            else if(type=="SWITCH")
                spawnSwitch(font);
            else if(type=="LED")
                spawnLED(font);
            else if(type=="DELETE")
                deleteSelection();

                return;
            }

            //----------------------------------------------------------
            // Try to COMPLETE a wire already in progress
            //----------------------------------------------------------

            if(m_creatingWire){

                bool completed=false;

                // Check gate input pins.
                for(auto& gate:m_gates){

                    if(gate.get()==m_wireSourceGate)
                        continue;

                    for(size_t i=0;i<gate->getInputCount();++i){

                        if(pinDistance(mouse,gate->getInputPinPosition(i))<PIN_HIT_RADIUS){

                            finishWire(gate.get(),i);
                            completed=true;
                            break;
                        }
                    }

                    if(completed)
                        break;
                }

                // Check LED input pins.
                if(!completed){

                    for(auto& led:m_leds){

                        if(pinDistance(mouse,led->getInputPinPosition())<PIN_HIT_RADIUS){

                            finishWireToLED(led.get());
                            completed=true;
                            break;
                        }
                    }
                }

                if(!completed)
                    cancelWire();

                return;
            }

            //----------------------------------------------------------
            // Try to START a wire from an output pin (gate or switch)
            //----------------------------------------------------------

            for(auto& gate:m_gates){

                if(pinDistance(mouse,gate->getOutputPinPosition())<PIN_HIT_RADIUS){

                    m_creatingWire=true;
                    m_wireSourceGate=gate.get();
                    m_wireSourceSwitch=nullptr;
                    m_wirePreviewEnd=mouse;
                    return;
                }
            }

            for(auto& sw:m_switches){

                if(pinDistance(mouse,sw->getOutputPinPosition())<PIN_HIT_RADIUS){

                    m_creatingWire=true;
                    m_wireSourceSwitch=sw.get();
                    m_wireSourceGate=nullptr;
                    m_wirePreviewEnd=mouse;
                    return;
                }
            }

            //----------------------------------------------------------
            // Press on a switch body: begin drag. Whether this ends up
            // being a "toggle" or a "move" is decided on mouse release,
            // based on how far the mouse actually moved.
            //----------------------------------------------------------

            for(auto& sw:m_switches){

                if(sw->contains(mouse)){
                    deselectAll();
                    m_selectedSwitch=sw.get();
                    m_draggingSwitch=true;
                    m_switchPressPos=mouse;

                    m_dragOffsetSwitch=
                        sw->getPosition()-mouse;

                    return;
                }
            }

            //----------------------------------------------------------
            // Press on an LED body: begin drag.
            //----------------------------------------------------------

            for(auto& led:m_leds){

                if(led->contains(mouse)){
                    deselectAll();

                    m_selectedLED=led.get();
                    m_draggingLED=true;

                    m_dragOffsetLED=
                        led->getPosition()-mouse;

                    return;
                }
            }

            //----------------------------------------------------------
            // Click on an existing wire (gate-to-gate or LED wire)
            //----------------------------------------------------------

            for(auto& wire:m_wires){

                if(wire->hitTest(mouse)){
                    deselectAll();
                    m_selectedWire=wire.get();
                    wire->setSelected(true);
                    return;
                }
            }

            for(auto& lw:m_ledWires){

                if(lw->hitTest(mouse)){
                    deselectAll();
                    m_selectedLedWire=lw.get();
                    lw->setSelected(true);
                    return;
                }
            }

            //----------------------------------------------------------
            // Normal select / drag
            //----------------------------------------------------------

            if(m_hoverGate){

                selectGate(m_hoverGate);

                m_dragging=true;

                m_dragOffset=
                    m_hoverGate->getPosition()-mouse;
            }
            else{

                deselectAll();
            }
        }
        //--------------------------------------------------------------
        // Mouse Move
        //--------------------------------------------------------------

        if(event.type==sf::Event::MouseMoved){

            if(m_dragging && m_selectedGate){

                m_selectedGate->setPosition(
                    m_grid->snapToGrid(
                        mouse+m_dragOffset));
            }

            if(m_draggingSwitch && m_selectedSwitch){

                m_selectedSwitch->setPosition(
                    m_grid->snapToGrid(
                        mouse+m_dragOffsetSwitch));
            }

            if(m_draggingLED && m_selectedLED){

                m_selectedLED->setPosition(
                    m_grid->snapToGrid(
                        mouse+m_dragOffsetLED));
            }

            if(m_creatingWire){

                m_wirePreviewEnd=mouse;
            }
        }

        //--------------------------------------------------------------
        // Mouse Release
        //--------------------------------------------------------------

        if(event.type==sf::Event::MouseButtonReleased &&
           event.mouseButton.button==sf::Mouse::Left){

            m_dragging=false;

            if(m_draggingSwitch){

                m_draggingSwitch=false;

                // If the mouse barely moved, treat this as a tap (toggle)
                // rather than a drag (move).
                if(m_selectedSwitch &&
                   pinDistance(mouse,m_switchPressPos)<4.f){

                    m_selectedSwitch->toggle();

                    if(m_engine)
                        m_engine->step();
                }
            }

            m_draggingLED=false;
        }

        //--------------------------------------------------------------
        // Keyboard Shortcuts
        //--------------------------------------------------------------

        if(event.type==sf::Event::KeyPressed){

            switch(event.key.code){

            case sf::Keyboard::Delete:
                deleteSelection();
                break;

            case sf::Keyboard::Escape:
                cancelWire();
                break;

            case sf::Keyboard::Space:
                if(m_engine)
                    m_engine->pulseClock();
                break;

            default:
                break;
            }
        }
    }

    void render(sf::RenderWindow& window){

        for(auto& wire:m_wires)
            wire->render(window);

        for(auto& lw:m_ledWires)
            lw->render(window);

        if(m_creatingWire){

            sf::Vector2f startPos;

            if(m_wireSourceGate)
                startPos=m_wireSourceGate->getOutputPinPosition();
            else if(m_wireSourceSwitch)
                startPos=m_wireSourceSwitch->getOutputPinPosition();

            sf::Vertex preview[2]={
                sf::Vertex(
                    startPos,
                    sf::Color(56,189,248)),
                sf::Vertex(
                    m_wirePreviewEnd,
                    sf::Color(56,189,248))
            };

            window.draw(preview,2,sf::Lines);
        }

        for(auto& gate:m_gates)
            gate->render(window);

        for(auto& sw:m_switches)
            sw->render(window);

        for(auto& led:m_leds)
            led->render(window);
    }

private:

    // Staggers spawn positions in a simple cascade so new components
    // don't land exactly on top of previous ones. Still draggable
    // afterward to wherever you actually want them.
    sf::Vector2f nextSpawnPosition(int counter) const{

        int col=(counter-1)%5;
        int row=(counter-1)/5;

        return sf::Vector2f(260.f+col*40.f,120.f+row*40.f);
    }

    void spawnGate(Op op,const sf::Font& font){

        std::string id="G"+std::to_string(++m_gateCounter);

        std::unique_ptr<Gate> gate;

        switch(op){

        case Op::AND:
            gate=std::make_unique<ANDGate>(id);
            break;

        case Op::OR:
            gate=std::make_unique<ORGate>(id);
            break;

        case Op::NOT:
            gate=std::make_unique<NOTGate>(id);
            break;

        case Op::NAND:
            gate=std::make_unique<NANDGate>(id);
            break;

        case Op::NOR:
            gate=std::make_unique<NORGate>(id);
            break;

        case Op::XOR:
            gate=std::make_unique<XORGate>(id);
            break;

        case Op::XNOR:
            gate=std::make_unique<XNORGate>(id);
            break;
        }

        Gate* ptr=gate.get();

        m_circuit->addGate(std::move(gate),id);

        m_gates.push_back(
            std::make_unique<GraphicalGate>(
                ptr,
                font,
                nextSpawnPosition(m_gateCounter)));
    }

    void spawnSwitch(const sf::Font& font){

        std::string id="S"+std::to_string(++m_switchCounter);

        auto sw=std::make_unique<InputSwitch>(id);
        InputSwitch* ptr=sw.get();

        m_circuit->addSwitch(std::move(sw),id);

        m_switches.push_back(
            std::make_unique<GraphicalSwitch>(
                ptr,
                font,
                nextSpawnPosition(m_switchCounter)));
    }

    void spawnLED(const sf::Font& font){

        std::string id="L"+std::to_string(++m_ledCounter);

        auto led=std::make_unique<LED>(id);
        LED* ptr=led.get();

        m_circuit->addLED(std::move(led),id);

        m_leds.push_back(
            std::make_unique<GraphicalLED>(
                ptr,
                font,
                nextSpawnPosition(m_ledCounter)));
    }

    void selectGate(GraphicalGate* gate){

        deselectAll();

        m_selectedGate=gate;

        if(gate)
            gate->setSelected(true);
    }

    void deselectAll(){

    for(auto& gate:m_gates)
        gate->setSelected(false);

    m_selectedGate=nullptr;
    m_selectedSwitch=nullptr;
    m_selectedLED=nullptr;

    if(m_selectedWire)
        m_selectedWire->setSelected(false);
    m_selectedWire=nullptr;

    if(m_selectedLedWire)
        m_selectedLedWire->setSelected(false);
    m_selectedLedWire=nullptr;
}

void deleteSelection(){

    if(m_selectedGate){

        Gate* logical=m_selectedGate->getLogicalGate();

        removeGraphicalWiresReferencing(logical,logical);

        m_circuit->removeGate(logical);

        m_gates.erase(
            std::remove_if(m_gates.begin(),m_gates.end(),
                [this](const std::unique_ptr<GraphicalGate>& g){
                    return g.get()==m_selectedGate;
                }),
            m_gates.end());

        deselectAll();

        if(m_engine)
            m_engine->step();

        return;
    }

    if(m_selectedSwitch){

        InputSwitch* logical=m_selectedSwitch->getLogicalSwitch();

        removeGraphicalWiresReferencing(logical,nullptr);

        m_circuit->removeSwitch(logical);

        m_switches.erase(
            std::remove_if(m_switches.begin(),m_switches.end(),
                [this](const std::unique_ptr<GraphicalSwitch>& s){
                    return s.get()==m_selectedSwitch;
                }),
            m_switches.end());

        deselectAll();

        if(m_engine)
            m_engine->step();

        return;
    }

    if(m_selectedLED){

        LED* logical=m_selectedLED->getLogicalLED();

        m_ledWires.erase(
            std::remove_if(m_ledWires.begin(),m_ledWires.end(),
                [logical](const std::unique_ptr<GraphicalLedWire>& lw){
                    return lw->getLED()==logical;
                }),
            m_ledWires.end());

        m_circuit->removeLED(logical);

        m_leds.erase(
            std::remove_if(m_leds.begin(),m_leds.end(),
                [this](const std::unique_ptr<GraphicalLED>& l){
                    return l.get()==m_selectedLED;
                }),
            m_leds.end());

        deselectAll();
        return;
    }

    if(m_selectedWire){

        Wire* logical=m_selectedWire->getLogicalWire();

        m_circuit->removeWire(logical);

        m_wires.erase(
            std::remove_if(m_wires.begin(),m_wires.end(),
                [this](const std::unique_ptr<GraphicalWire>& w){
                    return w.get()==m_selectedWire;
                }),
            m_wires.end());

        deselectAll();

        if(m_engine)
            m_engine->step();

        return;
    }

    if(m_selectedLedWire){

        LED* led=m_selectedLedWire->getLED();

        if(led)
            led->disconnect();

        m_ledWires.erase(
            std::remove_if(m_ledWires.begin(),m_ledWires.end(),
                [this](const std::unique_ptr<GraphicalLedWire>& lw){
                    return lw.get()==m_selectedLedWire;
                }),
            m_ledWires.end());

        deselectAll();

        if(m_engine)
            m_engine->step();

        return;
    }
}

// Removes any GUI wire (gate-to-gate or gate/switch-to-LED) that touches
// the given signal source, and — for gates specifically — also any wire
// that has it as a destination. Must be called BEFORE the matching
// Circuit::remove*() call, while the logical Wire pointers are still valid.
void removeGraphicalWiresReferencing(ISignalSource* src, Gate* asDestination){

    m_wires.erase(
        std::remove_if(m_wires.begin(),m_wires.end(),
            [src,asDestination](const std::unique_ptr<GraphicalWire>& w){
                Wire* lw=w->getLogicalWire();
                return lw->getSource()==src ||
                       (asDestination!=nullptr && lw->getDestination()==asDestination);
            }),
        m_wires.end());

    m_ledWires.erase(
        std::remove_if(m_ledWires.begin(),m_ledWires.end(),
            [src](const std::unique_ptr<GraphicalLedWire>& lw){
                return lw->getSource()==src;
            }),
        m_ledWires.end());
}


    void cancelWire(){

        m_creatingWire=false;
        m_wireSourceGate=nullptr;
        m_wireSourceSwitch=nullptr;
    }

    void finishWire(GraphicalGate* destGate, size_t pinIndex){

        std::string id="W"+std::to_string(++m_wireCounter);

        ISignalSource* source=nullptr;
        sf::Vector2f startPos;

        if(m_wireSourceGate){
            source=m_wireSourceGate->getLogicalGate();
            startPos=m_wireSourceGate->getOutputPinPosition();
        }
        else if(m_wireSourceSwitch){
            source=m_wireSourceSwitch->getLogicalSwitch();
            startPos=m_wireSourceSwitch->getOutputPinPosition();
        }

        if(!source){
            cancelWire();
            return;
        }

        Gate* destination=destGate->getLogicalGate();

        auto wire=std::make_unique<Wire>(id,source,destination,(int)pinIndex);

        Wire* rawWire=m_circuit->addWire(std::move(wire));

        auto gw=std::make_unique<GraphicalWire>(rawWire);

        gw->setEndpoints(
            startPos,
            destGate->getInputPinPosition(pinIndex));

        m_wires.push_back(std::move(gw));

        if(m_engine)
            m_engine->step();

        cancelWire();
    }

    void finishWireToLED(GraphicalLED* destLed){

        ISignalSource* source=nullptr;
        sf::Vector2f startPos;

        if(m_wireSourceGate){
            source=m_wireSourceGate->getLogicalGate();
            startPos=m_wireSourceGate->getOutputPinPosition();
        }
        else if(m_wireSourceSwitch){
            source=m_wireSourceSwitch->getLogicalSwitch();
            startPos=m_wireSourceSwitch->getOutputPinPosition();
        }

        if(!source){
            cancelWire();
            return;
        }

        destLed->getLogicalLED()->connect(source);

        m_ledWires.push_back(
    std::make_unique<GraphicalLedWire>(
        source,
        destLed->getLogicalLED(),
        startPos,
        destLed->getInputPinPosition()));

        if(m_engine)
            m_engine->step();

        cancelWire();
    }
};

}