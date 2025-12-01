#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <vector>

class Button {
public:
    Button(const std::string& text, const sf::Font& font, unsigned int characterSize, const sf::Vector2f& position, const sf::Vector2f& size);
    void draw(sf::RenderWindow& window);
    void drawWithOffset(sf::RenderWindow& window, float offsetY);
    bool isClicked(const sf::Vector2i& mousePos);
    void setCallback(std::function<void()> callback);
    void executeCallback();
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> callback;
};

class TextDisplay {
public:
    TextDisplay(const std::string& text, const sf::Font& font, unsigned int characterSize, const sf::Vector2f& position, sf::Color color = sf::Color::White);
    void draw(sf::RenderWindow& window);
    void setText(const std::string& newText);
    void setPosition(const sf::Vector2f& position);

private:
    sf::Text text;
};

class Menu {
public:
    Menu(sf::RenderWindow& window, const sf::Font& font);
    void addButton(const std::string& text, const sf::Vector2f& position, const sf::Vector2f& size, std::function<void()> callback);
    void handleEvent(const sf::Event& event);
    void draw();
    void clear();
    void setScrollable(bool scrollable, float maxHeight = 400.0f);

private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<Button> buttons;
    std::vector<TextDisplay> texts;
    bool isScrollable = false;
    float scrollOffset = 0.0f;
    float maxVisibleHeight = 400.0f;
    float totalHeight = 0.0f;
};