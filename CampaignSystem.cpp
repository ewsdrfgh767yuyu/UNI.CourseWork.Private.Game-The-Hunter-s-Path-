#include "CampaignSystem.h"
#include "utils.h"
#include "ItemTemplates.h"
#include "EnemyTemplates.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <limits>

using namespace std;

CampaignSystem::CampaignSystem()
{
    initializeLocations();
    srand(static_cast<unsigned int>(time(nullptr)));
}

CampaignSystem::~CampaignSystem()
{
    cout << "[DEBUG] CampaignSystem destructor called\n";
    cleanupParty();
    if (pendingTreasure)
    {
        delete pendingTreasure;
        pendingTreasure = nullptr;
    }
    cout << "[DEBUG] CampaignSystem destructor completed\n";
}

void CampaignSystem::initializeLocations()
{
    // FOREST
    Location forest;
    forest.type = LocationType::FOREST;
    forest.name = "Forest";
    forest.description = "Dense forest full of dangerous creatures and ancient secrets.";
    forest.connections = {LocationType::CAVE, LocationType::DEAD_CITY};
    forest.isFinalBossLocation = false;

    // CAVES
    Location cave;
    cave.type = LocationType::CAVE;
    cave.name = "Caves";
    cave.description = "Deep underground caves inhabited by troglodytes and elementals.";
    cave.connections = {LocationType::FOREST, LocationType::DEAD_CITY};
    cave.isFinalBossLocation = false;

    // DEAD CITY
    Location deadCity;
    deadCity.type = LocationType::DEAD_CITY;
    deadCity.name = "Dead City";
    deadCity.description = "Abandoned city populated by undead and ghosts.";
    deadCity.connections = {LocationType::CAVE, LocationType::CASTLE};
    deadCity.isFinalBossLocation = false;

    // CASTLE
    Location castle;
    castle.type = LocationType::CASTLE;
    castle.name = "Castle";
    castle.description = "Ancient castle where the final boss and his minions reside.";
    castle.connections = {};
    castle.isFinalBossLocation = true;

    locations[LocationType::FOREST] = forest;
    locations[LocationType::CAVE] = cave;
    locations[LocationType::DEAD_CITY] = deadCity;
    locations[LocationType::CASTLE] = castle;

    // Start with forest
    currentLocation = forest;
}

void CampaignSystem::startCampaign()
{
    cout << "+================================================================+\n";
    cout << "|                    CAMPAIGN MODE                               |\n";
    cout << "|           Welcome to the Hunter's Path!                        |\n";
    cout << "+================================================================+\n";
    cout << "\n";

    createPlayerParty();

    // Start with forest
    currentLocation = locations[LocationType::FOREST];

    // Generate fixed map
    gameMap.generateFixedMap();
    // Print map immediately after generation for debugging
    cout << "Map after generation:\n";
    gameMap.printMap();
    cout << "\nPress Enter to continue...";
    cin.get();

    // Start map mode
    runMapMode();
}

void CampaignSystem::initializeCampaign()
{
    // Start with forest
    currentLocation = locations[LocationType::FOREST];

    // Generate fixed map
    gameMap.generateFixedMap();
}

void CampaignSystem::createPlayerParty()
{
    cout << "\n=== PARTY BUILD SELECTION ===\n";

    // Get available presets
    const vector<PartyPreset> &presets = HeroFactory::getPartyPresets();

    cout << "Available party builds:\n";
    for (int i = 0; i < presets.size(); ++i)
    {
        cout << i + 1 << ". " << presets[i].name << "\n";
        cout << "   " << presets[i].description << "\n";
        cout << "   Composition: ";
        for (size_t j = 0; j < presets[i].heroes.size(); ++j)
        {
            const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(presets[i].heroes[j].first);
            cout << presets[i].heroes[j].second << " (" << tmpl.name << ")";
            if (j < presets[i].heroes.size() - 1)
                cout << ", ";
        }
        cout << "\n\n";
    }

    int presetChoice = getSafeIntInput("Choose build (1-" + to_string(presets.size()) + "): ", 1, static_cast<int>(presets.size()));

    // Create party from selected preset
    playerParty = HeroFactory::createPartyFromPreset(presetChoice - 1);

    // Display information about created party
    cout << "\nCreated party:\n";
    for (Player *hero : playerParty)
    {
        const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(hero->getHeroClass());
        cout << "- " << hero->getName() << " (" << tmpl.name << ")\n";
        cout << "  Stats: HP " << hero->getCurrentHealthPoint()
             << ", Damage " << hero->getDamage() << ", Defense " << hero->getDefense()
             << ", Attack Range " << hero->getAttackRange() << "\n";
    }
}

