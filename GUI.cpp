#include "GUI.h"

Button::Button(const std::string &text, const sf::Font &font, unsigned int characterSize, const sf::Vector2f &position, const sf::Vector2f &size)
    : shape(size), text(text, font, characterSize)
{
    shape.setPosition(position);
    shape.setFillColor(sf::Color::Blue);
    this->text.setPosition(position.x + 10, position.y + 10);
    this->text.setFillColor(sf::Color::White);
}

void Button::draw(sf::RenderWindow &window)
{
    window.draw(shape);
    window.draw(text);
}

void Button::drawWithOffset(sf::RenderWindow &window, float offsetY)
{
    sf::Vector2f originalShapePos = shape.getPosition();
    sf::Vector2f originalTextPos = text.getPosition();

    shape.setPosition(originalShapePos.x, originalShapePos.y - offsetY);
    text.setPosition(originalTextPos.x, originalTextPos.y - offsetY);

    window.draw(shape);
    window.draw(text);

    shape.setPosition(originalShapePos);
    text.setPosition(originalTextPos);
}

bool Button::isClicked(const sf::Vector2i &mousePos)
{
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::setCallback(std::function<void()> callback)
{
    this->callback = callback;
}

void Button::executeCallback()
{
    if (callback)
    {
        callback();
    }
}

sf::FloatRect Button::getBounds() const
{
    return shape.getGlobalBounds();
}

TextDisplay::TextDisplay(const std::string &text, const sf::Font &font, unsigned int characterSize, const sf::Vector2f &position, sf::Color color)
    : text(text, font, characterSize)
{
    this->text.setPosition(position);
    this->text.setFillColor(color);
}

void TextDisplay::draw(sf::RenderWindow &window)
{
    window.draw(text);
}

void TextDisplay::drawWithOffset(sf::RenderWindow &window, float offsetY)
{
    sf::Vector2f originalPos = text.getPosition();
    text.setPosition(originalPos.x, originalPos.y - offsetY);
    window.draw(text);
    text.setPosition(originalPos);
}

void TextDisplay::setText(const std::string &newText)
{
    text.setString(newText);
}

void TextDisplay::setPosition(const sf::Vector2f &position)
{
    text.setPosition(position);
}

Menu::Menu(sf::RenderWindow &window, const sf::Font &font) : window(window), font(font) {}

void Menu::addButton(const std::string &text, const sf::Vector2f &position, const sf::Vector2f &size, std::function<void()> callback, unsigned int fontSize)
{
    sf::Vector2u windowSize = window.getSize();
    if (fontSize == 0)
    {
        fontSize = static_cast<unsigned int>(28 * (windowSize.y / 768.0f));
    }
    buttons.emplace_back(text, font, fontSize, position, size);
    buttons.back().setCallback(callback);
    totalHeight = std::max(totalHeight, position.y + size.y);
}

void Menu::addText(const std::string &text, unsigned int characterSize, const sf::Vector2f &position, sf::Color color)
{
    sf::Vector2u windowSize = window.getSize();
    unsigned int scaledSize = static_cast<unsigned int>(characterSize * (windowSize.y / 768.0f));
    texts.emplace_back(text, font, scaledSize, position, color);
    totalHeight = std::max(totalHeight, position.y + static_cast<float>(scaledSize));
}

void Menu::setScrollable(bool scrollable, float maxHeight)
{
    isScrollable = scrollable;
    maxVisibleHeight = maxHeight;
}

void Menu::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (auto &button : buttons)
        {
            sf::FloatRect bounds = button.getBounds();
            bounds.top -= scrollOffset;
            if (bounds.contains(static_cast<sf::Vector2f>(mousePos)))
            {
                button.executeCallback();
                break;
            }
        }
    }
    else if (isScrollable && event.type == sf::Event::MouseWheelScrolled)
    {
        float delta = event.mouseWheelScroll.delta * 20.0f; // скорость прокрутки
        scrollOffset -= delta;
        scrollOffset = std::max(-100.0f, std::min(scrollOffset, totalHeight - maxVisibleHeight));
    }
}

void Menu::draw()
{
    if (isScrollable)
    {
        for (auto &button : buttons)
        {
            sf::FloatRect bounds = button.getBounds();
            if (bounds.top + bounds.height - scrollOffset > 120.0f) // исчезают, когда низ поднимается выше 120
            {
                button.drawWithOffset(window, scrollOffset);
            }
        }
        for (auto &text : texts)
        {
            text.drawWithOffset(window, scrollOffset);
        }
    }
    else
    {
        for (auto &button : buttons)
        {
            button.draw(window);
        }
        for (auto &text : texts)
        {
            text.draw(window);
        }
    }
}

void Menu::clear()
{
    buttons.clear();
    texts.clear();
}