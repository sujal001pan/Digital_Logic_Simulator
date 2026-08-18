#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace dls {

class ExpressionInputBox {
private:
    sf::RectangleShape m_box;
    sf::RectangleShape m_evalButton;
    sf::Text m_displayText;
    sf::Text m_placeholderText;
    sf::Text m_buttonText;
    sf::Text m_errorText;
    sf::RectangleShape m_cursor;

    std::string m_text;
    std::string m_errorMessage;
    
    bool m_isFocused = true;
    bool m_evaluateTriggered = false;
    sf::Clock m_cursorClock;

    void updateTextDisplay();

public:
    ExpressionInputBox(const sf::Font& font, sf::Vector2f position, sf::Vector2f size);

    void handleEvent(const sf::Event& event, sf::Vector2f mousePos);
    void update();
    void render(sf::RenderWindow& window);

    const std::string& getText() const { return m_text; }
    void setText(const std::string& text);

    void setErrorMessage(const std::string& msg);
    void clearErrorMessage();

    bool isEvaluateRequested();
};

} // namespace dls