void CampaignSystem::createPlayerPartyFromPreset(int presetIndex)
{
    // Create party from selected preset without console output
    playerParty = HeroFactory::createPartyFromPreset(presetIndex);
}

void CampaignSystem::runCampaignLoop()
{
    while (!gameCompleted && !playerParty.empty())
    {
        // Clear console before each new message
        system("cls");

        displayLocationInfo();

        // Generate random event (prevent battle on first event)
        CampaignEvent currentEvent = generateRandomEvent();
        if (currentDifficulty == 0 && currentEvent.type == EventType::BATTLE)
        {
            // If first event is battle, replace with text or treasure
            currentEvent.type = EventType::TREASURE;
            currentEvent.description = "You found a hidden treasure!";
            currentEvent.reward = new Item("Gold coin", "Valuable coin", ItemType::CONSUMABLE, EquipmentSlot::NONE, {{"health_restore", 10}});
        }

        // Handle event
        switch (currentEvent.type)
        {
        case EventType::BATTLE:
            handleBattleEvent(currentEvent);
            break;
        case EventType::TREASURE:
            handleTreasureEvent(currentEvent);
            break;
        case EventType::TEXT_EVENT:
            handleTextEvent(currentEvent);
            break;
        case EventType::EXIT:
            handleExitEvent(currentEvent);
            break;
        case EventType::BOSS_BATTLE:
            handleBossBattleEvent(currentEvent);
            break;
        }

        // Check if party is alive
        bool partyAlive = false;
        for (Player *player : playerParty)
        {
            if (player->getCurrentHealthPoint() > 0)
            {
                partyAlive = true;
                break;
            }
        }

        if (!partyAlive)
        {
            cout << "\n[SKULL] Your party has fallen in battle! Game over.\n";
            break;
        }

        // Increase difficulty after each event
        currentDifficulty++;
    }
}

CampaignEvent CampaignSystem::generateRandomEvent()
{
    CampaignEvent event;

    // Event probability distribution
    int randomValue = rand() % 100;

    if (currentLocation.isFinalBossLocation)
    {
        // In castle always boss battle
        event.type = EventType::BOSS_BATTLE;
        event.description = "You encountered the final boss!";
    }
    else if (randomValue < 40)
    {
        // 40% chance for battle
        event.type = EventType::BATTLE;
        event.description = "You stumbled upon a group of enemies!";
        event.difficultyModifier = currentDifficulty;
    }
    else if (randomValue < 70)
    {
        // 30% chance for find
        event.type = EventType::TREASURE;
        event.description = "You found a hidden treasure!";
        // Generate random item
        Item randomItem = ItemFactory::createRandomItem();
        event.reward = new Item(randomItem);
    }
    else if (randomValue < 90)
    {
        // 20% chance for text event
        event.type = EventType::TEXT_EVENT;
        event.description = "You discovered a strange phenomenon...";
        event.choices = {"Investigate", "Ignore", "Attack"};
        event.outcomes = {EventType::TREASURE, EventType::BATTLE, EventType::BATTLE};
    }
    else
    {
        // 10% chance for exit
        event.type = EventType::EXIT;
        event.description = "You found the exit from this location.";
    }

    return event;
}

