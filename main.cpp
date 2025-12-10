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
    MAP_MODE,
    TREASURE,
    EVENT,
    EXIT,
    BATTLE,
    INVENTORY,
    VICTORY,
    DEFEAT
};

enum class BattleState
{
    MAIN_MENU,
    SELECT_TARGET_ATTACK,
    SELECT_TARGET_ABILITY,
    SELECT_POSITION_MOVE,
    SELECT_ABILITY,
    CONFIRM_ABILITY
};

int main()
{
    // Initialize random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create full-screen window
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "The Hunter's Path", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf"))
    {
        // Try to load system font with Cyrillic support
        if (!font.loadFromFile("C:/Windows/Fonts/tahoma.ttf"))
        {
            cerr << "Font loading error: system calibri.ttf and tahoma.ttf" << endl;
            return -1;
        }
    }

    GameState currentState = GameState::MAIN_MENU;
    size_t selectedPresetIndex = static_cast<size_t>(-1);

    // Battle state variables
    BattleState battleState = BattleState::MAIN_MENU;
    AbilityType selectedAbility = AbilityType::NONE;
    int selectedTargetIndex = -1;
    int selectedPosition = -1;
    std::string pendingExpMessage;
    int selectedHeroIndex = -1;

    // Main menu
    Menu mainMenu(window, font);
    sf::Vector2u windowSize = window.getSize();
    float buttonWidth = windowSize.x * 0.3f;
    float buttonHeight = windowSize.y * 0.08f;
    float buttonX = windowSize.x * 0.3f;
    float buttonYStart = windowSize.y * 0.3f;
    float buttonSpacing = windowSize.y * 0.1f;
    mainMenu.addButton("Start Game", sf::Vector2f(buttonX, buttonYStart), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                       { currentState = GameState::CHARACTER_SELECTION; });
    mainMenu.addButton("Credits", sf::Vector2f(buttonX, buttonYStart + buttonSpacing), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                       { currentState = GameState::CREDITS; });
    mainMenu.addButton("Exit", sf::Vector2f(buttonX, buttonYStart + 2 * buttonSpacing), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                       { window.close(); });

    // Credits
    TextDisplay creditsText("Mikulski Stanislau - BSUIR student", font, 24, sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.35f));
    Menu creditsMenu(window, font);
    creditsMenu.addButton("Back", sf::Vector2f(windowSize.x * 0.45f, windowSize.y * 0.5f), sf::Vector2f(windowSize.x * 0.12f, windowSize.y * 0.07f), [&]()
                          { currentState = GameState::MAIN_MENU; });

    // Campaign
    CampaignSystem campaign;

    // Character selection
    Menu characterSelectionMenu(window, font);
    const auto &presets = HeroFactory::getPartyPresets();
    float buttonWidthCS = windowSize.x * 0.2f;
    float buttonHeightCS = windowSize.y * 0.06f;
    unsigned int fontSizeCS = static_cast<unsigned int>(20 * (windowSize.y / 768.0f));
    float yPos = windowSize.y * 0.15f;
    for (size_t i = 0; i < presets.size(); ++i)
    {
        characterSelectionMenu.addButton(presets[i].name, sf::Vector2f((windowSize.x - buttonWidthCS) / 2, yPos), sf::Vector2f(buttonWidthCS, buttonHeightCS), [&, i]()
                                         {
                                             selectedPresetIndex = i;
                                             currentState = GameState::CHARACTER_CONFIRMATION; }, fontSizeCS);
        yPos += buttonSpacing;
    }
    characterSelectionMenu.addButton("Back", sf::Vector2f((windowSize.x - windowSize.x * 0.08f) / 2, yPos), sf::Vector2f(windowSize.x * 0.08f, buttonHeightCS), [&]()
                                     { currentState = GameState::MAIN_MENU; }, fontSizeCS);
    characterSelectionMenu.setScrollable(true, windowSize.y * 0.5f);

    // Character confirmation
    Menu characterConfirmationMenu(window, font);

    // Treasure window
    Menu treasureMenu(window, font);
    std::vector<TextDisplay> treasureTexts;

    // Event window
    Menu eventMenu(window, font);
    std::vector<TextDisplay> eventTexts;

    // Exit window
    Menu exitMenu(window, font);
    std::vector<TextDisplay> exitTexts;

    // Battle window
    Menu battleMenu(window, font);
    std::vector<TextDisplay> battleTexts;

    // Inventory window
    Menu inventoryMenu(window, font);
    std::vector<TextDisplay> inventoryTexts;

    // Declare text objects outside the loop to avoid initialization issues
    sf::Text title("THE HUNTER'S PATH", font, static_cast<unsigned int>(36 * (windowSize.y / 768.0f)));
    title.setPosition(windowSize.x * 0.2f, windowSize.y * 0.1f);
    title.setFillColor(sf::Color::Yellow);

    sf::String subtitleText = "An Epic RPG with Turn-Based Battles";
    sf::Text subtitle(subtitleText, font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)));
    subtitle.setPosition(windowSize.x * 0.25f, windowSize.y * 0.15f);
    subtitle.setFillColor(sf::Color::Yellow);

    sf::Text creditsTitle("CREDITS", font, static_cast<unsigned int>(36 * (windowSize.y / 768.0f)));
    creditsTitle.setPosition(windowSize.x * 0.3f, windowSize.y * 0.15f);
    creditsTitle.setFillColor(sf::Color::Yellow);

    sf::Text characterSelectionTitle("CHOOSE YOUR PARTY", font, static_cast<unsigned int>(36 * (windowSize.y / 768.0f)));
    characterSelectionTitle.setPosition(windowSize.x * 0.2f, windowSize.y * 0.03f);
    characterSelectionTitle.setFillColor(sf::Color::Yellow);

    sf::Text characterConfirmationTitle("CONFIRM YOUR PARTY", font, static_cast<unsigned int>(36 * (windowSize.y / 768.0f)));
    characterConfirmationTitle.setPosition(windowSize.x * 0.2f, windowSize.y * 0.03f);
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
                // Handle campaign events if needed
                break;
            case GameState::MAP_MODE:
                // Handle movement keys
                if (event.type == sf::Event::KeyPressed)
                {
                    char move = '\0';
                    if (event.key.code == sf::Keyboard::W)
                        move = 'w';
                    else if (event.key.code == sf::Keyboard::A)
                        move = 'a';
                    else if (event.key.code == sf::Keyboard::S)
                        move = 's';
                    else if (event.key.code == sf::Keyboard::D)
                        move = 'd';
                    if (move != '\0')
                    {
                        if (campaign.getGameMapMutable().movePlayer(move))
                        {
                            // Check if stepped on an event
                            NodeType currentNode = campaign.getGameMap().getCurrentNode();
                            if (currentNode != NodeType::EMPTY && currentNode != NodeType::START)
                            {
                                Position currentPos = campaign.getGameMap().getPlayerPosition();
                                if (campaign.getVisitedNodes().find(currentPos) == campaign.getVisitedNodes().end())
                                {
                                    campaign.markNodeVisited(currentPos);
                                    campaign.handleNodeEventPublic(currentNode);
                                    // Check if need to transition to GUI event
                                    if (campaign.hasPendingBattle())
                                    {
                                        currentState = GameState::BATTLE;
                                    }
                                    else if (campaign.hasPendingTreasure())
                                    {
                                        currentState = GameState::TREASURE;
                                    }
                                    else if (campaign.hasPendingEvent())
                                    {
                                        currentState = GameState::EVENT;
                                    }
                                    else if (campaign.hasPendingExit())
                                    {
                                        currentState = GameState::EXIT;
                                    }
                                }
                            }
                        }
                    }
                }
                // Handle mouse click for inventory button
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect invButtonRect(windowSize.x * 0.7f, windowSize.y * 0.01f, windowSize.x * 0.1f, windowSize.y * 0.04f);
                    if (invButtonRect.contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        currentState = GameState::INVENTORY;
                        selectedHeroIndex = -1;
                    }
                }
                break;
            case GameState::TREASURE:
                treasureMenu.handleEvent(event);
                break;
            case GameState::EVENT:
                eventMenu.handleEvent(event);
                break;
            case GameState::EXIT:
                exitMenu.handleEvent(event);
                break;
            case GameState::BATTLE:
                battleMenu.handleEvent(event);
                break;
            case GameState::INVENTORY:
                inventoryMenu.handleEvent(event);
                break;
            case GameState::VICTORY:
                // Handle main menu button click
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect mainMenuRect(windowSize.x * 0.35f, windowSize.y * 0.4f, windowSize.x * 0.1f, windowSize.y * 0.05f);
                    if (mainMenuRect.contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        currentState = GameState::MAIN_MENU;
                    }
                }
                break;
            case GameState::DEFEAT:
                // Handle main menu button click
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect mainMenuRect(windowSize.x * 0.35f, windowSize.y * 0.4f, windowSize.x * 0.1f, windowSize.y * 0.05f);
                    if (mainMenuRect.contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        currentState = GameState::MAIN_MENU;
                    }
                }
                break;
            }
        }

        window.clear(sf::Color::Black);

        // Update confirmation menu if necessary
        if (currentState == GameState::CHARACTER_CONFIRMATION && selectedPresetIndex != static_cast<size_t>(-1))
        {
            characterConfirmationMenu.clear();

            const auto &presets = HeroFactory::getPartyPresets();
            const PartyPreset &preset = presets[selectedPresetIndex];

            // Добавить описание пресета
            characterConfirmationMenu.addText(sf::String(preset.description), static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.08f), sf::Color::White);
            size_t presetLineCount = 1;
            for (char c : preset.description)
            {
                if (c == '\n')
                    presetLineCount++;
            }
            float presetYPos = windowSize.y * 0.08f + presetLineCount * windowSize.y * 0.025f;

            float yPos = presetYPos + windowSize.y * 0.06f;
            float xOffset = windowSize.x * 0.07f;
            float statXOffset = windowSize.x * 0.09f;
            float abilityXOffset = windowSize.x * 0.11f;
            for (size_t i = 0; i < preset.heroes.size(); ++i)
            {
                const auto &heroPair = preset.heroes[i];
                const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(heroPair.first);

                // Имя героя
                characterConfirmationMenu.addText(sf::String(heroPair.second + " (" + tmpl.name + ")"), static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Yellow);
                yPos += windowSize.y * 0.035f;

                // Описание класса
                characterConfirmationMenu.addText(sf::String(tmpl.description), static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(xOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.03f;

                // Characteristics
                characterConfirmationMenu.addText(sf::String("Characteristics:"), static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(xOffset, yPos), sf::Color::Magenta);
                yPos += windowSize.y * 0.03f;
                characterConfirmationMenu.addText(sf::String("Health (HP): " + std::to_string(tmpl.baseMaxHP) + " - maximum hero health"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                characterConfirmationMenu.addText(sf::String("Damage: " + std::to_string(tmpl.baseDamage) + " - base damage in attack"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                characterConfirmationMenu.addText(sf::String("Defense: " + std::to_string(tmpl.baseDefense) + " - reduces incoming damage"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                characterConfirmationMenu.addText(sf::String("Attack: " + std::to_string(tmpl.baseAttack) + " - attack accuracy"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                characterConfirmationMenu.addText(sf::String("Stamina: " + std::to_string(tmpl.baseMaxStamina) + " - points for actions"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                characterConfirmationMenu.addText(sf::String("Initiative: " + std::to_string(tmpl.baseInitiative) + " - determines turn order"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                characterConfirmationMenu.addText(sf::String("Range: " + std::to_string(tmpl.baseAttackRange) + " - combat type (0 - melee)"), static_cast<unsigned int>(14 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::White);
                yPos += windowSize.y * 0.03f;

                // Abilities
                characterConfirmationMenu.addText(sf::String("Abilities:"), static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(xOffset, yPos), sf::Color::Cyan);
                yPos += windowSize.y * 0.03f;
                for (AbilityType abilityType : tmpl.availableAbilities)
                {
                    const AbilityInfo &ability = HeroFactory::getAbilityInfo(abilityType);
                    characterConfirmationMenu.addText(sf::String(ability.name + " (" + std::to_string(ability.staminaCost) + "):"), static_cast<unsigned int>(16 * (windowSize.y / 768.0f)), sf::Vector2f(statXOffset, yPos), sf::Color::Yellow);
                    yPos += windowSize.y * 0.025f;
                    characterConfirmationMenu.addText(sf::String(ability.effect), static_cast<unsigned int>(12 * (windowSize.y / 768.0f)), sf::Vector2f(abilityXOffset, yPos), sf::Color::Green);
                    size_t lineCount = 1;
                    for (char c : ability.effect)
                    {
                        if (c == '\n')
                            lineCount++;
                    }
                    yPos += windowSize.y * 0.025f * lineCount;
                }

                yPos += windowSize.y * 0.02f; // Отступ между героями
            }

            // Кнопки
            characterConfirmationMenu.addButton("Confirm", sf::Vector2f(windowSize.x * 0.2f, yPos), sf::Vector2f(windowSize.x * 0.15f, windowSize.y * 0.05f), [&]()
                                                {
                                                    campaign.createPlayerPartyFromPreset(selectedPresetIndex);
                                                    currentState = GameState::CAMPAIGN; });
            characterConfirmationMenu.addButton("Back", sf::Vector2f(windowSize.x * 0.4f, yPos), sf::Vector2f(windowSize.x * 0.15f, windowSize.y * 0.05f), [&]()
                                                { currentState = GameState::CHARACTER_SELECTION; });
            characterConfirmationMenu.setScrollable(true, windowSize.y * 0.9f);
        }

        // Update treasure menu
        if (currentState == GameState::TREASURE && campaign.hasPendingTreasure())
        {
            treasureMenu.clear();
            treasureTexts.clear();

            Item *treasure = campaign.getPendingTreasure();
            if (treasure)
            {
                treasureTexts.emplace_back("You found a treasure!", font, static_cast<unsigned int>(30 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.1f), sf::Color::Yellow);
                treasureTexts.emplace_back(treasure->getName(), font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.14f), sf::Color::White);
                treasureTexts.emplace_back(treasure->getDescription(), font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.17f), sf::Color::White);

                // Take button
                treasureMenu.addButton("Take", sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.25f), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.06f), [&]()
                                       {
                                           campaign.takePendingTreasure();
                                           currentState = GameState::MAP_MODE; });

                // Decline button
                treasureMenu.addButton("Decline", sf::Vector2f(windowSize.x * 0.4f, windowSize.y * 0.25f), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.06f), [&]()
                                       {
                                           campaign.clearPendingTreasure();
                                           currentState = GameState::MAP_MODE; });
            }
        }

        // Update event menu
        if (currentState == GameState::EVENT && campaign.hasPendingEvent())
        {
            eventMenu.clear();
            eventTexts.clear();

            const CampaignEvent &evt = campaign.getPendingEvent();
            eventTexts.emplace_back(evt.description, font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.1f), sf::Color::White);

            float yPos = windowSize.y * 0.15f;
            for (size_t i = 0; i < evt.choices.size(); ++i)
            {
                eventMenu.addButton(evt.choices[i], sf::Vector2f(windowSize.x * 0.1f, yPos), sf::Vector2f(windowSize.x * 0.4f, windowSize.y * 0.05f), [&, i]()
                                    {
                                         campaign.handleEventChoice(i);
                                         campaign.clearPendingEvent();
                                         currentState = GameState::MAP_MODE; });
                yPos += windowSize.y * 0.06f;
            }
        }

        // Update exit menu
        if (currentState == GameState::EXIT && campaign.hasPendingExit())
        {
            exitMenu.clear();
            exitTexts.clear();

            const CampaignEvent &evt = campaign.getPendingExit();
            exitTexts.emplace_back(evt.description, font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.1f), sf::Color::White);
            exitTexts.emplace_back("Choose a location to transition to:", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.14f), sf::Color::White);

            float yPos = windowSize.y * 0.18f;
            const auto &connections = campaign.getCurrentLocation().connections;
            for (size_t i = 0; i < connections.size(); ++i)
            {
                LocationType locType = connections[i];
                const Location &loc = campaign.getLocation(locType);
                std::string buttonText = std::to_string(i + 1) + ". " + loc.name + " - " + loc.description;
                exitMenu.addButton(buttonText, sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Vector2f(windowSize.x * 0.6f, windowSize.y * 0.05f), [&, i]()
                                   {
                                       campaign.handleExitChoice(i);
                                       campaign.clearPendingExit();
                                       currentState = GameState::MAP_MODE; });
                yPos += windowSize.y * 0.06f;
            }
        }

        // Update battle menu
        if (currentState == GameState::BATTLE && campaign.hasPendingBattle())
        {
            battleMenu.clear();
            battleTexts.clear();

            BattleSystem *battle = campaign.getCurrentBattle();
            if (battle)
            {
                // Check battle end first
                if (!battle->isBattleActive())
                {
                    battleTexts.emplace_back("BATTLE", font, static_cast<unsigned int>(36 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.02f), sf::Color::Yellow);
                    battleTexts.emplace_back(battle->getTurnOrderString(), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.45f, windowSize.y * 0.06f), sf::Color::White);
                    if (battle->isPlayerVictory())
                    {
                        battleTexts.emplace_back("VICTORY!", font, static_cast<unsigned int>(56 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.25f), sf::Color::Green);
                        battleMenu.addButton("Continue", sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.4f), sf::Vector2f(windowSize.x * 0.12f, windowSize.y * 0.06f), [&]()
                                             {
                             // Award experience
                             int expPerPlayer = campaign.getPendingExperience() / campaign.getPlayerParty().size();
                             for (Player *player : campaign.getPlayerParty()) {
                                 if (player->getCurrentHealthPoint() > 0) {
                                     player->setReceivedExperience(player->getReceivedExperience() + expPerPlayer);
                                     player->upLevel();
                                     cout << player->getName() << " receives " << expPerPlayer << " experience!\n";
                                 }
                             }
                             // Set experience gained message
                             pendingExpMessage = "Experience gained: " + std::to_string(expPerPlayer) + " per player";
                             // Check if this was the final boss battle
                             if (campaign.getCurrentLocation().isFinalBossLocation) {
                                 campaign.setGameCompleted(true);
                                 currentState = GameState::VICTORY;
                             } else {
                                 currentState = GameState::MAP_MODE;
                             }
                             campaign.clearPendingBattle();
                             campaign.clearPendingExperience();
                             battleState = BattleState::MAIN_MENU; });
                    }
                    else
                    {
                        battleTexts.emplace_back("DEFEAT!", font, static_cast<unsigned int>(56 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.25f), sf::Color::Red);
                        battleMenu.addButton("Main Menu", sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.4f), sf::Vector2f(windowSize.x * 0.12f, windowSize.y * 0.06f), [&]()
                                             {
                              campaign.clearPendingBattle();
                              currentState = GameState::DEFEAT;
                              battleState = BattleState::MAIN_MENU; });
                    }
                }
                else
                {
                    // Display battlefield
                    battleTexts.emplace_back("BATTLE", font, static_cast<unsigned int>(30 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.02f), sf::Color::Yellow);
                    battleTexts.emplace_back(battle->getBattleStatus(), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.06f), sf::Color::White);
                    float yPos = windowSize.y * 0.6f;

                    // Current turn entity
                    Entity *currentEntity = battle->getCurrentTurnEntity();
                    if (currentEntity)
                    {
                        string turnText = "Current Turn: " + currentEntity->getName() + " (Stamina: " + to_string(currentEntity->getCurrentStamina()) + "/" + to_string(currentEntity->getMaxStamina()) + ")";
                        battleTexts.emplace_back(turnText, font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos + windowSize.y * 0.02f), sf::Color::Cyan);
                        yPos += windowSize.y * 0.05f;

                        // Check if player entity
                        bool isPlayer = false;
                        for (Player *p : campaign.getPlayerParty())
                        {
                            if (p == currentEntity)
                            {
                                isPlayer = true;
                                break;
                            }
                        }

                        if (isPlayer)
                        {
                            // Player turn - show action buttons
                            if (battleState == BattleState::MAIN_MENU)
                            {
                                float buttonWidth = windowSize.x * 0.2f;
                                float buttonHeight = windowSize.y * 0.05f;
                                float buttonX = windowSize.x * 0.4f;
                                float spacing = windowSize.y * 0.01f;
                                battleMenu.addButton("Attack", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     { battleState = BattleState::SELECT_TARGET_ATTACK; });
                                yPos += buttonHeight + spacing;
                                battleMenu.addButton("Use Ability", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     { battleState = BattleState::SELECT_ABILITY; });
                                yPos += buttonHeight + spacing;
                                battleMenu.addButton("Move", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     { battleState = BattleState::SELECT_POSITION_MOVE; });
                                yPos += buttonHeight + spacing;
                                battleMenu.addButton("Skip Turn", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     {
                                 // Skip turn
                                 currentEntity->setCurrentStamina(0);
                                 battleState = BattleState::MAIN_MENU; });
                                yPos += buttonHeight + spacing;
                                battleMenu.addButton("End Turn", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     {
                                 // End turn
                                 battle->nextTurn();
                                 battleState = BattleState::MAIN_MENU; });
                            }
                            else if (battleState == BattleState::SELECT_TARGET_ATTACK)
                            {
                                // Show available targets for attack
                                vector<pair<Entity *, int>> targets = battle->getAvailableTargetsForCurrent();
                                if (!targets.empty())
                                {
                                    battleTexts.emplace_back("Select Target to Attack:", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                                    yPos += windowSize.y * 0.035f;
                                    for (size_t i = 0; i < targets.size(); ++i)
                                    {
                                        string targetText = to_string(i + 1) + ". " + targets[i].first->getName() + " (HP: " + to_string(targets[i].first->getCurrentHealthPoint()) + ")";
                                        battleMenu.addButton(targetText, sf::Vector2f((windowSize.x - windowSize.x * 0.35f) / 2, yPos), sf::Vector2f(windowSize.x * 0.35f, windowSize.y * 0.04f), [&, i, targets]()
                                                             {
                                            battle->attack(currentEntity, targets[i].first);
                                            battleState = BattleState::MAIN_MENU; });
                                        yPos += windowSize.y * 0.045f;
                                    }
                                }
                                else
                                {
                                    battleTexts.emplace_back("No targets available for attack!", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Red);
                                    yPos += windowSize.y * 0.035f;
                                }
                                battleMenu.addButton("Back", sf::Vector2f((windowSize.x - windowSize.x * 0.1f) / 2, yPos), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.04f), [&]()
                                                     { battleState = BattleState::MAIN_MENU; });
                                battleMenu.setScrollable(true, windowSize.y * 0.5f);
                                battleMenu.setScrollable(true, windowSize.y * 0.5f);
                            }
                            else if (battleState == BattleState::SELECT_ABILITY)
                            {
                                // Show available abilities
                                Player *player = static_cast<Player *>(currentEntity);
                                const vector<AbilityType> &abilities = player->getAvailableAbilities();
                                if (!abilities.empty())
                                {
                                    battleTexts.emplace_back("Select Ability:", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                                    yPos += windowSize.y * 0.035f;
                                    for (size_t i = 0; i < abilities.size(); ++i)
                                    {
                                        const AbilityInfo &info = HeroFactory::getAbilityInfo(abilities[i]);
                                        string abilityText = to_string(i + 1) + ". " + info.name + " (" + to_string(info.staminaCost) + ")";
                                        battleMenu.addButton(abilityText, sf::Vector2f((windowSize.x - windowSize.x * 0.3f) / 2, yPos), sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.04f), [&, i, abilities]()
                                                             {
                                            selectedAbility = abilities[i];
                                            battleState = BattleState::CONFIRM_ABILITY; });
                                        yPos += windowSize.y * 0.05f;
                                    }
                                }
                                else
                                {
                                    battleTexts.emplace_back("No abilities available!", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Red);
                                    yPos += windowSize.y * 0.035f;
                                }
                                battleMenu.addButton("Back", sf::Vector2f((windowSize.x - windowSize.x * 0.1f) / 2, yPos), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.04f), [&]()
                                                     { battleState = BattleState::MAIN_MENU; });
                            }
                            else if (battleState == BattleState::CONFIRM_ABILITY)
                            {
                                // Show ability description and confirm
                                const AbilityInfo &info = HeroFactory::getAbilityInfo(selectedAbility);
                                battleTexts.emplace_back("Confirm Ability: " + info.name + " (" + to_string(info.staminaCost) + ")", font, static_cast<unsigned int>(22 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Yellow);
                                yPos += windowSize.y * 0.035f;
                                battleTexts.emplace_back("Description: " + info.description, font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                                yPos += windowSize.y * 0.025f;
                                battleTexts.emplace_back(sf::String("Effect: " + info.effect), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                                yPos += windowSize.y * 0.035f;
                                float buttonWidth = windowSize.x * 0.2f;
                                float buttonHeight = windowSize.y * 0.05f;
                                float buttonX = windowSize.x * 0.4f;
                                float spacing = windowSize.y * 0.01f;
                                battleMenu.addButton("Confirm", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     {
                                                         const AbilityInfo &info = HeroFactory::getAbilityInfo(selectedAbility);
                                                         if (info.isAreaEffect) {
                                                             battle->useAbility(currentEntity, selectedAbility);
                                                             battleState = BattleState::MAIN_MENU;
                                                         } else {
                                                             battleState = BattleState::SELECT_TARGET_ABILITY;
                                                         } });
                                yPos += buttonHeight + spacing;
                                battleMenu.addButton("Back", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     { battleState = BattleState::SELECT_ABILITY; });
                            }
                            else if (battleState == BattleState::SELECT_TARGET_ABILITY)
                            {
                                // Show targets for ability (simplified - assume area effect or single target)
                                vector<pair<Entity *, int>> targets = battle->getAvailableTargetsForCurrent();
                                if (!targets.empty())
                                {
                                    battleTexts.emplace_back("Select Target for Ability:", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                                    yPos += windowSize.y * 0.035f;
                                    for (size_t i = 0; i < targets.size(); ++i)
                                    {
                                        string targetText = to_string(i + 1) + ". " + targets[i].first->getName();
                                        battleMenu.addButton(targetText, sf::Vector2f((windowSize.x - windowSize.x * 0.25f) / 2, yPos), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.04f), [&, i, targets]()
                                                             {
                                            battle->useAbility(currentEntity, selectedAbility);
                                            battleState = BattleState::MAIN_MENU; });
                                        yPos += windowSize.y * 0.045f;
                                    }
                                }
                                else
                                {
                                    // Use ability without target
                                    battleMenu.addButton("Use Ability", sf::Vector2f((windowSize.x - windowSize.x * 0.15f) / 2, yPos), sf::Vector2f(windowSize.x * 0.15f, windowSize.y * 0.04f), [&]()
                                                         {
                                        battle->useAbility(currentEntity, selectedAbility);
                                        battleState = BattleState::MAIN_MENU; });
                                    yPos += windowSize.y * 0.045f;
                                }
                                battleMenu.addButton("Back", sf::Vector2f((windowSize.x - windowSize.x * 0.1f) / 2, yPos), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.04f), [&]()
                                                     { battleState = BattleState::CONFIRM_ABILITY; });
                                battleMenu.setScrollable(true, windowSize.y * 0.5f);
                            }
                            else if (battleState == BattleState::SELECT_POSITION_MOVE)
                            {
                                // Show position buttons
                                battleTexts.emplace_back("Select Position to Move:", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                                yPos += windowSize.y * 0.035f;
                                float buttonWidth = windowSize.x * 0.2f;
                                float buttonHeight = windowSize.y * 0.04f;
                                float buttonX = windowSize.x * 0.4f;
                                float spacing = windowSize.y * 0.01f;
                                for (int pos = 0; pos < 4; ++pos)
                                {
                                    string posText = "Position " + to_string(pos + 1);
                                    battleMenu.addButton(posText, sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&, pos]()
                                                         {
                                         battle->movePosition(currentEntity, pos);
                                         battleState = BattleState::MAIN_MENU; });
                                    yPos += buttonHeight + spacing;
                                }
                                battleMenu.addButton("Back", sf::Vector2f(buttonX, yPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     { battleState = BattleState::MAIN_MENU; });
                            }
                        }
                        else
                        {
                            // AI turn - simulate
                            if (currentEntity->getCurrentStamina() <= 0)
                            {
                                // Automatically skip turn if no stamina
                                battle->nextTurn();
                            }
                            else
                            {
                                battleTexts.emplace_back("AI Turn - Press Next to continue", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Yellow);
                                float buttonWidth = windowSize.x * 0.2f;
                                float buttonHeight = windowSize.y * 0.04f;
                                float buttonX = windowSize.x * 0.4f;
                                float spacing = windowSize.y * 0.01f;
                                float aiYPos = yPos + windowSize.y * 0.035f;
                                battleMenu.addButton("Next", sf::Vector2f(buttonX, aiYPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     {
                                     // Simple AI: attack if possible, else use ability if available, else skip turn
                                     vector<pair<Entity *, int>> targets = battle->getAvailableTargetsForCurrent();
                                     if (!targets.empty()) {
                                         int targetIndex = rand() % targets.size();
                                         battle->attack(currentEntity, targets[targetIndex].first);
                                     } else if (currentEntity->getAbility() != AbilityType::NONE) {
                                         // Try to use ability if no attack targets
                                         const AbilityInfo &info = HeroFactory::getAbilityInfo(currentEntity->getAbility());
                                         if (currentEntity->getCurrentStamina() >= info.staminaCost) {
                                             battle->useAbility(currentEntity, currentEntity->getAbility());
                                         } else {
                                             battle->nextTurn();
                                         }
                                     } else {
                                         battle->nextTurn();
                                     } });
                                aiYPos += buttonHeight + spacing;
                                battleMenu.addButton("End AI Turn", sf::Vector2f(buttonX, aiYPos), sf::Vector2f(buttonWidth, buttonHeight), [&]()
                                                     { battle->nextTurn(); });
                            }
                        }
                    }
                }
            }
            else
            {
                // Fallback
                battleTexts.emplace_back("Battle system error.", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.2f, windowSize.y * 0.1f), sf::Color::Red);
                battleMenu.addButton("Continue", sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.2f), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.05f), [&]()
                                     {
                    campaign.clearPendingBattle();
                    currentState = GameState::MAP_MODE; });
            }
        }

        // Update inventory menu
        if (currentState == GameState::INVENTORY)
        {
            inventoryMenu.clear();
            inventoryTexts.clear();

            if (selectedHeroIndex == -1)
            {
                // Show hero selection
                inventoryTexts.emplace_back("Select Hero", font, static_cast<unsigned int>(30 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.25f, windowSize.y * 0.05f), sf::Color::Yellow);
                const auto &party = campaign.getPlayerParty();
                float yPos = windowSize.y * 0.1f;
                for (size_t i = 0; i < party.size(); ++i)
                {
                    inventoryMenu.addButton(party[i]->getName(), sf::Vector2f(windowSize.x * 0.1f, yPos), sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.05f), [&, i]()
                                            { selectedHeroIndex = i; });
                    yPos += windowSize.y * 0.06f;
                }
                inventoryMenu.addButton("Back", sf::Vector2f(windowSize.x * 0.35f, windowSize.y * 0.5f), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.05f), [&]()
                                        { currentState = GameState::MAP_MODE; });
            }
            else
            {
                // Show hero details
                Player *hero = campaign.getPlayerParty()[selectedHeroIndex];
                inventoryTexts.emplace_back("Hero: " + hero->getName(), font, static_cast<unsigned int>(30 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, windowSize.y * 0.05f), sf::Color::Yellow);

                // Stats
                auto bonuses = hero->getEquipmentBonuses();
                float yPos = windowSize.y * 0.1f;
                inventoryTexts.emplace_back("Stats:", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Cyan);
                yPos += windowSize.y * 0.03f;
                inventoryTexts.emplace_back("HP: " + std::to_string(hero->getBaseMaxHP()) + " + " + std::to_string(bonuses["health"]) + " = " + std::to_string(hero->getMaxHealthPoint()), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                inventoryTexts.emplace_back("Damage: " + std::to_string(hero->getBaseDamage()) + " + " + std::to_string(bonuses["damage"]) + " = " + std::to_string(hero->getDamage()), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                inventoryTexts.emplace_back("Defense: " + std::to_string(hero->getBaseDefense()) + " + " + std::to_string(bonuses["defense"]) + " = " + std::to_string(hero->getDefense()), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                inventoryTexts.emplace_back("Attack: " + std::to_string(hero->getBaseAttack()) + " + " + std::to_string(bonuses["attack"]) + " = " + std::to_string(hero->getAttack()), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                inventoryTexts.emplace_back("Stamina: " + std::to_string(hero->getBaseMaxStamina()) + " + " + std::to_string(bonuses["stamina"]) + " = " + std::to_string(hero->getMaxStamina()), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                yPos += windowSize.y * 0.025f;
                inventoryTexts.emplace_back("Initiative: " + std::to_string(hero->getBaseInitiative()) + " + " + std::to_string(bonuses["initiative"]) + " = " + std::to_string(hero->getInitiative()), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                yPos += windowSize.y * 0.04f;

                // Equipment
                inventoryTexts.emplace_back("Equipment:", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Cyan);
                yPos += windowSize.y * 0.03f;
                const auto &equipment = hero->getEquipment();
                for (const auto &slot : equipment)
                {
                    std::string slotName = Player::slotNames.at(slot.first);
                    std::string itemName = slot.second.getName();
                    inventoryTexts.emplace_back(slotName + ": " + itemName, font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                    // Add unequip button if not None
                    if (itemName != "None")
                    {
                        inventoryMenu.addButton("Unequip", sf::Vector2f(windowSize.x * 0.4f, yPos), sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.03f), [&, slot]()
                                                { hero->unequipItem(slot.first); });
                    }
                    yPos += windowSize.y * 0.025f;
                }
                yPos += windowSize.y * 0.04f;

                // Inventory
                inventoryTexts.emplace_back("Inventory:", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Cyan);
                yPos += windowSize.y * 0.03f;
                const auto &inv = hero->getInventory();
                for (size_t i = 0; i < inv.size(); ++i)
                {
                    inventoryTexts.emplace_back(std::to_string(i + 1) + ". " + inv[i].getName(), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                    // Add equip button if equippable
                    if (inv[i].getSlot() != EquipmentSlot::NONE)
                    {
                        inventoryMenu.addButton("Equip", sf::Vector2f(windowSize.x * 0.4f, yPos), sf::Vector2f(windowSize.x * 0.08f, windowSize.y * 0.03f), [&, i]()
                                                { hero->equipItem(i); });
                    }
                    // Add use button if consumable
                    if (inv[i].getType() == ItemType::CONSUMABLE)
                    {
                        inventoryMenu.addButton("Use", sf::Vector2f(windowSize.x * 0.5f, yPos), sf::Vector2f(windowSize.x * 0.08f, windowSize.y * 0.03f), [&, i]()
                                                { hero->useConsumable(i); });
                    }
                    yPos += windowSize.y * 0.025f;
                }
                yPos += windowSize.y * 0.04f;

                // Party inventory
                inventoryTexts.emplace_back("Party Inventory:", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::Cyan);
                yPos += windowSize.y * 0.03f;
                const auto &partyInv = campaign.getPartyInventory();
                for (size_t i = 0; i < partyInv.size(); ++i)
                {
                    inventoryTexts.emplace_back(std::to_string(i + 1) + ". " + partyInv[i].getName(), font, static_cast<unsigned int>(18 * (windowSize.y / 768.0f)), sf::Vector2f(windowSize.x * 0.05f, yPos), sf::Color::White);
                    inventoryMenu.addButton("Take", sf::Vector2f(windowSize.x * 0.4f, yPos), sf::Vector2f(windowSize.x * 0.08f, windowSize.y * 0.03f), [&, i]()
                                            { hero->addItem(partyInv[i]); campaign.getPartyInventoryMutable().erase(campaign.getPartyInventoryMutable().begin() + i); });
                    yPos += windowSize.y * 0.025f;
                }

                // Back button
                inventoryMenu.addButton("Back to Heroes", sf::Vector2f(windowSize.x * 0.35f, windowSize.y * 0.9f), sf::Vector2f(windowSize.x * 0.15f, windowSize.y * 0.05f), [&]()
                                        { selectedHeroIndex = -1; });
                inventoryMenu.setScrollable(true, windowSize.y * 0.9f);
            }
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
            characterConfirmationMenu.draw();
            break;
        case GameState::CAMPAIGN:
            // Initialize campaign and go to map
            campaign.createPlayerPartyFromPreset(selectedPresetIndex);
            campaign.initializeCampaign();
            currentState = GameState::MAP_MODE;
            break;
        case GameState::MAP_MODE:
        {
            // Display map
            std::string mapStr = campaign.getGameMap().getMapString();
            sf::Text mapText(mapStr, font, static_cast<unsigned int>(14 * (windowSize.y / 768.0f)));
            mapText.setPosition(windowSize.x * 0.01f, windowSize.y * 0.01f);
            mapText.setFillColor(sf::Color::White);
            window.draw(mapText);

            // Display location info
            std::string locationInfo = "Location: " + campaign.getCurrentLocation().name + "\nDifficulty: " + std::to_string(campaign.getCurrentDifficulty());
            sf::Text infoText(locationInfo, font, static_cast<unsigned int>(16 * (windowSize.y / 768.0f)));
            infoText.setPosition(windowSize.x * 0.01f, windowSize.y * 0.5f);
            infoText.setFillColor(sf::Color::Yellow);
            window.draw(infoText);

            // Display party status
            std::string partyStatus = "Party Status:\n";
            const auto &party = campaign.getPlayerParty();
            for (Player *player : party)
            {
                if (player)
                {
                    partyStatus += player->getName() + " (Lv." + std::to_string(player->getLevel()) + ") - " +
                                   std::to_string(player->getReceivedExperience()) + "/" +
                                   std::to_string(player->getRequiredExperience()) + " EXP\n";
                }
            }
            sf::Text partyText(partyStatus, font, static_cast<unsigned int>(14 * (windowSize.y / 768.0f)));
            partyText.setPosition(windowSize.x * 0.01f, windowSize.y * 0.55f);
            partyText.setFillColor(sf::Color::Cyan);
            window.draw(partyText);

            // Display pending experience message if any
            if (!pendingExpMessage.empty())
            {
                sf::Text expText(pendingExpMessage, font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)));
                expText.setPosition(windowSize.x * 0.3f, windowSize.y * 0.7f);
                expText.setFillColor(sf::Color::Yellow);
                window.draw(expText);
                // Clear message after displaying
                pendingExpMessage.clear();
            }

            // Inventory button in corner
            sf::RectangleShape invButton(sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.04f));
            invButton.setPosition(windowSize.x * 0.7f, windowSize.y * 0.01f);
            invButton.setFillColor(sf::Color::Blue);
            window.draw(invButton);

            sf::Text invText("Inventory", font, static_cast<unsigned int>(16 * (windowSize.y / 768.0f)));
            invText.setPosition(windowSize.x * 0.71f, windowSize.y * 0.015f);
            invText.setFillColor(sf::Color::White);
            window.draw(invText);
            break;
        }
        case GameState::TREASURE:
            for (auto &text : treasureTexts)
            {
                text.draw(window);
            }
            treasureMenu.draw();
            break;
        case GameState::EVENT:
            for (auto &text : eventTexts)
            {
                text.draw(window);
            }
            eventMenu.draw();
            break;
        case GameState::EXIT:
            for (auto &text : exitTexts)
            {
                text.draw(window);
            }
            exitMenu.draw();
            break;
        case GameState::BATTLE:
            for (auto &text : battleTexts)
            {
                text.draw(window);
            }
            battleMenu.draw();
            break;
        case GameState::INVENTORY:
            for (auto &text : inventoryTexts)
            {
                text.draw(window);
            }
            inventoryMenu.draw();
            break;
        case GameState::VICTORY:
        {
            // Victory screen
            sf::Text victoryTitle("VICTORY!", font, static_cast<unsigned int>(56 * (windowSize.y / 768.0f)));
            victoryTitle.setPosition(windowSize.x * 0.25f, windowSize.y * 0.2f);
            victoryTitle.setFillColor(sf::Color::Green);
            window.draw(victoryTitle);

            sf::Text victoryText("You have defeated the Lord of Darkness and saved the world!", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)));
            victoryText.setPosition(windowSize.x * 0.15f, windowSize.y * 0.3f);
            victoryText.setFillColor(sf::Color::White);
            window.draw(victoryText);

            // Main Menu button
            sf::RectangleShape mainMenuButton(sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.05f));
            mainMenuButton.setPosition(windowSize.x * 0.35f, windowSize.y * 0.4f);
            mainMenuButton.setFillColor(sf::Color::Blue);
            window.draw(mainMenuButton);

            sf::Text mainMenuText("Main Menu", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)));
            mainMenuText.setPosition(windowSize.x * 0.355f, windowSize.y * 0.41f);
            mainMenuText.setFillColor(sf::Color::White);
            window.draw(mainMenuText);
            break;
        }
        case GameState::DEFEAT:
        {
            // Defeat screen
            sf::Text defeatTitle("DEFEAT!", font, static_cast<unsigned int>(56 * (windowSize.y / 768.0f)));
            defeatTitle.setPosition(windowSize.x * 0.25f, windowSize.y * 0.2f);
            defeatTitle.setFillColor(sf::Color::Red);
            window.draw(defeatTitle);

            sf::Text defeatText("All your heroes have fallen in battle. Try again!", font, static_cast<unsigned int>(24 * (windowSize.y / 768.0f)));
            defeatText.setPosition(windowSize.x * 0.15f, windowSize.y * 0.3f);
            defeatText.setFillColor(sf::Color::White);
            window.draw(defeatText);

            // Main Menu button
            sf::RectangleShape defeatMainMenuButton(sf::Vector2f(windowSize.x * 0.1f, windowSize.y * 0.05f));
            defeatMainMenuButton.setPosition(windowSize.x * 0.35f, windowSize.y * 0.4f);
            defeatMainMenuButton.setFillColor(sf::Color::Blue);
            window.draw(defeatMainMenuButton);

            sf::Text defeatMainMenuText("Main Menu", font, static_cast<unsigned int>(20 * (windowSize.y / 768.0f)));
            defeatMainMenuText.setPosition(windowSize.x * 0.355f, windowSize.y * 0.41f);
            defeatMainMenuText.setFillColor(sf::Color::White);
            window.draw(defeatMainMenuText);
            break;
        }
        }

        window.display();
    }

    return 0;
}
