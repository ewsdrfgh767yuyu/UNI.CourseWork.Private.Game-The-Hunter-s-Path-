#include "CampaignSystem.h"
#include "utils.h"
#include "ItemTemplates.h"
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

    // Получаем доступные пресеты
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

    // Создаем отряд по выбранному пресету
    playerParty = HeroFactory::createPartyFromPreset(presetChoice - 1);

    // Выводим информацию о созданном отряде
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
    // Создаем отряд по выбранному пресету без консольного вывода
    playerParty = HeroFactory::createPartyFromPreset(presetIndex);
}

void CampaignSystem::runCampaignLoop()
{
    while (!gameCompleted && !playerParty.empty())
    {
        // Очистка консоли перед каждым новым сообщением
        system("cls");

        displayLocationInfo();

        // Генерируем случайное событие (предотвращаем бой на первом событии)
        CampaignEvent currentEvent = generateRandomEvent();
        if (currentDifficulty == 0 && currentEvent.type == EventType::BATTLE)
        {
            // Если первое событие - бой, заменяем на текст или сокровище
            currentEvent.type = EventType::TREASURE;
            currentEvent.description = "Вы нашли скрытое сокровище!";
            currentEvent.reward = new Item("Золотая монета", "Ценная монета", ItemType::CONSUMABLE, EquipmentSlot::NONE, {{"health_restore", 10}});
        }

        // Обрабатываем событие
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

        // Проверяем, жив ли отряд
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
            cout << "\n[SKULL] Ваш отряд пал в бою! Игра окончена.\n";
            break;
        }

        // Увеличиваем сложность после каждого события
        currentDifficulty++;
    }
}

CampaignEvent CampaignSystem::generateRandomEvent()
{
    CampaignEvent event;

    // Распределение вероятностей событий
    int randomValue = rand() % 100;

    if (currentLocation.isFinalBossLocation)
    {
        // В замке всегда бой с боссом
        event.type = EventType::BOSS_BATTLE;
        event.description = "Вы столкнулись с финальным боссом!";
    }
    else if (randomValue < 40)
    {
        // 40% шанс на бой
        event.type = EventType::BATTLE;
        event.description = "Вы наткнулись на группу врагов!";
        event.difficultyModifier = currentDifficulty;
    }
    else if (randomValue < 70)
    {
        // 30% шанс на находку
        event.type = EventType::TREASURE;
        event.description = "Вы нашли скрытое сокровище!";
        // Генерируем случайный предмет
        Item randomItem = ItemFactory::createRandomItem();
        event.reward = new Item(randomItem);
    }
    else if (randomValue < 90)
    {
        // 20% шанс на текстовый ивент
        event.type = EventType::TEXT_EVENT;
        event.description = "Вы обнаружили странное явление...";
        event.choices = {"Исследовать", "Игнорировать", "Атаковать"};
        event.outcomes = {EventType::TREASURE, EventType::BATTLE, EventType::BATTLE};
    }
    else
    {
        // 10% шанс на выход
        event.type = EventType::EXIT;
        event.description = "Вы нашли выход из этой локации.";
    }

    return event;
}