void CampaignSystem::handleBattleEvent(const CampaignEvent &event)
{
    // Create enemies based on difficulty
    vector<Entity *> enemies;
    int enemyCount = 1 + (rand() % 4); // 1-4 enemies
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy *enemy = EnemyFactory::createRandomEnemy(currentLocation.type, event.difficultyModifier);
        enemies.push_back(enemy);
    }

    // Calculate total experience
    pendingExperience = 0;
    for (Entity *enemy : enemies)
    {
        pendingExperience += static_cast<Enemy *>(enemy)->getExperienceValue();
    }

    // Convert Player* to Entity*
    vector<Entity *> playerEntities;
    for (Player *player : playerParty)
    {
        playerEntities.push_back(player);
    }

    // Create battle system
    currentBattle = new BattleSystem();
    currentBattle->startBattle(playerEntities, enemies);

    // Set pending for GUI
    pendingBattle = true;
}

void CampaignSystem::handleTreasureEvent(const CampaignEvent &event)
{
    if (event.reward)
    {
        // Set pending for GUI - item will be added only if player chooses to take it
        pendingTreasure = new Item(*event.reward);
        delete event.reward;
    }
}

void CampaignSystem::handleTextEvent(const CampaignEvent &event)
{
    // Set pending for GUI
    pendingEvent = event;
}

void CampaignSystem::handleExitEvent(const CampaignEvent &event)
{
    cout << "\n=== EXIT ===\n";
    cout << event.description << "\n";

    // Show available transitions
    if (!currentLocation.connections.empty())
    {
        cout << "Available transitions:\n";
        for (int i = 0; i < currentLocation.connections.size(); ++i)
        {
            LocationType locType = currentLocation.connections[i];
            Location loc = locations[locType];
            cout << i + 1 << ". " << loc.name << " - " << loc.description << "\n";
        }

        int choice = getSafeIntInput("Choose location to transition to (1-" + to_string(currentLocation.connections.size()) + "): ", 1, static_cast<int>(currentLocation.connections.size()));

        if (choice > 0 && choice <= static_cast<int>(currentLocation.connections.size()))
        {
            LocationType targetLocation = currentLocation.connections[choice - 1];
            Location newLoc = locations[targetLocation];

            cout << "\nDo you agree to go to " << newLoc.name << "? (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore();

            if (confirm == 'y' || confirm == 'Y')
            {
                cout << "You are transitioning to " << newLoc.name << "...\n";
                // Change current location
                currentLocation = newLoc;
                // Generate new map for new location
                gameMap.generateFixedMap();
                visitedNodes.clear();
                currentDifficulty++;

                // Restore health of all party members upon transitioning to new location
                for (Player *player : playerParty)
                {
                    if (player && player->getCurrentHealthPoint() > 0)
                    {
                        player->setCurrentHealthPoint(player->getMaxHealthPoint());
                        cout << player->getName() << " fully restored health!\n";
                    }
                }
            }
            else
            {
                cout << "You stay in place.\n";
            }
        }
        else
        {
            cout << "Invalid choice!\n";
        }
    }
    else
    {
        cout << "No available transitions from this location.\n";
    }

    cout << "\nPress Enter to continue...";
    cin.get();
}

void CampaignSystem::handleBossBattleEvent(const CampaignEvent &event)
{
    // Create the final boss
    vector<Entity *> bossParty;
    Enemy *finalBoss = new Enemy("Lord of Darkness", 300, 25, 10, 8, 3, 3, 15, 2, AbilityType::LIFE_STEAL, 200, 5, "final_boss", 0.1);
    bossParty.push_back(finalBoss);

    // Add a couple of minions (not from the castle)
    Enemy *minion1 = EnemyFactory::createEnemyByName("Vampire", 2);
    Enemy *minion2 = EnemyFactory::createEnemyByName("Troglodyte", 2);
    bossParty.push_back(minion1);
    bossParty.push_back(minion2);

    // Convert Player* to Entity*
    vector<Entity *> playerEntities;
    for (Player *player : playerParty)
    {
        playerEntities.push_back(player);
    }

    // Create battle system
    currentBattle = new BattleSystem();
    currentBattle->startBattle(playerEntities, bossParty);

    // Set pending for GUI
    pendingBattle = true;
}

