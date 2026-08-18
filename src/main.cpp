#include <iostream>
#include <SFML/Graphics.hpp>

#include "core/Circuit.h"
#include "core/SimulationEngine.h"

#include "gui/GUIManager.h"
#include "gui/GridRenderer.h"
#include "gui/InteractionPanel.h"
#include "gui/InteractionController.h"

#include "gui/ExpressionInputBox.h"
#include "gui/OutputPanels.h"
#include "core/AnalyticalEngine.h"

enum class AppMode {
    Circuit,
    Expression
};

int main() {

    try {

        // ------------------------------------------------------------
        // Backend
        // ------------------------------------------------------------

        dls::Circuit circuit;
        dls::SimulationEngine engine;

        engine.setCircuit(&circuit);

        // ------------------------------------------------------------
        // GUI
        // ------------------------------------------------------------

        dls::GUIManager gui(
            1280,
            720,
            "Digital Logic Simulator"
        );

        sf::Font font;

        if (!font.loadFromFile("resources/fonts/Inter-Regular.ttf")) {
            std::cerr << "Unable to load font.\n";
            return 1;
        }

        dls::GridRenderer grid(40.f);

        dls::InteractionPanel panel(
            font,
            gui.window().getSize().y
        );

        dls::InteractionController controller(
            &circuit,
            &engine,
            &panel,
            &grid
        );

        // ------------------------------------------------------------
        // Analytical Mode Setup
        // ------------------------------------------------------------

        AppMode currentMode = AppMode::Circuit;

        dls::ExpressionInputBox inputBox(
            font, 
            sf::Vector2f(220.f, 30.f), 
            sf::Vector2f(500.f, 38.f)
        );
        dls::OutputPanels outputPanels(font);
        dls::AnalyticalEngine analyticalEngine;
        dls::AnalyticalResults analyticalResults;

        // ------------------------------------------------------------
        // Main Loop
        // ------------------------------------------------------------

        gui.run(

            [&](const sf::Event& event) {

                // Press TAB to toggle between Circuit Builder and Expression Mode
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
                    currentMode = (currentMode == AppMode::Circuit) ? AppMode::Expression : AppMode::Circuit;
                }

                if (currentMode == AppMode::Expression) {
                    sf::Vector2f mousePos = gui.window().mapPixelToCoords(sf::Mouse::getPosition(gui.window()));
                    inputBox.handleEvent(event, mousePos);
                } else {
                    controller.handleEvent(
                        event,
                        gui.window(),
                        font
                    );
                }
            },

            [&](float dt) {

                if (currentMode == AppMode::Expression) {
                    if (inputBox.isEvaluateRequested()) {
                        try {
                            analyticalResults = analyticalEngine.processExpression(inputBox.getText());
                            inputBox.clearErrorMessage();
                        } catch (const std::exception& e) {
                            inputBox.setErrorMessage(e.what());
                        }
                    }
                    inputBox.update();
                } else {
                    engine.step();
                }
            },

            [&](sf::RenderWindow& window) {

                if (currentMode == AppMode::Expression) {
                    // Switch to static UI view for analytical overlay rendering
                    window.setView(window.getDefaultView());

                    inputBox.render(window);

                    if (!analyticalResults.variables.empty()) {
                        outputPanels.drawTruthTable(
                            window, 
                            analyticalResults.truthTable, 
                            analyticalResults.variables, 
                            sf::Vector2f(50.f, 100.f)
                        );
                            outputPanels.drawKMap(
                                window, 
                                analyticalResults.kmap, 
                                analyticalResults.groupings, 
                                analyticalResults.variables,
                                sf::Vector2f(350.f, 100.f)
                            );
                        outputPanels.drawExpressions(
                            window, 
                            analyticalResults.canonicalSOP, 
                            analyticalResults.canonicalPOS, 
                            analyticalResults.simplifiedSOP, 
                            sf::Vector2f(50.f, 520.f)
                        );
                    }
                } else {
                    // Circuit Canvas Rendering
                    grid.render(
                        window,
                        gui.view()
                    );

                    panel.updateHover(
                        window.mapPixelToCoords(
                            sf::Mouse::getPosition(window))
                    );

                    panel.render(window);

                    controller.render(window);
                }
            }
        );

    }
    catch (const std::exception& e) {

        std::cerr << "Fatal Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}