void CampaignSystem::handleBattleEvent(const CampaignEvent &event)
{
    cout << "\n=== BATTLE ===\n";
    cout << event.description << "\n";

    // Создаем врагов для текущей локации
    vector<Entity *> enemies;
    int enemyCount = 1 + (rand() % 3); // 1-3 врага

    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy *enemy = EnemyFactory::createRandomEnemy(currentLocation.type, event.difficultyModifier);
        enemies.push_back(enemy);
        cout << "Appeared " << enemy->getName() << "!\n";
    }

    // Создаем систему боя
    BattleSystem battleSystem;

    // Начинаем бой
    // Конвертируем Player* в Entity*
    vector<Entity *> playerEntities;
    for (Player *player : playerParty)
    {
        playerEntities.push_back(player);
    }
    battleSystem.startBattle(playerEntities, enemies);

    // Основной цикл боя
    bool playerWon = false; // Флаг победы игрока
    while (battleSystem.isBattleActive())
    {
        cout << "\n"
             << battleSystem.getBattleStatus() << "\n";
        battleSystem.printBattlefield();

        Entity *currentEntity = battleSystem.getCurrentTurnEntity();
        if (!currentEntity)
            break;

        // Пропускаем мертвых сущностей
        if (currentEntity->getCurrentHealthPoint() <= 0)
        {
            battleSystem.nextTurn();
            continue;
        }

        // Определяем, игрок это или ИИ
        bool isPlayerEntity = false;
        for (Player *player : playerParty)
        {
            if (player == currentEntity)
            {
                isPlayerEntity = true;
                break;
            }
        }

        // Ход персонажа - повторяем пока есть стамина
        while (currentEntity->getCurrentStamina() > 0 && battleSystem.isBattleActive())
        {
            if (isPlayerEntity)
            {
                // Player turn (simplified version)
                cout << "\n=== TURN " << currentEntity->getName() << " ===\n";
                cout << "Stamina left: " << currentEntity->getCurrentStamina() << "/" << currentEntity->getMaxStamina() << "\n";
                // Show available actions
                cout << "Choose action:\n";
                cout << "1. Attack\n";
                cout << "2. Use ability\n";
                cout << "3. Skip turn\n";
                int actionChoice = getSafeIntInput("Choose (1-3): ", 1, 3);

                if (actionChoice == 1)
                {
                    // Attack
                    vector<pair<Entity *, int>> targets = battleSystem.getAvailableTargetsForCurrent();
                    if (!targets.empty())
                    {
                        cout << "Available targets:\n";
                        for (int i = 0; i < targets.size(); ++i)
                        {
                            cout << i + 1 << ". " << targets[i].first->getName() << " (HP: " << targets[i].first->getCurrentHealthPoint() << ", Pos: " << targets[i].second << ")\n";
                        }
                        int targetChoice;
                        cout << "Choose target (1-" << targets.size() << "): ";
                        cin >> targetChoice;
                        cin.ignore();
                        if (targetChoice > 0 && targetChoice <= static_cast<int>(targets.size()))
                        {
                            battleSystem.attack(currentEntity, targets[targetChoice - 1].first);
                        }
                        else
                        {
                            cout << "Invalid target choice!\n";
                            continue;
                        }
                    }
                    else
                    {
                        cout << "No available targets for attack!\n";
                        continue;
                    }
                }
                else if (actionChoice == 2)
                {
                    // Использование способности
                    Player *player = static_cast<Player *>(currentEntity);
                    const vector<AbilityType> &abilities = player->getAvailableAbilities();

                    if (!abilities.empty())
                    {
                        cout << "Доступные способности:\n";
                        for (int i = 0; i < abilities.size(); ++i)
                        {
                            const AbilityInfo &info = HeroFactory::getAbilityInfo(abilities[i]);
                            cout << i + 1 << ". " << info.name << " - " << info.description << "\n";
                            cout << "   Эффект: " << info.effect << "\n";
                        }

                        int abilityChoice;
                        cout << "Выберите способность (1-" << abilities.size() << "): ";
                        cin >> abilityChoice;
                        cin.ignore();

                        if (abilityChoice > 0 && abilityChoice <= static_cast<int>(abilities.size()))
                        {
                            AbilityType chosenAbility = abilities[abilityChoice - 1];
                            if (!battleSystem.useAbility(currentEntity, chosenAbility))
                            {
                                cout << "Не удалось использовать способность!\n";
                            }
                            else
                            {
                                // Показываем активные эффекты после использования способности
                                const vector<Effect> &effects = currentEntity->getActiveEffects();
                                if (!effects.empty())
                                {
                                    cout << "\nАктивные эффекты " << currentEntity->getName() << ":\n";
                                    for (const auto &effect : effects)
                                    {
                                        cout << "- " << effect.name << " (" << effect.duration << " ходов";
                                        if (effect.value != 0)
                                        {
                                            string sign = (effect.value > 0) ? "+" : "";
                                            cout << ", " << sign << effect.value;
                                        }
                                        cout << ")\n";
                                    }
                                }
                            }
                        }
                        else
                        {
                            cout << "Неверный выбор способности!\n";
                            continue;
                        }
                    }
                    else
                    {
                        cout << "Нет доступных способностей!\n";
                        continue;
                    }
                }
                else if (actionChoice == 3)
                {
                    // Skip turn
                    cout << currentEntity->getName() << " skips turn.\n";
                    currentEntity->setCurrentStamina(0); // End turn
                }
                else
                {
                    cout << "Invalid action choice!\n";
                    continue;
                }
            }
            else
            {
                // AI turn (random attack)
                vector<pair<Entity *, int>> targets = battleSystem.getAvailableTargetsForCurrent();
                if (!targets.empty())
                {
                    int targetIndex = rand() % targets.size();
                    battleSystem.attack(currentEntity, targets[targetIndex].first);
                }
                else
                {
                    // No targets, skip turn
                    break;
                }
            }

            // Проверяем, не закончен ли бой после атаки
            if (!battleSystem.isBattleActive())
            {
                break;
            }

            cout << "\nНажмите Enter для продолжения...";
            cin.get();
        }

        // Проверяем, закончен ли бой
        if (battleSystem.isPlayerVictory())
        {
            playerWon = true;
            cout << "[DEBUG] endBattle called from handleBattleCampaignEvent victory/defeat check\n";
            battleSystem.endBattle();
            break;
        }
        else if (battleSystem.isPlayerDefeat())
        {
            playerWon = false;
            cout << "[DEBUG] endBattle called from handleBattleCampaignEvent victory/defeat check\n";
            battleSystem.endBattle();
            break;
        }

        // Переходим к следующему персонажу
        battleSystem.nextTurn();
    }

    // Собираем мертвых игроков для удаления после завершения боя
    vector<Player *> deadPlayers;

    // Проверяем результат боя
    if (playerWon)
    {
        cout << "\n[PARTY] VICTORY! You have successfully defeated the enemies!\n";

        for (auto it = playerParty.begin(); it != playerParty.end();)
        {
            if (*it && (*it)->getCurrentHealthPoint() <= 0)
            {
                cout << (*it)->getName() << " пал в бою и покидает отряд!\n";
                deadPlayers.push_back(*it);
                it = playerParty.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Начисляем опыт всем оставшимся игрокам
        int totalExp = 0;
        for (Entity *enemy : enemies)
        {
            Enemy *e = static_cast<Enemy *>(enemy);
            totalExp += e->getExperienceValue();
        }

        int expPerPlayer = static_cast<int>(totalExp) / static_cast<int>(playerParty.size());
        for (Player *player : playerParty)
        {
            if (player->getCurrentHealthPoint() > 0)
            {
                player->setReceivedExperience(player->getReceivedExperience() + expPerPlayer);
                player->upLevel();
                cout << player->getName() << " получает " << expPerPlayer << " опыта!\n";
            }
        }

        cout << "\nPress Enter to return to map mode...";
        cin.get();
    }
    else
    {
        cout << "\n[SKULL] DEFEAT! Your party has fallen in battle!\n";

        // Удаляем мертвых игроков из отряда
        for (auto it = playerParty.begin(); it != playerParty.end();)
        {
            if (*it && (*it)->getCurrentHealthPoint() <= 0)
            {
                cout << (*it)->getName() << " пал в бою и покидает отряд!\n";
                deadPlayers.push_back(*it);
                it = playerParty.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // Удаляем мертвых игроков после завершения боя
    for (Player *deadPlayer : deadPlayers)
    {
        delete deadPlayer;
    }

    // Удаляем врагов после боя
    for (Entity *enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
    cout << "[DEBUG] Enemies deleted and vector cleared\n";
}

void CampaignSystem::handleTreasureEvent(const CampaignEvent &event)
{
    cout << "\n=== TREASURE ===\n";
    cout << event.description << "\n";

    if (event.reward)
    {
        cout << "You found: " << event.reward->getName() << "\n";
        // Добавить предмет в общий инвентарь отряда
        partyInventory.push_back(*event.reward);
        cout << "[DEBUG] Added item to partyInventory, size: " << partyInventory.size() << "\n";
        cout << "Item added to party inventory.\n";
        delete event.reward;
    }
    else
    {
        cout << "Unfortunately, the treasure was empty.\n";
    }

    // Предложить управление инвентарем
    if (!playerParty.empty())
    {
        manageInventory();
    }

    cout << "\nPress Enter to continue...";
    cin.get();
}

void CampaignSystem::handleTextEvent(const CampaignEvent &event)
{
    cout << "\n=== CampaignEvent ===\n";
    cout << event.description << "\n";

    if (!event.choices.empty())
    {
        cout << "Action options:\n";
        for (int i = 0; i < event.choices.size(); ++i)
        {
            cout << i + 1 << ". " << event.choices[i] << "\n";
        }

        int choice = getSafeIntInput("Choose action (1-" + to_string(event.choices.size()) + "): ", 1, static_cast<int>(event.choices.size()));

        if (choice > 0 && choice <= static_cast<int>(event.choices.size()))
        {
            EventType outcome = event.outcomes[choice - 1];

            // Обрабатываем результат выбора
            switch (outcome)
            {
            case EventType::BATTLE:
                handleBattleEvent(CampaignEvent{EventType::BATTLE, "Your choice led to a battle!", {}, {}, nullptr, currentDifficulty});
                break;
            case EventType::TREASURE:
            {
                Item randomAccessory = ItemFactory::createRandomItemOfType(ItemType::ACCESSORY);
                handleTreasureEvent(CampaignEvent{EventType::TREASURE, "Your choice brought a reward!", {}, {}, new Item(randomAccessory), 0});
                break;
            }
            default:
                cout << "Your choice did not bring noticeable results.\n";
                break;
            }
        }
    }

    cout << "\nPress Enter to continue...";
    cin.get();
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
    cout << "\n=== FINAL BATTLE ===\n";
    cout << "You have encountered the LORD OF DARKNESS - the final boss of the game!\n";

    // Create the final boss
    vector<Entity *> bossParty;
    Enemy *finalBoss = new Enemy("Lord of Darkness", 300, 25, 10, 8, 3, 3, 15, 2, AbilityType::LIFE_STEAL, 200, 5, "final_boss", 0.1);
    bossParty.push_back(finalBoss);

    // Add a couple of minions (not from the castle)
    Enemy *minion1 = EnemyFactory::createEnemyByName("Vampire", 2);
    Enemy *minion2 = EnemyFactory::createEnemyByName("Troglodyte", 2);
    bossParty.push_back(minion1);
    bossParty.push_back(minion2);

    BattleSystem battleSystem;
    // Конвертируем Player* в Entity*
    vector<Entity *> playerEntities;
    for (Player *player : playerParty)
    {
        playerEntities.push_back(player);
    }
    battleSystem.startBattle(playerEntities, bossParty);

    // Бой с боссом
    while (battleSystem.isBattleActive())
    {
        cout << "\n"
             << battleSystem.getBattleStatus() << "\n";
        battleSystem.printBattlefield();

        Entity *currentEntity = battleSystem.getCurrentTurnEntity();
        if (!currentEntity)
            break;

        // Определяем, игрок это или ИИ
        bool isPlayerEntity = false;
        for (Player *player : playerParty)
        {
            if (player == currentEntity)
            {
                isPlayerEntity = true;
                break;
            }
        }

        // Ход персонажа - повторяем пока есть стамина
        while (currentEntity->getCurrentStamina() > 0 && battleSystem.isBattleActive())
        {
            if (isPlayerEntity)
            {
                // Ход игрока
                cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";
                cout << "Осталось стамины: " << currentEntity->getCurrentStamina() << "/" << currentEntity->getMaxStamina() << "\n";

                // Показываем доступные действия
                cout << "Выберите действие:\n";
                cout << "1. Атаковать\n";
                cout << "2. Использовать способность\n";
                cout << "3. Пропустить ход\n";

                int actionChoice = getSafeIntInput("Выберите (1-3): ", 1, 3);

                if (actionChoice == 1)
                {
                    // Атака
                    vector<pair<Entity *, int>> targets = battleSystem.getAvailableTargetsForCurrent();

                    if (!targets.empty())
                    {
                        cout << "Доступные цели:\n";
                        for (int i = 0; i < targets.size(); ++i)
                        {
                            cout << i + 1 << ". " << targets[i].first->getName() << " (HP: " << targets[i].first->getCurrentHealthPoint() << ")\n";
                        }

                        int targetChoice = getSafeIntInput("Выберите цель (1-" + to_string(targets.size()) + "): ", 1, static_cast<int>(targets.size()));

                        if (targetChoice > 0 && targetChoice <= static_cast<int>(targets.size()))
                        {
                            battleSystem.attack(currentEntity, targets[targetChoice - 1].first);
                        }
                        else
                        {
                            cout << "Неверный выбор цели!\n";
                            continue;
                        }
                    }
                    else
                    {
                        cout << "Нет доступных целей для атаки!\n";
                        continue;
                    }
                }
                else if (actionChoice == 2)
                {
                    // Использование способности
                    Player *player = static_cast<Player *>(currentEntity);
                    const vector<AbilityType> &abilities = player->getAvailableAbilities();

                    if (!abilities.empty())
                    {
                        cout << "Доступные способности:\n";
                        for (int i = 0; i < abilities.size(); ++i)
                        {
                            const AbilityInfo &info = HeroFactory::getAbilityInfo(abilities[i]);
                            cout << i + 1 << ". " << info.name << " - " << info.description << "\n";
                            cout << "   Эффект: " << info.effect << "\n";
                        }

                        int abilityChoice = getSafeIntInput("Выберите способность (1-" + to_string(abilities.size()) + "): ", 1, static_cast<int>(abilities.size()));

                        if (abilityChoice > 0 && abilityChoice <= static_cast<int>(abilities.size()))
                        {
                            AbilityType chosenAbility = abilities[abilityChoice - 1];
                            if (!battleSystem.useAbility(currentEntity, chosenAbility))
                            {
                                cout << "Не удалось использовать способность!\n";
                            }
                            else
                            {
                                // Показываем активные эффекты после использования способности
                                const vector<Effect> &effects = currentEntity->getActiveEffects();
                                if (!effects.empty())
                                {
                                    cout << "\nАктивные эффекты " << currentEntity->getName() << ":\n";
                                    for (const auto &effect : effects)
                                    {
                                        cout << "- " << effect.name << " (" << effect.duration << " ходов";
                                        if (effect.value != 0)
                                        {
                                            string sign = (effect.value > 0) ? "+" : "";
                                            string statName;
                                            switch (effect.type)
                                            {
                                            case EffectType::BUFF_DAMAGE:
                                                statName = "урон";
                                                break;
                                            case EffectType::BUFF_DEFENSE:
                                                statName = "защита";
                                                break;
                                            case EffectType::BUFF_INITIATIVE:
                                                statName = "инициатива";
                                                break;
                                            case EffectType::DEBUFF_DAMAGE:
                                                statName = "урон";
                                                break;
                                            case EffectType::DEBUFF_DEFENSE:
                                                statName = "защита";
                                                break;
                                            case EffectType::DEBUFF_INITIATIVE:
                                                statName = "инициатива";
                                                break;
                                            case EffectType::POISON_DAMAGE:
                                                statName = "урон от яда";
                                                break;
                                            case EffectType::REGENERATION:
                                                statName = "регенерация";
                                                break;
                                            default:
                                                statName = "стат";
                                                break;
                                            }
                                            cout << ", " << statName << " " << sign << effect.value;
                                        }
                                        cout << ")\n";
                                    }
                                }
                            }
                        }
                        else
                        {
                            cout << "Неверный выбор способности!\n";
                            continue;
                        }
                    }
                    else
                    {
                        cout << "Нет доступных способностей!\n";
                        continue;
                    }
                }
                else if (actionChoice == 3)
                {
                    // Пропустить ход
                    cout << currentEntity->getName() << " пропускает ход.\n";
                    currentEntity->setCurrentStamina(0); // Завершить ход
                }
                else
                {
                    cout << "Неверный выбор действия!\n";
                    continue;
                }
            }
            else
            {
                // Ход ИИ (случайная атака)
                vector<pair<Entity *, int>> targets = battleSystem.getAvailableTargetsForCurrent();
                if (!targets.empty())
                {
                    int targetIndex = rand() % targets.size();
                    battleSystem.attack(currentEntity, targets[targetIndex].first);
                    cout << currentEntity->getName() << " атакует " << targets[targetIndex].first->getName() << "!\n";
                }
                else
                {
                    // Нет целей, пропускаем ход
                    break;
                }
            }

            // Проверяем, не закончен ли бой после атаки
            if (!battleSystem.isBattleActive())
            {
                break;
            }

            cout << "\nНажмите Enter для продолжения...";
            cin.get();
        }

        // Проверяем, закончен ли бой
        if (battleSystem.isPlayerVictory() || battleSystem.isPlayerDefeat())
        {
            battleSystem.endBattle();
            break;
        }

        // Переходим к следующему персонажу
        battleSystem.nextTurn();
    }

    if (battleSystem.isPlayerVictory())
    {
        cout << "\n[VICTORY] ПОБЕДА! Вы победили Властелина Тьмы и спасли мир!\n";

        // Удаляем мертвых игроков из отряда
        for (auto it = playerParty.begin(); it != playerParty.end();)
        {
            if (*it && (*it)->getCurrentHealthPoint() <= 0)
            {
                cout << (*it)->getName() << " пал в бою и покидает отряд!\n";
                delete *it;
                it = playerParty.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Начисляем опыт всем оставшимся игрокам
        int totalExp = 0;
        for (Entity *enemy : bossParty)
        {
            Enemy *e = static_cast<Enemy *>(enemy);
            totalExp += e->getExperienceValue();
        }

        int expPerPlayer = static_cast<int>(totalExp) / static_cast<int>(playerParty.size());
        for (Player *player : playerParty)
        {
            if (player->getCurrentHealthPoint() > 0)
            {
                player->setReceivedExperience(player->getReceivedExperience() + expPerPlayer);
                player->upLevel();
                cout << player->getName() << " получает " << expPerPlayer << " опыта!\n";
            }
        }

        gameCompleted = true;
    }
    else
    {
        cout << "\n[DEFEAT] Поражение... Властелин Тьмы победил.\n";
    }

    battleSystem.endBattle();

    // Освобождаем память
    for (Entity *enemy : bossParty)
    {
        delete enemy;
    }
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
        cout << "\nДоступные переходы:\n";
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
        cout << "Вы переместились в " << currentLocation.name << ".\n";
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
            cout << "\n[SKULL] Ваш отряд пал в бою! Игра окончена.\n";
            cout << "[DEBUG] Breaking from runMapMode loop\n";
            break;
        }
    }
}

char CampaignSystem::getPlayerMovementInput()
{
    cout << "Введите направление движения (w/a/s/d): ";
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
        cout << "Неверный ввод. Используйте w/a/s/d: ";
    }
}

void CampaignSystem::displayMap()
{
    cout << "+================================================================+\n";
    cout << "|                           MAP                                  |\n";
    cout << "+================================================================+\n";
    cout << "\n";

    gameMap.printMap();

    cout << "\nУсловные обозначения:\n";
    cout << "@ - Вы (игрок)\n";
    cout << "I - Вход (старт)\n";
    cout << "B - Бой\n";
    cout << "S - Событие\n";
    cout << "P - Предмет (сокровище)\n";
    cout << "V - Выход\n";
    cout << ". - Пустая клетка\n";
    cout << "? - Непосещенная клетка в радиусе видимости\n";
    cout << "  - Клетка вне радиуса видимости\n";
}

void CampaignSystem::handleNodeEvent(NodeType nodeType)
{
    switch (nodeType)
    {
    case NodeType::BATTLE:
    {
        if (currentLocation.type == LocationType::CASTLE)
        {
            CampaignEvent bossEvent{EventType::BOSS_BATTLE, "Вы столкнулись с финальным боссом!"};
            handleBossBattleEvent(bossEvent);
        }
        else
        {
            CampaignEvent battleEvent{EventType::BATTLE, "Вы наткнулись на группу врагов!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        break;
    }
    case NodeType::EVENT:
    {
        CampaignEvent eventEvent{EventType::TEXT_EVENT, "Вы обнаружили странное явление...", {"Исследовать", "Игнорировать"}, {EventType::TREASURE, EventType::BATTLE}, nullptr, 0};
        handleTextEvent(eventEvent);
        break;
    }
    case NodeType::TREASURE:
    {
        Item randomItem = ItemFactory::createRandomItem();
        CampaignEvent treasureEvent{EventType::TREASURE, "Вы нашли скрытое сокровище!", {}, {}, new Item(randomItem), 0};
        handleTreasureEvent(treasureEvent);
        break;
    }
    case NodeType::EXIT:
    {
        CampaignEvent exitEvent{EventType::EXIT, "Вы нашли выход из этой локации."};
        handleExitEvent(exitEvent);
        break;
    }
    case NodeType::FOREST:
    {
        // Точка интереса: Лес - случайное событие
        int randEvent = rand() % 3;
        if (randEvent == 0)
        {
            CampaignEvent battleEvent{EventType::BATTLE, "В лесу вы наткнулись на диких зверей!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else if (randEvent == 1)
        {
            Item randomItem = ItemFactory::createRandomItem();
            CampaignEvent treasureEvent{EventType::TREASURE, "Вы нашли древний артефакт в лесу!", {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        else
        {
            CampaignEvent eventEvent{EventType::TEXT_EVENT, "Вы услышали шепот деревьев...", {"Прислушаться", "Игнорировать"}, {EventType::TREASURE, EventType::BATTLE}, nullptr, 0};
            handleTextEvent(eventEvent);
        }
        break;
    }
    case NodeType::CAVE:
    {
        // Точка интереса: Пещера - чаще бой
        int randEvent = rand() % 3;
        if (randEvent < 2)
        {
            CampaignEvent battleEvent{EventType::BATTLE, "Из пещеры выскочили монстры!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else
        {
            Item randomItem = ItemFactory::createRandomItem();
            CampaignEvent treasureEvent{EventType::TREASURE, "В пещере вы нашли кристалл!", {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        break;
    }
    case NodeType::DEAD_CITY:
    {
        // Точка интереса: Мертвый город - чаще событие
        int randEvent = rand() % 3;
        if (randEvent == 0)
        {
            CampaignEvent battleEvent{EventType::BATTLE, "Призраки города атакуют!", {}, {}, nullptr, currentDifficulty};
            handleBattleEvent(battleEvent);
        }
        else if (randEvent == 1)
        {
            Item randomItem = ItemFactory::createRandomItem();
            CampaignEvent treasureEvent{EventType::TREASURE, "Вы нашли забытые реликвии!", {}, {}, new Item(randomItem), 0};
            handleTreasureEvent(treasureEvent);
        }
        else
        {
            CampaignEvent eventEvent{EventType::TEXT_EVENT, "Город шепчет свои секреты...", {"Исследовать", "Уйти"}, {EventType::TREASURE, EventType::BATTLE}, nullptr, 0};
            handleTextEvent(eventEvent);
        }
        break;
    }
    case NodeType::CASTLE:
    {
        // Точка интереса: Замок - финальный босс
        CampaignEvent bossEvent{EventType::BOSS_BATTLE, "Вы проникли в сердце замка и столкнулись с Властелином Тьмы!"};
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
        // Выбор персонажа для управления инвентарем
        cout << "\n=== ВЫБОР ПЕРСОНАЖА ===\n";
        cout << "Выберите персонажа для управления инвентарем:\n";
        for (int i = 0; i < playerParty.size(); ++i)
        {
            cout << i + 1 << ". " << playerParty[i]->getName() << " (HP: " << playerParty[i]->getCurrentHealthPoint() << "/" << playerParty[i]->getMaxHealthPoint() << ")\n";
        }
        cout << playerParty.size() + 1 << ". Выйти из управления инвентарем\n";

        int heroChoice = getSafeIntInput("Выберите (1-" + to_string(playerParty.size() + 1) + "): ", 1, static_cast<int>(playerParty.size() + 1));

        if (heroChoice > 0 && heroChoice <= static_cast<int>(playerParty.size()))
        {
            player = playerParty[heroChoice - 1];
        }
        else if (heroChoice == playerParty.size() + 1)
        {
            // Выход
            return;
        }
        else
        {
            cout << "Неверный выбор!\n";
            return;
        }
    }

    while (true)
    {
        cout << "\n=== УПРАВЛЕНИЕ ИНВЕНТАРЕМ ===\n";
        cout << "Герой: " << player->getName() << "\n\n";
        cout << "[DEBUG] manageInventory called, partyInventory size: " << partyInventory.size() << ", player inventory size: " << player->getInventory().size() << "\n";

        player->displayInventory();
        cout << "\n";
        player->displayEquipment();

        cout << "\nВыберите действие:\n";
        cout << "1. Надеть предмет\n";
        cout << "2. Снять предмет\n";
        cout << "3. Использовать расходуемый предмет\n";
        cout << "4. Взять предмет из общего инвентаря\n";
        cout << "5. Выйти из управления инвентарем\n";

        int choice = getSafeIntInput("Выберите (1-5): ", 1, 5);

        if (choice == 1)
        {
            // Надеть предмет
            if (player->getInventory().empty())
            {
                cout << "Инвентарь пуст!\n";
                continue;
            }

            cout << "Выберите предмет для надевания:\n";
            for (int i = 0; i < player->getInventory().size(); ++i)
            {
                cout << i + 1 << ". " << player->getInventory()[i].getName() << "\n";
            }

            int itemChoice = getSafeIntInput("Выберите предмет (1-" + to_string(player->getInventory().size()) + "): ", 1, static_cast<int>(player->getInventory().size()));

            if (itemChoice > 0 && itemChoice <= static_cast<int>(player->getInventory().size()))
            {
                player->equipItem(itemChoice - 1);
            }
            else
            {
                cout << "Неверный выбор!\n";
            }
        }
        else if (choice == 2)
        {
            // Снять предмет
            cout << "Выберите слот для снятия:\n";
            cout << "1. Голова\n";
            cout << "2. Грудь\n";
            cout << "3. Руки\n";
            cout << "4. Ноги\n";
            cout << "5. Ступни\n";
            cout << "6. Правая рука\n";
            cout << "7. Левая рука\n";
            cout << "8. Шея\n";
            cout << "9. Кольцо 1\n";
            cout << "10. Кольцо 2\n";

            int slotChoice = getSafeIntInput("Выберите слот (1-10): ", 1, 10);

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
                cout << "Неверный выбор!\n";
                continue;
            }

            player->unequipItem(slot);
        }
        else if (choice == 3)
        {
            // Использовать расходуемый предмет
            if (player->getInventory().empty())
            {
                cout << "Инвентарь пуст!\n";
                continue;
            }

            cout << "Выберите предмет для использования:\n";
            for (int i = 0; i < player->getInventory().size(); ++i)
            {
                cout << i + 1 << ". " << player->getInventory()[i].getName() << "\n";
            }

            int itemChoice = getSafeIntInput("Выберите предмет (1-" + to_string(player->getInventory().size()) + "): ", 1, static_cast<int>(player->getInventory().size()));

            if (itemChoice > 0 && itemChoice <= static_cast<int>(player->getInventory().size()))
            {
                player->useConsumable(itemChoice - 1);
            }
            else
            {
                cout << "Неверный выбор!\n";
            }
        }
        else if (choice == 4)
        {
            // Взять предмет из общего инвентаря
            if (partyInventory.empty())
            {
                cout << "Общий инвентарь пуст!\n";
                continue;
            }

            cout << "Общий инвентарь отряда:\n";
            for (int i = 0; i < partyInventory.size(); ++i)
            {
                cout << i + 1 << ". " << partyInventory[i].getName() << "\n";
            }

            int itemChoice = getSafeIntInput("Выберите предмет для взятия (1-" + to_string(partyInventory.size()) + "): ", 1, static_cast<int>(partyInventory.size()));

            if (itemChoice > 0 && itemChoice <= static_cast<int>(partyInventory.size()))
            {
                player->addItem(partyInventory[itemChoice - 1]);
                partyInventory.erase(partyInventory.begin() + itemChoice - 1);
                cout << "Предмет взят в личный инвентарь.\n";
            }
            else
            {
                cout << "Неверный выбор!\n";
            }
        }
        else if (choice == 5)
        {
            // Выход
            break;
        }
        else
        {
            cout << "Неверный выбор!\n";
        }

        cout << "\nНажмите Enter для продолжения...";
        cin.get();
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