void CampaignSystem::displayLocationInfo()
{
    cout << "\n+================================================================+\n";
    cout << "| Current location: " << currentLocation.name << string(max(0, 40 - (int)currentLocation.name.length()), ' ') << "|\n";
    cout << "| " << currentLocation.description << string(max(0, 58 - (int)currentLocation.description.length()), ' ') << "|\n";
    cout << "| Difficulty: " << currentDifficulty << string(46, ' ') << "|\n";
    cout << "+================================================================+\n";
}

void CampaignSystem::displayAvailableConnections()
{
    if (!currentLocation.connections.empty())
    {
        cout << "\nAvailable transitions:\n";
        for (LocationType locType : currentLocation.connections)
        {
            Location loc = locations[locType];
            cout << "- " << loc.name << "\n";
        }
    }
}

bool CampaignSystem::moveToLocation(LocationType targetLocation)
{
    auto it = locations.find(targetLocation);
    if (it != locations.end())
    {
        currentLocation = it->second;
        cout << "You have moved to " << currentLocation.name << ".\n";
        return true;
    }
    return false;
}

void CampaignSystem::runMapMode()
{
    while (!gameCompleted && !playerParty.empty())
    {
        // Очистка консоли перед каждым новым сообщением
        system("cls");

        displayMap();

        // Показываем доступные ходы
        auto validMoves = gameMap.getValidMoves();
        if (!validMoves.empty())
        {
            cout << "\nДоступные ходы: ";
            for (char move : validMoves)
            {
                cout << move << " ";
            }
            cout << "\n";
        }

        // Получаем ввод движения
        char move = getPlayerMovementInput();

        if (gameMap.movePlayer(move))
        {
            // Проверяем, наступили ли на специальный узел
            NodeType currentNode = gameMap.getCurrentNode();
            if (currentNode != NodeType::EMPTY && currentNode != NodeType::START)
            {
                Position currentPos = gameMap.getPlayerPosition();
                if (visitedNodes.find(currentPos) == visitedNodes.end())
                {
                    // Новое событие
                    visitedNodes[currentPos] = true;
                    handleNodeEvent(currentNode);
                }
            }
        }
        else
        {
            cout << "Неверный ход! Попробуйте снова.\n";
            cout << "\nНажмите Enter для продолжения...";
            cin.get();
        }

        // Проверяем, жив ли отряд
        cout << "[DEBUG] Checking party status, party size: " << playerParty.size() << "\n";
        bool partyAlive = false;
        for (Player *player : playerParty)
        {
            if (player)
            {
                cout << "[DEBUG] Player " << player->getName() << " HP: " << player->getCurrentHealthPoint() << "\n";
                if (player->getCurrentHealthPoint() > 0)
                {
                    partyAlive = true;
                    cout << "[DEBUG] Found alive player: " << player->getName() << "\n";
                    break;
                }
            }
        }

        if (!partyAlive)
        {
            cout << "\n[SKULL] Your party has fallen in battle! Game over.\n";
            cout << "[DEBUG] Breaking from runMapMode loop\n";
            break;
        }
    }
}

char CampaignSystem::getPlayerMovementInput()
{
    cout << "Enter movement direction (w/a/s/d): ";
    string input;
    while (true)
    {
        getline(cin, input);
        if (!input.empty())
        {
            char move = input[0];
            if (move == 'w' || move == 'a' || move == 's' || move == 'd')
            {
                return move;
            }
        }
        cout << "Invalid input. Use w/a/s/d: ";
    }
}

