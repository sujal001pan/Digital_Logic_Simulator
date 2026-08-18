#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "dls/Types.h"

namespace dls {

class OutputPanels {
private:
    const sf::Font& m_font;

public:
    explicit OutputPanels(const sf::Font& font);

    void drawTruthTable(sf::RenderWindow& window,
                        const std::vector<TruthRow>& table,
                        const std::vector<std::string>& variables,
                        sf::Vector2f position);


    void drawKMap(sf::RenderWindow& window,
                  const std::vector<std::vector<KMapCell>>& kmap,
                  const std::vector<Grouping>& groups,
                  const std::vector<std::string>& variables, // <-- Make sure this is a string vector
                  sf::Vector2f position);

    void drawExpressions(sf::RenderWindow& window,
                         const std::string& sop,
                         const std::string& pos,
                         const std::string& simplified,
                         sf::Vector2f position);
};

} // namespace dls