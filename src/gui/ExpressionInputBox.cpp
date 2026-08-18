#include "ExpressionInputBox.h"

namespace dls {

ExpressionInputBox::ExpressionInputBox(const sf::Font& font, sf::Vector2f position, sf::Vector2f size) {
    // Input box outline and background
    m_box.setPosition(position);
    m_box.setSize(size);
    m_box.setFillColor(sf::Color(30, 41, 59));       // Dark slate background
    m_box.setOutlineColor(sf::Color(96, 165, 250));   // Blue active border
    m_box.setOutlineThickness(2.f);

    // Active typed text
    m_displayText.setFont(font);
    m_displayText.setCharacterSize(18);
    m_displayText.setFillColor(sf::Color::White);
    m_displayText.setPosition(position.x + 10.f, position.y + (size.y - 24.f) / 2.f);

    // Placeholder guidance text
    m_placeholderText.setFont(font);
    m_placeholderText.setCharacterSize(18);
    m_placeholderText.setFillColor(sf::Color(148, 163, 184));
    m_placeholderText.setString("Enter expression (e.g. A.B + !C)...");
    m_placeholderText.setPosition(position.x + 10.f, position.y + (size.y - 24.f) / 2.f);

    // Evaluate button styling
    float btnWidth = 110.f;
    m_evalButton.setPosition(position.x + size.x + 10.f, position.y);
    m_evalButton.setSize(sf::Vector2f(btnWidth, size.y));
    m_evalButton.setFillColor(sf::Color(37, 99, 235));

    m_buttonText.setFont(font);
    m_buttonText.setCharacterSize(16);
    m_buttonText.setFillColor(sf::Color::White);
    m_buttonText.setString("Evaluate");
    
    sf::FloatRect bounds = m_buttonText.getLocalBounds();
    m_buttonText.setPosition(
        m_evalButton.getPosition().x + (btnWidth - bounds.width) / 2.f - bounds.left,
        m_evalButton.getPosition().y + (size.y - bounds.height) / 2.f - bounds.top - 2.f
    );

    // Parse Error text line below input box
    m_errorText.setFont(font);
    m_errorText.setCharacterSize(14);
    m_errorText.setFillColor(sf::Color(248, 113, 113)); // Red text for syntax errors
    m_errorText.setPosition(position.x, position.y + size.y + 6.f);

    // Blinking text cursor
    m_cursor.setSize(sf::Vector2f(2.f, 20.f));
    m_cursor.setFillColor(sf::Color::White);
}

void ExpressionInputBox::updateTextDisplay() {
    m_displayText.setString(m_text);
    
    // findCharacterPos returns sf::Vector2f (x, y)
    sf::Vector2f charPos = m_displayText.findCharacterPos(m_text.length());
    float cursorX = m_text.empty() ? m_box.getPosition().x + 10.f : charPos.x + 2.f;
    
    m_cursor.setPosition(cursorX, m_box.getPosition().y + (m_box.getSize().y - 20.f) / 2.f);
}

void ExpressionInputBox::setText(const std::string& text) {
    m_text = text;
    updateTextDisplay();
    clearErrorMessage();
}

void ExpressionInputBox::setErrorMessage(const std::string& msg) {
    m_errorMessage = msg;
    m_errorText.setString(m_errorMessage);
}

void ExpressionInputBox::clearErrorMessage() {
    m_errorMessage.clear();
    m_errorText.setString("");
}

bool ExpressionInputBox::isEvaluateRequested() {
    if (m_evaluateTriggered) {
        m_evaluateTriggered = false;
        return true;
    }
    return false;
}

void ExpressionInputBox::handleEvent(const sf::Event& event, sf::Vector2f mousePos) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (m_box.getGlobalBounds().contains(mousePos)) {
                m_isFocused = true;
                m_box.setOutlineColor(sf::Color(96, 165, 250));
            } else if (m_evalButton.getGlobalBounds().contains(mousePos)) {
                m_evaluateTriggered = true;
            } else {
                m_isFocused = false;
                m_box.setOutlineColor(sf::Color(71, 85, 105));
            }
        }
    }

    if (m_isFocused && event.type == sf::Event::TextEntered) {
        sf::Uint32 unicode = event.text.unicode;

        if (unicode == 8) { // Backspace
            if (!m_text.empty()) {
                m_text.pop_back();
                updateTextDisplay();
                clearErrorMessage();
            }
        } else if (unicode == 13 || unicode == 10) { // Enter key
            m_evaluateTriggered = true;
        } else if (unicode >= 32 && unicode < 128) { // Printable ASCII
            m_text += static_cast<char>(unicode);
            updateTextDisplay();
            clearErrorMessage();
        }
    }
}

void ExpressionInputBox::update() {
    if (m_cursorClock.getElapsedTime().asMilliseconds() > 500) {
        m_cursorClock.restart();
    }
}

void ExpressionInputBox::render(sf::RenderWindow& window) {
    window.draw(m_box);
    window.draw(m_evalButton);
    window.draw(m_buttonText);

    if (m_text.empty()) {
        window.draw(m_placeholderText);
    } else {
        window.draw(m_displayText);
    }

    if (m_isFocused && (m_cursorClock.getElapsedTime().asMilliseconds() < 500)) {
        window.draw(m_cursor);
    }

    if (!m_errorMessage.empty()) {
        window.draw(m_errorText);
    }
}

} // namespace dls