void CampaignSystem::displayMap()
{
    cout << "+================================================================+\n";
    cout << "|                           MAP                                  |\n";
    cout << "+================================================================+\n";
    cout << "\n";

    gameMap.printMap();

    cout << "\nLegend:\n";
    cout << "@ - You (player)\n";
    cout << "I - Entrance (start)\n";
    cout << "B - Battle\n";
    cout << "S - Event\n";
    cout << "P - Item (treasure)\n";
    cout << "V - Exit\n";
    cout << ". - Empty cell\n";
    cout << "? - Unvisited cell in visibility radius\n";
    cout << "  - Cell outside visibility radius\n";
}

void CampaignSystem::handleNodeEvent(NodeType nodeType)
{
    switch (nodeType)
    {
    case NodeType::BATTLE:
    {
        if (currentLocation.type == LocationType::CASTLE)
        {
            CampaignEvent bossEvent{EventType::BOSS_BATTLE, "You have encountered the final boss!"};
            handleBossBattleEvent(bossEvent);
        }
        else
        {
            CampaignEvent battleEvent{EventType::BATTLE, "You have stumbled upon a group of enemies!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        break;
    }
    case NodeType::EVENT:
    {
        // Mysterious event that leads to investigation
        std::string eventDescriptions[] = {
            "You hear a strange noise coming from the shadows. It sounds like something is lurking nearby...",
            "In the distance, you see an unusual glow emanating from a hidden grove. What could it be?",
            "The wind carries whispers of ancient secrets, and you spot a mysterious rune on the ground.",
            "A faint echo of footsteps echoes through the ruins, but no one is visible. Something is amiss...",
            "You notice a peculiar shimmer in the air, like a veil between worlds is thinning."};
        int descIndex = rand() % 5;
        std::vector<std::string> choices = {"Investigate", "Approach"};
        std::vector<EventType> outcomes = {EventType::BATTLE, EventType::TREASURE};
        CampaignEvent eventEvent{EventType::TEXT_EVENT, eventDescriptions[descIndex], choices, outcomes, nullptr, 0};
        handleTextEvent(eventEvent);
        break;
    }
    case NodeType::TREASURE:
    {
        Item randomItem = ItemFactory::createRandomItem();
        CampaignEvent treasureEvent{EventType::TREASURE, "You have found a hidden treasure!", {}, {}, new Item(randomItem), 0};
        handleTreasureEvent(treasureEvent);
        break;
    }
    case NodeType::EXIT:
    {
        if (currentLocation.type == LocationType::CASTLE)
        {
            CampaignEvent bossEvent{EventType::BOSS_BATTLE, "You have found the exit and faced the final boss!"};
            handleBossBattleEvent(bossEvent);
        }
        else
        {
            CampaignEvent exitEvent{EventType::EXIT, "You have found the exit from this location."};
            pendingExit = exitEvent;
        }
        break;
    }
    case NodeType::FOREST:
    {
        // Point of interest: Forest - random event with literary descriptions
        int randEvent = rand() % 4;
        if (randEvent == 0)
        {
            std::string battleDescs[] = {
                "The rustle of leaves foretells danger - wild beasts of the forest are out hunting!",
                "Ancient trees whisper of intruders, and here they are: packs of wolves and bears!",
                "Deep in the thicket, roars echo - forest predators defend their territory."};
            int descIdx = rand() % 3;
            CampaignEvent battleEvent{EventType::BATTLE, battleDescs[descIdx], {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else if (randEvent == 1)
        {
            std::string treasureDescs[] = {
                "Among the roots of an ancient oak, you find an elven amulet shimmering in the moonlight.",
                "In a stream fed by forest springs, a crystal of pure water gleams.",
                "On the branches of a sacred tree hangs the fruit of eternal youth, a gift of nature."};
            int descIdx = rand() % 3;
            Item randomItem = ItemFactory::createRandomItem();
            CampaignEvent treasureEvent{EventType::TREASURE, treasureDescs[descIdx], {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        else if (randEvent == 2)
        {
            // Battle variant
            CampaignEvent battleEvent{EventType::BATTLE, "Forest spirits summon guardians - ancient ents awaken!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else
        {
            // Treasure variant
            Item randomItem = ItemFactory::createRandomItemOfType(ItemType::ACCESSORY);
            CampaignEvent treasureEvent{EventType::TREASURE, "In the thicket, you find a forgotten altar with a magical artifact.", {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        break;
    }
    case NodeType::CAVE:
    {
        // Point of interest: Cave - underground mysteries
        int randEvent = rand() % 4;
        if (randEvent == 0)
        {
            std::string battleDescs[] = {
                "Footsteps echo from the cave darkness - troglodytes and their pets attack!",
                "Water drops echo in the grotto - goblins are setting up an ambush!",
                "Red eyes glow deep in the tunnel - earth elementals rise up!"};
            int descIdx = rand() % 3;
            CampaignEvent battleEvent{EventType::BATTLE, battleDescs[descIdx], {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else if (randEvent == 1)
        {
            std::string treasureDescs[] = {
                "In a crystal cave, you find a precious stone pulsing with inner energy.",
                "Among the stalactites gleams an ancient crystal holding secrets of the underworld.",
                "In an abandoned mine, you unearthed a magical gem capable of controlling the elements."};
            int descIdx = rand() % 3;
            Item randomItem = ItemFactory::createRandomItem();
            CampaignEvent treasureEvent{EventType::TREASURE, treasureDescs[descIdx], {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        else if (randEvent == 2)
        {
            // Battle variant
            CampaignEvent battleEvent{EventType::BATTLE, "Cave guardians awaken - giant spiders and scorpions!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else
        {
            // Treasure variant
            Item randomItem = ItemFactory::createRandomItemOfType(ItemType::WEAPON);
            CampaignEvent treasureEvent{EventType::TREASURE, "In a secret chamber, you find ancient weapons forgotten through the ages.", {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        break;
    }
    case NodeType::DEAD_CITY:
    {
        // Point of interest: Dead City - haunted ruins
        int randEvent = rand() % 4;
        if (randEvent == 0)
        {
            std::string battleDescs[] = {
                "Ghostly silhouettes materialize - the dead citizens of the city rise up!",
                "Echoes of footsteps in empty streets - skeletons and zombies emerge from graves!",
                "The city's curse strikes - the undead awaken to new life."};
            int descIdx = rand() % 3;
            CampaignEvent battleEvent{EventType::BATTLE, battleDescs[descIdx], {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else if (randEvent == 1)
        {
            std::string treasureDescs[] = {
                "In the palace ruins, you find a royal crown holding echoes of past glory.",
                "Among the dusty ruins gleams a golden amulet that protected ancient rulers.",
                "In the city's forgotten treasury, you unearthed a magical artifact full of dark energy."};
            int descIdx = rand() % 3;
            Item randomItem = ItemFactory::createRandomItem();
            CampaignEvent treasureEvent{EventType::TREASURE, treasureDescs[descIdx], {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        else if (randEvent == 2)
        {
            // Battle variant
            CampaignEvent battleEvent{EventType::BATTLE, "Spirits of the past give no rest - vampires and liches attack!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else
        {
            // Treasure variant
            Item randomItem = ItemFactory::createRandomItemOfType(ItemType::ARMOR);
            CampaignEvent treasureEvent{EventType::TREASURE, "In the dungeons of the dead city, you find the armor of an ancient hero.", {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        break;
    }
    case NodeType::CASTLE:
    {
        // Point of interest: Castle - final boss
        CampaignEvent bossEvent{EventType::BOSS_BATTLE, "You have penetrated the heart of the castle and faced the Lord of Darkness!"};
        handleBossBattleEvent(bossEvent);
        break;
    }
    default:
        break;
    }
}

void CampaignSystem::manageInventory(Player *player)
{
    if (playerParty.empty())
        return;

    if (player == nullptr)
    {
        // Character selection for inventory management
        cout << "\n=== CHARACTER SELECTION ===\n";
        cout << "Choose a character to manage inventory:\n";
        for (int i = 0; i < playerParty.size(); ++i)
        {
            cout << i + 1 << ". " << playerParty[i]->getName() << " (HP: " << playerParty[i]->getCurrentHealthPoint() << "/" << playerParty[i]->getMaxHealthPoint() << ")\n";
        }
        cout << playerParty.size() + 1 << ". Exit inventory management\n";

        int heroChoice = getSafeIntInput("Choose (1-" + to_string(playerParty.size() + 1) + "): ", 1, static_cast<int>(playerParty.size() + 1));

        if (heroChoice > 0 && heroChoice <= static_cast<int>(playerParty.size()))
        {
            player = playerParty[heroChoice - 1];
        }
        else if (heroChoice == playerParty.size() + 1)
        {
            // Exit
            return;
        }
        else
        {
            cout << "Invalid choice!\n";
            return;
        }
    }

    while (true)
    {
        cout << "\n=== INVENTORY MANAGEMENT ===\n";
        cout << "Hero: " << player->getName() << "\n\n";
        cout << "[DEBUG] manageInventory called, partyInventory size: " << partyInventory.size() << ", player inventory size: " << player->getInventory().size() << "\n";

        player->displayInventory();
        cout << "\n";
        player->displayEquipment();

        cout << "\nChoose action:\n";
        cout << "1. Equip item\n";
        cout << "2. Unequip item\n";
        cout << "3. Use consumable item\n";
        cout << "4. Take item from party inventory\n";
        cout << "5. Exit inventory management\n";

        int choice = getSafeIntInput("Choose (1-5): ", 1, 5);

        if (choice == 1)
        {
            // Equip item
            if (player->getInventory().empty())
            {
                cout << "Inventory is empty!\n";
                continue;
            }

            cout << "Choose item to equip:\n";
            for (int i = 0; i < player->getInventory().size(); ++i)
            {
                cout << i + 1 << ". " << player->getInventory()[i].getName() << "\n";
            }

            int itemChoice = getSafeIntInput("Choose item (1-" + to_string(player->getInventory().size()) + "): ", 1, static_cast<int>(player->getInventory().size()));

            if (itemChoice > 0 && itemChoice <= static_cast<int>(player->getInventory().size()))
            {
                player->equipItem(itemChoice - 1);
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
        else if (choice == 2)
        {
            // Unequip item
            cout << "Choose slot to unequip:\n";
            cout << "1. Head\n";
            cout << "2. Chest\n";
            cout << "3. Hands\n";
            cout << "4. Legs\n";
            cout << "5. Feet\n";
            cout << "6. Right hand\n";
            cout << "7. Left hand\n";
            cout << "8. Neck\n";
            cout << "9. Ring 1\n";
            cout << "10. Ring 2\n";

            int slotChoice = getSafeIntInput("Choose slot (1-10): ", 1, 10);

            EquipmentSlot slot;
            switch (slotChoice)
            {
            case 1:
                slot = EquipmentSlot::HEAD;
                break;
            case 2:
                slot = EquipmentSlot::CHEST;
                break;
            case 3:
                slot = EquipmentSlot::HANDS;
                break;
            case 4:
                slot = EquipmentSlot::LEGS;
                break;
            case 5:
                slot = EquipmentSlot::FEET;
                break;
            case 6:
                slot = EquipmentSlot::MAIN_HAND;
                break;
            case 7:
                slot = EquipmentSlot::OFF_HAND;
                break;
            case 8:
                slot = EquipmentSlot::NECK;
                break;
            case 9:
                slot = EquipmentSlot::RING1;
                break;
            case 10:
                slot = EquipmentSlot::RING2;
                break;
            default:
                cout << "Invalid choice!\n";
                continue;
            }

            player->unequipItem(slot);
        }
        else if (choice == 3)
        {
            // Use consumable item
            if (player->getInventory().empty())
            {
                cout << "Inventory is empty!\n";
                continue;
            }

            cout << "Choose item to use:\n";
            for (int i = 0; i < player->getInventory().size(); ++i)
            {
                cout << i + 1 << ". " << player->getInventory()[i].getName() << "\n";
            }

            int itemChoice = getSafeIntInput("Choose item (1-" + to_string(player->getInventory().size()) + "): ", 1, static_cast<int>(player->getInventory().size()));

            if (itemChoice > 0 && itemChoice <= static_cast<int>(player->getInventory().size()))
            {
                player->useConsumable(itemChoice - 1);
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
        else if (choice == 4)
        {
            // Take item from party inventory
            if (partyInventory.empty())
            {
                cout << "Party inventory is empty!\n";
                continue;
            }

            cout << "Party inventory:\n";
            for (int i = 0; i < partyInventory.size(); ++i)
            {
                cout << i + 1 << ". " << partyInventory[i].getName() << "\n";
            }

            int itemChoice = getSafeIntInput("Choose item to take (1-" + to_string(partyInventory.size()) + "): ", 1, static_cast<int>(partyInventory.size()));

            if (itemChoice > 0 && itemChoice <= static_cast<int>(partyInventory.size()))
            {
                player->addItem(partyInventory[itemChoice - 1]);
                partyInventory.erase(partyInventory.begin() + itemChoice - 1);
                cout << "Item taken to personal inventory.\n";
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
        else if (choice == 5)
        {
            // Exit
            break;
        }
        else
        {
            cout << "Invalid choice!\n";
        }

        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    }
}

void CampaignSystem::handleEventChoice(int choiceIndex)
{
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(pendingEvent.choices.size()))
        return;

    EventType outcome;
    std::string consequenceDescription;
    if (choiceIndex == 0) // Investigate
    {
        // Randomly choose between battle or treasure
        outcome = (rand() % 2 == 0) ? EventType::BATTLE : EventType::TREASURE;
        consequenceDescription = (outcome == EventType::BATTLE) ? "Upon investigating, you discover hostile creatures guarding the source of the disturbance!" : "Your investigation reveals a hidden cache of valuable items!";
    }
    else if (choiceIndex == 1) // Approach
    {
        // Approach gives treasure
        outcome = EventType::TREASURE;
        consequenceDescription = "As you approach cautiously, you find a trove of treasures waiting to be claimed!";
    }
    else
    {
        return; // Invalid choice
    }

    switch (outcome)
    {
    case EventType::BATTLE:
        handleBattleEvent(CampaignEvent{EventType::BATTLE, consequenceDescription, {}, {}, nullptr, currentDifficulty});
        break;
    case EventType::TREASURE:
    {
        Item randomItem = ItemFactory::createRandomItem();
        handleTreasureEvent(CampaignEvent{EventType::TREASURE, consequenceDescription, {}, {}, new Item(randomItem), 0});
        break;
    }
    default:
        // No action
        break;
    }

    // Clear the pending event after handling
    clearPendingEvent();
}

void CampaignSystem::handleExitChoice(int choiceIndex)
{
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(currentLocation.connections.size()))
        return;

    LocationType targetLocation = currentLocation.connections[choiceIndex];
    Location newLoc = locations[targetLocation];

    // Change current location
    currentLocation = newLoc;
    // Generate new map for new location
    gameMap.generateFixedMap();
    visitedNodes.clear();
    currentDifficulty++;

    // Restore health of all party members upon transitioning to new location
    for (Player *player : playerParty)
    {
        if (player && player->getCurrentHealthPoint() > 0)
        {
            player->setCurrentHealthPoint(player->getMaxHealthPoint());
        }
    }
}

void CampaignSystem::cleanupParty()
{
    cout << "[DEBUG] CampaignSystem::cleanupParty() called, party size: " << playerParty.size() << "\n";
    for (Player *player : playerParty)
    {
        if (player)
        {
            cout << "[DEBUG] Deleting player: " << player->getName() << "\n";
            delete player;
            player = nullptr;
        }
    }
    playerParty.clear();
    cout << "[DEBUG] CampaignSystem::cleanupParty() completed\n";
}
