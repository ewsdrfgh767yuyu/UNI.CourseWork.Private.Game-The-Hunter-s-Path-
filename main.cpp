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
    INVENTORY
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

    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 1000), "The Hunter's Path");
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

    // Main menu
    Menu mainMenu(window, font);
    mainMenu.addButton("Start Game", sf::Vector2f(250, 200), sf::Vector2f(300, 60), [&]()
                       { currentState = GameState::CHARACTER_SELECTION; });
    mainMenu.addButton("Credits", sf::Vector2f(250, 280), sf::Vector2f(300, 60), [&]()
                       { currentState = GameState::CREDITS; });
    mainMenu.addButton("Exit", sf::Vector2f(250, 360), sf::Vector2f(300, 60), [&]()
                       { window.close(); });

    // Credits
    TextDisplay creditsText("Mikulski Stanislau - BSUIR student", font, 24, sf::Vector2f(200, 250));
    Menu creditsMenu(window, font);
    creditsMenu.addButton("Back", sf::Vector2f(350, 350), sf::Vector2f(100, 50), [&]()
                          { currentState = GameState::MAIN_MENU; });

    // Campaign
    CampaignSystem campaign;

    // Character selection
    Menu characterSelectionMenu(window, font);
    const auto &presets = HeroFactory::getPartyPresets();
    float yPos = 150;
    for (size_t i = 0; i < presets.size(); ++i)
    {
        characterSelectionMenu.addButton(presets[i].name, sf::Vector2f(100, yPos), sf::Vector2f(600, 50), [&, i]()
                                         {
                                            selectedPresetIndex = i;
                                            currentState = GameState::CHARACTER_CONFIRMATION; });
        yPos += 60;
    }
    characterSelectionMenu.addButton("Back", sf::Vector2f(350, yPos), sf::Vector2f(100, 50), [&]()
                                     { currentState = GameState::MAIN_MENU; });
    characterSelectionMenu.setScrollable(true, 400.0f);

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
                    sf::FloatRect invButtonRect(700, 10, 100, 40);
                    if (invButtonRect.contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        currentState = GameState::INVENTORY;
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
            characterConfirmationMenu.addText(preset.description, 20, sf::Vector2f(50, 80), sf::Color::White);

            float yPos = 120;
            for (size_t i = 0; i < preset.heroes.size(); ++i)
            {
                const auto &heroPair = preset.heroes[i];
                const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(heroPair.first);

                // Имя героя
                characterConfirmationMenu.addText(heroPair.second + " (" + tmpl.name + ")", 20, sf::Vector2f(50, yPos), sf::Color::Yellow);
                yPos += 27;

                // Описание класса
                characterConfirmationMenu.addText(tmpl.description, 18, sf::Vector2f(70, yPos), sf::Color::White);
                yPos += 22;

                // Характеристики
                characterConfirmationMenu.addText("Характеристики:", 18, sf::Vector2f(70, yPos), sf::Color::Magenta);
                yPos += 20;
                characterConfirmationMenu.addText("Здоровье (HP): " + std::to_string(tmpl.baseMaxHP) + " - максимальное здоровье героя", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 16;
                characterConfirmationMenu.addText("Урон (Damage): " + std::to_string(tmpl.baseDamage) + " - базовый урон в атаке", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 16;
                characterConfirmationMenu.addText("Защита (Defense): " + std::to_string(tmpl.baseDefense) + " - снижает входящий урон", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 16;
                characterConfirmationMenu.addText("Атака (Attack): " + std::to_string(tmpl.baseAttack) + " - точность атаки", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 16;
                characterConfirmationMenu.addText("Выносливость (Stamina): " + std::to_string(tmpl.baseMaxStamina) + " - очки для действий", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 16;
                characterConfirmationMenu.addText("Инициатива (Initiative): " + std::to_string(tmpl.baseInitiative) + " - определяет порядок хода", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 16;
                characterConfirmationMenu.addText("Дальность (Range): " + std::to_string(tmpl.baseAttackRange) + " - тип боя (0 - ближний)", 14, sf::Vector2f(90, yPos), sf::Color::White);
                yPos += 20;

                // Способности
                characterConfirmationMenu.addText("Способности:", 18, sf::Vector2f(70, yPos), sf::Color::Cyan);
                yPos += 20;
                for (AbilityType abilityType : tmpl.availableAbilities)
                {
                    const AbilityInfo &ability = HeroFactory::getAbilityInfo(abilityType);
                    characterConfirmationMenu.addText(ability.name + ":", 16, sf::Vector2f(90, yPos), sf::Color::Yellow);
                    yPos += 18;
                    characterConfirmationMenu.addText(ability.effect, 12, sf::Vector2f(110, yPos), sf::Color::Green);
                    yPos += 16;
                }

                yPos += 10; // Отступ между героями
            }

            // Кнопки
            characterConfirmationMenu.addButton("Confirm", sf::Vector2f(200, yPos), sf::Vector2f(150, 50), [&]()
                                                {
                                                    campaign.createPlayerPartyFromPreset(selectedPresetIndex);
                                                    currentState = GameState::CAMPAIGN; });
            characterConfirmationMenu.addButton("Back", sf::Vector2f(400, yPos), sf::Vector2f(150, 50), [&]()
                                                { currentState = GameState::CHARACTER_SELECTION; });
            characterConfirmationMenu.setScrollable(true, 500.0f);
        }

        // Update treasure menu
        if (currentState == GameState::TREASURE && campaign.hasPendingTreasure())
        {
            treasureMenu.clear();
            treasureTexts.clear();

            Item *treasure = campaign.getPendingTreasure();
            if (treasure)
            {
                treasureTexts.emplace_back("You found a treasure!", font, 30, sf::Vector2f(250, 100), sf::Color::Yellow);
                treasureTexts.emplace_back(treasure->getName(), font, 24, sf::Vector2f(250, 140), sf::Color::White);
                treasureTexts.emplace_back(treasure->getDescription(), font, 20, sf::Vector2f(250, 170), sf::Color::White);

                // Take button
                treasureMenu.addButton("Take", sf::Vector2f(250, 250), sf::Vector2f(100, 60), [&]()
                                       {
                                           campaign.takePendingTreasure();
                                           currentState = GameState::MAP_MODE; });

                // Decline button
                treasureMenu.addButton("Decline", sf::Vector2f(400, 250), sf::Vector2f(100, 60), [&]()
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
            eventTexts.emplace_back(evt.description, font, 24, sf::Vector2f(50, 100), sf::Color::White);

            float yPos = 150;
            for (size_t i = 0; i < evt.choices.size(); ++i)
            {
                eventMenu.addButton(evt.choices[i], sf::Vector2f(100, yPos), sf::Vector2f(400, 50), [&, i]()
                                    {
                                        campaign.handleEventChoice(i);
                                        campaign.clearPendingEvent();
                                        currentState = GameState::MAP_MODE; });
                yPos += 60;
            }
        }

        // Update exit menu
        if (currentState == GameState::EXIT && campaign.hasPendingExit())
        {
            exitMenu.clear();
            exitTexts.clear();

            const CampaignEvent &evt = campaign.getPendingExit();
            exitTexts.emplace_back(evt.description, font, 24, sf::Vector2f(50, 100), sf::Color::White);
            exitTexts.emplace_back("Choose a location to transition to:", font, 20, sf::Vector2f(50, 140), sf::Color::White);

            float yPos = 180;
            const auto &connections = campaign.getCurrentLocation().connections;
            for (size_t i = 0; i < connections.size(); ++i)
            {
                LocationType locType = connections[i];
                const Location &loc = campaign.getLocation(locType);
                std::string buttonText = std::to_string(i + 1) + ". " + loc.name + " - " + loc.description;
                exitMenu.addButton(buttonText, sf::Vector2f(50, yPos), sf::Vector2f(600, 50), [&, i]()
                                   {
                                       campaign.handleExitChoice(i);
                                       campaign.clearPendingExit();
                                       currentState = GameState::MAP_MODE; });
                yPos += 60;
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
                    battleTexts.emplace_back("BATTLE", font, 36, sf::Vector2f(250, 20), sf::Color::Yellow);
                    battleTexts.emplace_back(battle->getBattleStatus(), font, 18, sf::Vector2f(50, 60), sf::Color::White);
                    battleTexts.emplace_back(battle->getTurnOrderString(), font, 18, sf::Vector2f(450, 60), sf::Color::White);
                    if (battle->isPlayerVictory())
                    {
                        battleTexts.emplace_back("VICTORY!", font, 56, sf::Vector2f(250, 250), sf::Color::Green);
                        battleMenu.addButton("Continue", sf::Vector2f(300, 400), sf::Vector2f(120, 60), [&]()
                                             {
                            // Award experience
                            int totalExp = 0;
                            for (const auto &pos : battle->getEnemyPositions()) {
                                if (pos.entity) totalExp += static_cast<Enemy*>(pos.entity)->getExperienceValue();
                            }
                            int expPerPlayer = totalExp / campaign.getPlayerParty().size();
                            for (Player *player : campaign.getPlayerParty()) {
                                if (player->getCurrentHealthPoint() > 0) {
                                    player->setReceivedExperience(player->getReceivedExperience() + expPerPlayer);
                                    player->upLevel();
                                    cout << player->getName() << " receives " << expPerPlayer << " experience!\n";
                                }
                            }
                            // Check if this was the final boss battle
                            if (campaign.getCurrentLocation().isFinalBossLocation) {
                                campaign.setGameCompleted(true);
                            }
                            campaign.clearPendingBattle();
                            currentState = GameState::MAP_MODE;
                            battleState = BattleState::MAIN_MENU; });
                    }
                    else
                    {
                        battleTexts.emplace_back("DEFEAT!", font, 56, sf::Vector2f(250, 250), sf::Color::Red);
                        battleMenu.addButton("Continue", sf::Vector2f(300, 400), sf::Vector2f(120, 60), [&]()
                                             {
                            campaign.clearPendingBattle();
                            currentState = GameState::MAP_MODE;
                            battleState = BattleState::MAIN_MENU; });
                    }
                }
                else
                {
                    // Display battlefield
                    battleTexts.emplace_back("BATTLE", font, 30, sf::Vector2f(250, 20), sf::Color::Yellow);
                    battleTexts.emplace_back(battle->getBattleStatus(), font, 18, sf::Vector2f(50, 60), sf::Color::White);
                    float yPos = 500;

                    // Current turn entity
                    Entity *currentEntity = battle->getCurrentTurnEntity();
                    if (currentEntity)
                    {
                        string turnText = "Current Turn: " + currentEntity->getName() + " (Stamina: " + to_string(currentEntity->getCurrentStamina()) + "/" + to_string(currentEntity->getMaxStamina()) + ")";
                        battleTexts.emplace_back(turnText, font, 24, sf::Vector2f(50, yPos + 20), sf::Color::Cyan);
                        yPos += 50;

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
                                battleMenu.addButton("Attack", sf::Vector2f(50, yPos), sf::Vector2f(120, 50), [&]()
                                                     { battleState = BattleState::SELECT_TARGET_ATTACK; });
                                battleMenu.addButton("Use Ability", sf::Vector2f(180, yPos), sf::Vector2f(140, 50), [&]()
                                                     { battleState = BattleState::SELECT_ABILITY; });
                                battleMenu.addButton("Move", sf::Vector2f(330, yPos), sf::Vector2f(100, 50), [&]()
                                                     { battleState = BattleState::SELECT_POSITION_MOVE; });
                                battleMenu.addButton("Skip Turn", sf::Vector2f(440, yPos), sf::Vector2f(120, 50), [&]()
                                                     {
                                // Skip turn
                                currentEntity->setCurrentStamina(0);
                                battleState = BattleState::MAIN_MENU; });
                                battleMenu.addButton("End Turn", sf::Vector2f(570, yPos), sf::Vector2f(120, 50), [&]()
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
                                    battleTexts.emplace_back("Select Target to Attack:", font, 20, sf::Vector2f(50, yPos), sf::Color::White);
                                    yPos += 35;
                                    for (size_t i = 0; i < targets.size(); ++i)
                                    {
                                        string targetText = to_string(i + 1) + ". " + targets[i].first->getName() + " (HP: " + to_string(targets[i].first->getCurrentHealthPoint()) + ")";
                                        battleMenu.addButton(targetText, sf::Vector2f(50, yPos), sf::Vector2f(350, 40), [&, i, targets]()
                                                             {
                                            battle->attack(currentEntity, targets[i].first);
                                            battleState = BattleState::MAIN_MENU; });
                                        yPos += 45;
                                    }
                                }
                                else
                                {
                                    battleTexts.emplace_back("No targets available for attack!", font, 20, sf::Vector2f(50, yPos), sf::Color::Red);
                                    yPos += 35;
                                }
                                battleMenu.addButton("Back", sf::Vector2f(50, yPos), sf::Vector2f(100, 40), [&]()
                                                     { battleState = BattleState::MAIN_MENU; });
                            }
                            else if (battleState == BattleState::SELECT_ABILITY)
                            {
                                // Show available abilities
                                Player *player = static_cast<Player *>(currentEntity);
                                const vector<AbilityType> &abilities = player->getAvailableAbilities();
                                if (!abilities.empty())
                                {
                                    battleTexts.emplace_back("Select Ability:", font, 20, sf::Vector2f(50, yPos), sf::Color::White);
                                    yPos += 35;
                                    for (size_t i = 0; i < abilities.size(); ++i)
                                    {
                                        const AbilityInfo &info = HeroFactory::getAbilityInfo(abilities[i]);
                                        string abilityText = to_string(i + 1) + ". " + info.name;
                                        battleMenu.addButton(abilityText, sf::Vector2f(50, yPos), sf::Vector2f(300, 40), [&, i, abilities]()
                                                             {
                                            selectedAbility = abilities[i];
                                            battleState = BattleState::CONFIRM_ABILITY; });
                                        yPos += 50;
                                    }
                                }
                                else
                                {
                                    battleTexts.emplace_back("No abilities available!", font, 20, sf::Vector2f(50, yPos), sf::Color::Red);
                                    yPos += 35;
                                }
                                battleMenu.addButton("Back", sf::Vector2f(50, yPos), sf::Vector2f(100, 40), [&]()
                                                     { battleState = BattleState::MAIN_MENU; });
                            }
                            else if (battleState == BattleState::CONFIRM_ABILITY)
                            {
                                // Show ability description and confirm
                                const AbilityInfo &info = HeroFactory::getAbilityInfo(selectedAbility);
                                battleTexts.emplace_back("Confirm Ability: " + info.name, font, 22, sf::Vector2f(50, yPos), sf::Color::Yellow);
                                yPos += 35;
                                battleTexts.emplace_back("Description: " + info.description, font, 18, sf::Vector2f(50, yPos), sf::Color::White);
                                yPos += 25;
                                battleTexts.emplace_back("Effect: " + info.effect, font, 18, sf::Vector2f(50, yPos), sf::Color::White);
                                yPos += 35;
                                battleMenu.addButton("Confirm", sf::Vector2f(50, yPos), sf::Vector2f(120, 50), [&]()
                                                     { battleState = BattleState::SELECT_TARGET_ABILITY; });
                                battleMenu.addButton("Back", sf::Vector2f(180, yPos), sf::Vector2f(100, 40), [&]()
                                                     { battleState = BattleState::SELECT_ABILITY; });
                            }
                            else if (battleState == BattleState::SELECT_TARGET_ABILITY)
                            {
                                // Show targets for ability (simplified - assume area effect or single target)
                                vector<pair<Entity *, int>> targets = battle->getAvailableTargetsForCurrent();
                                if (!targets.empty())
                                {
                                    battleTexts.emplace_back("Select Target for Ability:", font, 20, sf::Vector2f(50, yPos), sf::Color::White);
                                    yPos += 35;
                                    for (size_t i = 0; i < targets.size(); ++i)
                                    {
                                        string targetText = to_string(i + 1) + ". " + targets[i].first->getName();
                                        battleMenu.addButton(targetText, sf::Vector2f(50, yPos), sf::Vector2f(250, 40), [&, i, targets]()
                                                             {
                                            battle->useAbility(currentEntity, selectedAbility);
                                            battleState = BattleState::MAIN_MENU; });
                                        yPos += 45;
                                    }
                                }
                                else
                                {
                                    // Use ability without target
                                    battleMenu.addButton("Use Ability", sf::Vector2f(50, yPos), sf::Vector2f(150, 40), [&]()
                                                         {
                                        battle->useAbility(currentEntity, selectedAbility);
                                        battleState = BattleState::MAIN_MENU; });
                                    yPos += 45;
                                }
                                battleMenu.addButton("Back", sf::Vector2f(50, yPos), sf::Vector2f(100, 40), [&]()
                                                     { battleState = BattleState::CONFIRM_ABILITY; });
                            }
                            else if (battleState == BattleState::SELECT_POSITION_MOVE)
                            {
                                // Show position buttons
                                battleTexts.emplace_back("Select Position to Move:", font, 20, sf::Vector2f(50, yPos), sf::Color::White);
                                yPos += 35;
                                for (int pos = 0; pos < 4; ++pos)
                                {
                                    string posText = "Position " + to_string(pos + 1);
                                    battleMenu.addButton(posText, sf::Vector2f(50.0f + static_cast<float>(pos) * 120.0f, yPos), sf::Vector2f(110, 40), [&, pos]()
                                                         {
                                        battle->movePosition(currentEntity, pos);
                                        battleState = BattleState::MAIN_MENU; });
                                }
                                battleMenu.addButton("Back", sf::Vector2f(50, yPos + 50), sf::Vector2f(100, 40), [&]()
                                                     { battleState = BattleState::MAIN_MENU; });
                            }
                        }
                        else
                        {
                            // AI turn - simulate
                            battleTexts.emplace_back("AI Turn - Press Next to continue", font, 20, sf::Vector2f(50, yPos), sf::Color::Yellow);
                            battleMenu.addButton("Next", sf::Vector2f(50, yPos + 35), sf::Vector2f(100, 40), [&]()
                                                 {
                                // Simple AI: attack if possible, else skip turn
                                vector<pair<Entity *, int>> targets = battle->getAvailableTargetsForCurrent();
                                if (!targets.empty()) {
                                    int targetIndex = rand() % targets.size();
                                    battle->attack(currentEntity, targets[targetIndex].first);
                                } else {
                                    battle->nextTurn();
                                } });
                            battleMenu.addButton("End AI Turn", sf::Vector2f(160, yPos + 35), sf::Vector2f(120, 40), [&]()
                                                 { battle->nextTurn(); });
                        }
                    }
                }
            }
            else
            {
                // Fallback
                battleTexts.emplace_back("Battle system error.", font, 24, sf::Vector2f(200, 100), sf::Color::Red);
                battleMenu.addButton("Continue", sf::Vector2f(300, 200), sf::Vector2f(100, 50), [&]()
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

            inventoryTexts.emplace_back("Party Inventory", font, 30, sf::Vector2f(250, 50), sf::Color::Yellow);

            const auto &partyInv = campaign.getPartyInventory();
            float yPos = 100;
            for (size_t i = 0; i < partyInv.size(); ++i)
            {
                inventoryTexts.emplace_back(std::to_string(i + 1) + ". " + partyInv[i].getName(), font, 20, sf::Vector2f(50, yPos), sf::Color::White);
                yPos += 25;
            }

            // Close button
            inventoryMenu.addButton("Close", sf::Vector2f(350, 500), sf::Vector2f(100, 50), [&]()
                                    { currentState = GameState::MAP_MODE; });
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
            // Check if game is completed
            if (campaign.isGameCompleted())
            {
                // Victory screen
                sf::Text victoryTitle("VICTORY!", font, 56);
                victoryTitle.setPosition(250, 200);
                victoryTitle.setFillColor(sf::Color::Green);
                window.draw(victoryTitle);

                sf::Text victoryText("You have defeated the Lord of Darkness and saved the world!", font, 24);
                victoryText.setPosition(150, 300);
                victoryText.setFillColor(sf::Color::White);
                window.draw(victoryText);

                // Exit button
                sf::RectangleShape exitButton(sf::Vector2f(100, 50));
                exitButton.setPosition(350, 400);
                exitButton.setFillColor(sf::Color::Red);
                window.draw(exitButton);

                sf::Text exitText("Exit", font, 20);
                exitText.setPosition(375, 410);
                exitText.setFillColor(sf::Color::White);
                window.draw(exitText);

                // Handle exit button click
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect exitRect(350, 400, 100, 50);
                    if (exitRect.contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        window.close();
                    }
                }
            }
            else
            {
                // Display map
                std::string mapStr = campaign.getGameMap().getMapString();
                sf::Text mapText(mapStr, font, 14);
                mapText.setPosition(10, 10);
                mapText.setFillColor(sf::Color::White);
                window.draw(mapText);

                // Display location info
                std::string locationInfo = "Location: " + campaign.getCurrentLocation().name + "\nDifficulty: " + std::to_string(campaign.getCurrentDifficulty());
                sf::Text infoText(locationInfo, font, 16);
                infoText.setPosition(10, 500);
                infoText.setFillColor(sf::Color::Yellow);
                window.draw(infoText);

                // Inventory button in corner
                sf::RectangleShape invButton(sf::Vector2f(100, 40));
                invButton.setPosition(700, 10);
                invButton.setFillColor(sf::Color::Blue);
                window.draw(invButton);

                sf::Text invText("Inventory", font, 16);
                invText.setPosition(710, 15);
                invText.setFillColor(sf::Color::White);
                window.draw(invText);
            }
            break;
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
        }

        window.display();
    }

    return 0;
}
