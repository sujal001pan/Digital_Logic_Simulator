#include "OutputPanels.h"

namespace dls {

OutputPanels::OutputPanels(const sf::Font& font)
    : m_font(font) {}

void OutputPanels::drawTruthTable(sf::RenderWindow& window,
                                   const std::vector<TruthRow>& table,
                                   const std::vector<std::string>& variables,
                                   sf::Vector2f position) {
    if (variables.empty() || table.empty()) return;

    float cellWidth = 35.f;
    float cellHeight = 25.f;

    sf::RectangleShape headerBg(sf::Vector2f((variables.size() + 1) * cellWidth, cellHeight));
    headerBg.setPosition(position);
    headerBg.setFillColor(sf::Color(30, 41, 59));
    headerBg.setOutlineColor(sf::Color(71, 85, 105));
    headerBg.setOutlineThickness(1.f);
    window.draw(headerBg);

    for (size_t i = 0; i < variables.size(); ++i) {
        sf::Text label(variables[i], m_font, 14);
        label.setFillColor(sf::Color(148, 163, 184));
        label.setPosition(position.x + i * cellWidth + 10.f, position.y + 3.f);
        window.draw(label);
    }
    sf::Text outLabel("F", m_font, 14);
    outLabel.setFillColor(sf::Color(96, 165, 250));
    outLabel.setPosition(position.x + variables.size() * cellWidth + 10.f, position.y + 3.f);
    window.draw(outLabel);

    for (size_t r = 0; r < table.size(); ++r) {
        float rowY = position.y + (r + 1) * cellHeight;

        sf::RectangleShape rowBg(sf::Vector2f((variables.size() + 1) * cellWidth, cellHeight));
        rowBg.setPosition(position.x, rowY);
        rowBg.setFillColor((r % 2 == 0) ? sf::Color(15, 23, 42) : sf::Color(30, 41, 59));
        rowBg.setOutlineColor(sf::Color(51, 65, 85));
        rowBg.setOutlineThickness(1.f);
        window.draw(rowBg);

        for (size_t c = 0; c < table[r].inputs.size(); ++c) {
            sf::Text valText(table[r].inputs[c] ? "1" : "0", m_font, 13);
            valText.setFillColor(sf::Color::White);
            valText.setPosition(position.x + c * cellWidth + 12.f, rowY + 3.f);
            window.draw(valText);
        }

        sf::Text outText(table[r].output ? "1" : "0", m_font, 13);
        outText.setFillColor(table[r].output ? sf::Color(74, 222, 128) : sf::Color(248, 113, 113));
        outText.setPosition(position.x + variables.size() * cellWidth + 12.f, rowY + 3.f);
        window.draw(outText);
    }
}

void OutputPanels::drawKMap(sf::RenderWindow& window,
                            const std::vector<std::vector<KMapCell>>& kmap,
                            const std::vector<Grouping>& groups,
                            const std::vector<std::string>& variables,
                            sf::Vector2f position) {
    if (kmap.empty() || kmap[0].empty() || variables.empty()) return;

    size_t numVars = variables.size();
    size_t rows = kmap.size();
    size_t cols = kmap[0].size();
    float cellSize = 45.f;
    float startX = position.x + 60.f;
    float startY = position.y + 30.f;

    // --- 1. Draw Axis Label ---
    std::string axisText = "";
    if (numVars == 1)      axisText = "\\ " + variables[0];
    else if (numVars == 2) axisText = variables[0] + " \\ " + variables[1];
    else if (numVars == 3) axisText = variables[0] + " \\ " + variables[1] + variables[2];
    else if (numVars == 4) axisText = variables[0] + variables[1] + " \\ " + variables[2] + variables[3];

    sf::Text corner(axisText, m_font, 13);
    corner.setFillColor(sf::Color(148, 163, 184));
    corner.setPosition(position.x, position.y);
    window.draw(corner);

    // --- 2. Determine Gray Code Sequences ---
    std::vector<std::string> gray1 = {"0", "1"};
    std::vector<std::string> gray2 = {"00", "01", "11", "10"};
    
    std::vector<std::string> rowLabels = (rows == 4) ? gray2 : gray1;
    std::vector<std::string> colLabels = (cols == 4) ? gray2 : gray1;

    // --- 3. Draw Column Headers ---
    for (size_t c = 0; c < cols; ++c) {
        sf::Text colText(colLabels[c], m_font, 13);
        colText.setFillColor(sf::Color(96, 165, 250));
        colText.setPosition(startX + c * cellSize + 12.f, position.y + 10.f);
        window.draw(colText);
    }

    // --- 4. Draw Row Headers ---
    for (size_t r = 0; r < rows; ++r) {
        sf::Text rowText(rowLabels[r], m_font, 13);
        rowText.setFillColor(sf::Color(96, 165, 250));
        rowText.setPosition(position.x + 20.f, startY + r * cellSize + 12.f);
        window.draw(rowText);
    }

    // --- 5. Draw the K-Map Grid Cells ---
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            // Draw Background Rectangle
            sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
            cell.setPosition(startX + c * cellSize, startY + r * cellSize);
            cell.setFillColor(sf::Color(30, 41, 59));     // Dark slate color
            cell.setOutlineColor(sf::Color(71, 85, 105)); // Outline border
            cell.setOutlineThickness(1.f);
            window.draw(cell);

            // Read the boolean value from KMapCell
            bool cellValue = kmap[r][c].value; 
            
            // Draw Text (1 or 0)
            sf::Text valText(cellValue ? "1" : "0", m_font, 16);
            valText.setFillColor(cellValue ? sf::Color(74, 222, 128) : sf::Color(148, 163, 184));
            valText.setPosition(startX + c * cellSize + 16.f, startY + r * cellSize + 12.f);
            window.draw(valText);
        }
    }
}
void OutputPanels::drawExpressions(sf::RenderWindow& window,
                                  const std::string& sop,
                                  const std::string& pos,
                                  const std::string& simplified,
                                  sf::Vector2f position) {
    sf::Text tSop("Canonical SOP :  " + (sop.empty() ? "-" : sop), m_font, 14);
    tSop.setFillColor(sf::Color(226, 232, 240));
    tSop.setPosition(position);
    window.draw(tSop);

    sf::Text tPos("Canonical POS :  " + (pos.empty() ? "-" : pos), m_font, 14);
    tPos.setFillColor(sf::Color(226, 232, 240));
    tPos.setPosition(position.x, position.y + 24.f);
    window.draw(tPos);

    sf::Text tSimp("Simplified    :  " + (simplified.empty() ? "-" : simplified), m_font, 15);
    tSimp.setFillColor(sf::Color(74, 222, 128));
    tSimp.setPosition(position.x, position.y + 50.f);
    window.draw(tSimp);
}

} // namespace dls