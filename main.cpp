#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "GUI.h"
#include "CampaignSystem.h"
#include "HeroTemplates.h"
#include "utils.h"

using namespace std;
using namespace sf;

enum class GameState
{
    MAIN_MENU,
    CREDITS,
    CHARACTER_SELECTION,
    CHARACTER_CONFIRMATION,
    CAMPAIGN,
    MAP_MODE
};

int main()
{
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));

    // Создаем окно
    sf::RenderWindow window(sf::VideoMode(800, 600), "The Hunter's Path");
    window.setFramerateLimit(60);

    // Загружаем шрифт
    sf::Font font;
    if (!font.loadFromFile("resources/arial.ttf"))
    {
        // Попробуем загрузить системный шрифт с поддержкой кириллицы
        if (!font.loadFromFile("C:/Windows/Fonts/tahoma.ttf"))
        {
            cerr << "Ошибка загрузки шрифта resources/arial.ttf и системного tahoma.ttf" << endl;
            return -1;
        }
    }

    GameState currentState = GameState::MAIN_MENU;
    int selectedPresetIndex = -1;

    // Главное меню
    Menu mainMenu(window, font);
    mainMenu.addButton("Start Game", sf::Vector2f(300, 200), sf::Vector2f(200, 50), [&]()
                      { currentState = GameState::CHARACTER_SELECTION; });
    mainMenu.addButton("Credits", sf::Vector2f(300, 270), sf::Vector2f(200, 50), [&]()
                       { currentState = GameState::CREDITS; });
    mainMenu.addButton("Exit", sf::Vector2f(300, 340), sf::Vector2f(200, 50), [&]()
                       { window.close(); });

    // Титры
    TextDisplay creditsText("Mikulski Stanislau - BSUIR student", font, 24, sf::Vector2f(200, 250));
    Menu creditsMenu(window, font);
    creditsMenu.addButton("Back", sf::Vector2f(350, 350), sf::Vector2f(100, 50), [&]()
                          { currentState = GameState::MAIN_MENU; });

    // Кампания
    CampaignSystem campaign;

    // Выбор персонажа
    Menu characterSelectionMenu(window, font);
    const auto& presets = HeroFactory::getPartyPresets();
    float yPos = 150;
    for (size_t i = 0; i < presets.size(); ++i) {
        characterSelectionMenu.addButton(presets[i].name, sf::Vector2f(100, yPos), sf::Vector2f(600, 50), [&, i]()
                                        {
                                            selectedPresetIndex = i;
                                            currentState = GameState::CHARACTER_CONFIRMATION;
                                        });
        yPos += 60;
    }
    characterSelectionMenu.addButton("Back", sf::Vector2f(350, yPos), sf::Vector2f(100, 50), [&]()
                                    { currentState = GameState::MAIN_MENU; });
    characterSelectionMenu.setScrollable(true, 400.0f);

    // Подтверждение выбора персонажа
    Menu characterConfirmationMenu(window, font);
    std::vector<TextDisplay> confirmationTexts;

    // Declare text objects outside the loop to avoid initialization issues
    sf::Text title("THE HUNTER'S PATH", font, 36);
    title.setPosition(200, 100);
    title.setFillColor(sf::Color::Yellow);

    sf::String subtitleText = "An Epic RPG with Turn-Based Battles";
    sf::Text subtitle(subtitleText, font, 18);
    subtitle.setPosition(250, 150);
    subtitle.setFillColor(sf::Color::Yellow);

    sf::Text creditsTitle("CREDITS", font, 36);
    creditsTitle.setPosition(300, 150);
    creditsTitle.setFillColor(sf::Color::Yellow);

    sf::Text characterSelectionTitle("CHOOSE YOUR PARTY", font, 36);
    characterSelectionTitle.setPosition(200, 30);
    characterSelectionTitle.setFillColor(sf::Color::Yellow);

    sf::Text characterConfirmationTitle("CONFIRM YOUR PARTY", font, 36);
    characterConfirmationTitle.setPosition(200, 30);
    characterConfirmationTitle.setFillColor(sf::Color::Yellow);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            switch (currentState)
            {
            case GameState::MAIN_MENU:
                mainMenu.handleEvent(event);
                break;
            case GameState::CREDITS:
                creditsMenu.handleEvent(event);
                break;
            case GameState::CHARACTER_SELECTION:
                characterSelectionMenu.handleEvent(event);
                break;
            case GameState::CHARACTER_CONFIRMATION:
                characterConfirmationMenu.handleEvent(event);
                break;
            case GameState::CAMPAIGN:
                // Обработка событий кампании, если нужно
                break;
            case GameState::MAP_MODE:
                // Обработка клавиш для движения
                if (event.type == sf::Event::KeyPressed) {
                    char move = '\0';
                    if (event.key.code == sf::Keyboard::W) move = 'w';
                    else if (event.key.code == sf::Keyboard::A) move = 'a';
                    else if (event.key.code == sf::Keyboard::S) move = 's';
                    else if (event.key.code == sf::Keyboard::D) move = 'd';
                    if (move != '\0') {
                        if (campaign.getGameMapMutable().movePlayer(move)) {
                            // Проверить, наступили ли на событие
                            NodeType currentNode = campaign.getGameMap().getCurrentNode();
                            if (currentNode != NodeType::EMPTY && currentNode != NodeType::START) {
                                Position currentPos = campaign.getGameMap().getPlayerPosition();
                                if (campaign.getVisitedNodes().find(currentPos) == campaign.getVisitedNodes().end()) {
                                    campaign.markNodeVisited(currentPos);
                                    campaign.handleNodeEventPublic(currentNode);
                                }
                            }
                        }
                    }
                }
                break;
            }
        }

        window.clear(sf::Color::Black);

        // Обновление меню подтверждения при необходимости
        if (currentState == GameState::CHARACTER_CONFIRMATION && selectedPresetIndex != -1) {
            characterConfirmationMenu.clear();
            confirmationTexts.clear();

            const auto& presets = HeroFactory::getPartyPresets();
            const PartyPreset& preset = presets[selectedPresetIndex];

            // Добавить описание пресета
            confirmationTexts.emplace_back(preset.description, font, 20, sf::Vector2f(50, 80), sf::Color::White);

            float yPos = 120;
            for (size_t i = 0; i < preset.heroes.size(); ++i) {
                const auto& heroPair = preset.heroes[i];
                const HeroTemplate& tmpl = HeroFactory::getHeroTemplate(heroPair.first);

                // Имя героя
                confirmationTexts.emplace_back(heroPair.second + " (" + tmpl.name + ")", font, 20, sf::Vector2f(50, yPos), sf::Color::Yellow);
                yPos += 27;

                // Описание класса
                confirmationTexts.emplace_back(tmpl.description, font, 18, sf::Vector2f(70, yPos), sf::Color::White);
                yPos += 22;

                // Характеристики
                std::string statsStr = "Stats: HP: " + std::to_string(tmpl.baseMaxHP) + ", Damage: " + std::to_string(tmpl.baseDamage) + ", Defense: " + std::to_string(tmpl.baseDefense) + ", Attack: " + std::to_string(tmpl.baseAttack) + ", Stamina: " + std::to_string(tmpl.baseMaxStamina) + ", Initiative: " + std::to_string(tmpl.baseInitiative) + ", Range: " + std::to_string(tmpl.baseAttackRange);
                confirmationTexts.emplace_back(statsStr, font, 16, sf::Vector2f(70, yPos), sf::Color::Magenta);
                yPos += 20;

                // Способности
                std::string abilitiesStr = "Abilities: ";
                for (size_t j = 0; j < tmpl.availableAbilities.size(); ++j) {
                    const AbilityInfo& ability = HeroFactory::getAbilityInfo(tmpl.availableAbilities[j]);
                    abilitiesStr += ability.name;
                    if (j < tmpl.availableAbilities.size() - 1) abilitiesStr += ", ";
                }
                confirmationTexts.emplace_back(abilitiesStr, font, 16, sf::Vector2f(70, yPos), sf::Color::Cyan);
                yPos += 22;

                // Эффекты способностей
                for (AbilityType abilityType : tmpl.availableAbilities) {
                    const AbilityInfo& ability = HeroFactory::getAbilityInfo(abilityType);
                    confirmationTexts.emplace_back("  " + ability.name + ": " + ability.effect, font, 14, sf::Vector2f(70, yPos), sf::Color::Green);
                    yPos += 17;
                }

                yPos += 10; // Отступ между героями
            }

            // Кнопки
            characterConfirmationMenu.addButton("Confirm", sf::Vector2f(200, yPos), sf::Vector2f(150, 50), [&]()
                                                {
                                                    campaign.createPlayerPartyFromPreset(selectedPresetIndex);
                                                    currentState = GameState::CAMPAIGN;
                                                });
            characterConfirmationMenu.addButton("Back", sf::Vector2f(400, yPos), sf::Vector2f(150, 50), [&]()
                                                {
                                                    currentState = GameState::CHARACTER_SELECTION;
                                                });
            characterConfirmationMenu.setScrollable(true, 500.0f);
        }

        switch (currentState)
        {
        case GameState::MAIN_MENU:
            window.draw(title);
            window.draw(subtitle);
            mainMenu.draw();
            break;
        case GameState::CREDITS:
            window.draw(creditsTitle);
            creditsText.draw(window);
            creditsMenu.draw();
            break;
        case GameState::CHARACTER_SELECTION:
            window.draw(characterSelectionTitle);
            characterSelectionMenu.draw();
            break;
        case GameState::CHARACTER_CONFIRMATION:
            window.draw(characterConfirmationTitle);
            for (auto& text : confirmationTexts) {
                text.draw(window);
            }
            characterConfirmationMenu.draw();
            break;
        case GameState::CAMPAIGN:
            // Инициализируем кампанию и переходим на карту
            campaign.createPlayerPartyFromPreset(selectedPresetIndex);
            campaign.initializeCampaign();
            currentState = GameState::MAP_MODE;
            break;
        case GameState::MAP_MODE:
            // Отображаем карту
            {
                std::string mapStr = campaign.getGameMap().getMapString();
                sf::Text mapText(mapStr, font, 14);
                mapText.setPosition(10, 10);
                mapText.setFillColor(sf::Color::White);
                window.draw(mapText);

                // Отображаем информацию о локации
                std::string locationInfo = "Location: " + campaign.getCurrentLocation().name + "\nDifficulty: " + std::to_string(campaign.getCurrentDifficulty());
                sf::Text infoText(locationInfo, font, 16);
                infoText.setPosition(10, 500);
                infoText.setFillColor(sf::Color::Yellow);
                window.draw(infoText);
            }
            break;
        }

        window.display();
    }

    return 0;
}
