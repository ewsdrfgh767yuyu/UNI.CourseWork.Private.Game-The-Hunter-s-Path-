#include "CampaignSystem.h"
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
    cleanupParty();
}

void CampaignSystem::initializeLocations()
{
    // ЛЕС
    Location forest;
    forest.type = LocationType::FOREST;
    forest.name = "Лес";
    forest.description = "Густой лес, полный опасных существ и древних тайн.";
    forest.connections = {LocationType::CAVE};
    forest.isFinalBossLocation = false;

    // ПЕЩЕРЫ
    Location cave;
    cave.type = LocationType::CAVE;
    cave.name = "Пещеры";
    cave.description = "Глубокие подземные пещеры, где обитают троглодиты и элементали.";
    cave.connections = {LocationType::FOREST, LocationType::DEAD_CITY};
    cave.isFinalBossLocation = false;

    // ГОРОД МЕРТВЫХ
    Location deadCity;
    deadCity.type = LocationType::DEAD_CITY;
    deadCity.name = "Город мертвых";
    deadCity.description = "Заброшенный город, населенный нежитью и призраками.";
    deadCity.connections = {LocationType::CAVE, LocationType::CASTLE};
    deadCity.isFinalBossLocation = false;

    // ЗАМОК
    Location castle;
    castle.type = LocationType::CASTLE;
    castle.name = "Замок";
    castle.description = "Древний замок, где обитает финальный босс и его приспешники.";
    castle.connections = {LocationType::DEAD_CITY};
    castle.isFinalBossLocation = true;

    locations[LocationType::FOREST] = forest;
    locations[LocationType::CAVE] = cave;
    locations[LocationType::DEAD_CITY] = deadCity;
    locations[LocationType::CASTLE] = castle;

    // Начинаем с леса
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

    // Генерируем карту
    gameMap.generate();
    // Выводим карту сразу после генерации для отладки
    cout << "Карта после генерации:\n";
    gameMap.printMap();
    cout << "\nНажмите Enter для продолжения...";
    cin.get();

    // Запускаем режим карты
    runMapMode();
}

void CampaignSystem::createPlayerParty()
{
    cout << "\n=== СОЗДАНИЕ ОТРЯДА ===\n";

    // Выбор размера отряда
    vector<string> partySizes = {"Одиночка (1 герой)", "Малый отряд (2 героя)", "Полный отряд (3 героя)"};
    cout << "Выберите размер отряда:\n";
    for (size_t i = 0; i < partySizes.size(); ++i)
    {
        cout << i + 1 << ". " << partySizes[i] << "\n";
    }

    int partySizeChoice;
    cout << "Выберите (1-" << partySizes.size() << "): ";
    cin >> partySizeChoice;
    cin.ignore();

    int maxHeroes = partySizeChoice; // 1, 2 или 3 героя

    // Получаем доступные классы
    vector<HeroClass> availableClasses = HeroFactory::getAvailableClasses();

    for (int i = 0; i < maxHeroes; ++i)
    {
        cout << "\n--- Выбор героя " << (i + 1) << " ---\n";

        // Показываем доступные классы
        vector<string> classNames;
        for (HeroClass cls : availableClasses)
        {
            const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(cls);
            classNames.push_back(tmpl.name + " - " + tmpl.description);
        }

        cout << "Выберите класс героя:\n";
        for (size_t j = 0; j < classNames.size(); ++j)
        {
            cout << j + 1 << ". " << classNames[j] << "\n";
        }

        int classChoice;
        cout << "Выберите (1-" << classNames.size() << "): ";
        cin >> classChoice;
        cin.ignore();

        HeroClass selectedClass = availableClasses[classChoice - 1];
        const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(selectedClass);

        // Если выбран одиночка, создаем только его
        if (tmpl.isLoner)
        {
            cout << "Вы выбрали Одиночку! Отряд будет состоять только из него.\n";
            Player *loner = HeroFactory::createHero(selectedClass, "Одиночка");
            playerParty.push_back(loner);
            break;
        }

        // Ввод имени героя
        string heroName;
        cout << "Введите имя героя: ";
        getline(cin, heroName);
        if (heroName.empty())
        {
            heroName = tmpl.name;
        }

        Player *hero = HeroFactory::createHero(selectedClass, heroName);
        playerParty.push_back(hero);

        cout << "Создан " << hero->getName() << " (" << tmpl.name << ")\n";
    }
}

void CampaignSystem::runCampaignLoop()
{
    while (!gameCompleted && !playerParty.empty())
    {
        // Очистка консоли перед каждым новым сообщением
        system("cls");

        displayLocationInfo();

        // Генерируем случайное событие (предотвращаем бой на первом событии)
        Event currentEvent = generateRandomEvent();
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

Event CampaignSystem::generateRandomEvent()
{
    Event event;

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
        // TODO: Добавить реальные предметы
        event.reward = new Item("Золотая монета", "Ценная монета", ItemType::CONSUMABLE, EquipmentSlot::NONE, {{"health_restore", 10}});
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

void CampaignSystem::handleBattleEvent(const Event &event)
{
    cout << "\n=== БОЙ ===\n";
    cout << event.description << "\n";

    // Создаем врагов для текущей локации
    vector<Entity *> enemies;
    int enemyCount = 1 + (rand() % 3); // 1-3 врага

    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy *enemy = EnemyFactory::createRandomEnemy(currentLocation.type, event.difficultyModifier);
        enemies.push_back(enemy);
        cout << "Появился " << enemy->getName() << "!\n";
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
                // Ход игрока (упрощенная версия)
                cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";
                cout << "Осталось стамины: " << currentEntity->getCurrentStamina() << "/" << currentEntity->getMaxStamina() << "\n";
                vector<Entity *> targets = battleSystem.getAvailableTargetsForCurrent();

                if (!targets.empty())
                {
                    cout << "Доступные цели:\n";
                    for (size_t i = 0; i < targets.size(); ++i)
                    {
                        cout << i + 1 << ". " << targets[i]->getName() << " (HP: " << targets[i]->getCurrentHealthPoint() << ")\n";
                    }

                    int targetChoice;
                    cout << "Выберите цель (1-" << targets.size() << "): ";
                    cin >> targetChoice;
                    cin.ignore();

                    if (targetChoice > 0 && targetChoice <= static_cast<int>(targets.size()))
                    {
                        battleSystem.attack(currentEntity, targets[targetChoice - 1]);
                    }
                    else
                    {
                        // Если выбор некорректный, пропускаем ход
                        break;
                    }
                }
                else
                {
                    // Нет целей, пропускаем ход
                    break;
                }
            }
            else
            {
                // Ход ИИ (случайная атака)
                vector<Entity *> targets = battleSystem.getAvailableTargetsForCurrent();
                if (!targets.empty())
                {
                    int targetIndex = rand() % targets.size();
                    battleSystem.attack(currentEntity, targets[targetIndex]);
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

        // Переходим к следующему персонажу
        battleSystem.nextTurn();
    }

    // Проверяем результат боя
    if (battleSystem.isPlayerVictory())
    {
        cout << "\n[PARTY] ПОБЕДА! Вы успешно победили врагов!\n";

        // Начисляем опыт всем живым игрокам
        int totalExp = 0;
        for (Entity* enemy : enemies) {
            Enemy* e = static_cast<Enemy*>(enemy);
            totalExp += e->getExperienceValue();
        }

        int expPerPlayer = static_cast<int>(totalExp) / static_cast<int>(playerParty.size());
        for (Player* player : playerParty) {
            if (player->getCurrentHealthPoint() > 0) {
                player->setReceivedExperience(player->getReceivedExperience() + expPerPlayer);
                player->upLevel();
                cout << player->getName() << " получает " << expPerPlayer << " опыта!\n";
            }
        }

        cout << "\nНажмите Enter для выхода в режим карты...";
        cin.get();
    }
    else if (battleSystem.isPlayerDefeat())
    {
        cout << "\n[SKULL] ПОРАЖЕНИЕ! Ваш отряд пал в бою!\n";
    }

    battleSystem.endBattle();

    // Освобождаем память врагов
    for (Entity *enemy : enemies)
    {
        delete enemy;
    }
}

void CampaignSystem::handleTreasureEvent(const Event &event)
{
    cout << "\n=== НАХОДКА ===\n";
    cout << event.description << "\n";

    if (event.reward)
    {
        cout << "Вы нашли: " << event.reward->getName() << "\n";
        // Добавить предмет в инвентарь первого героя
        if (!playerParty.empty())
        {
            playerParty[0]->addItem(*event.reward);
            cout << "Предмет добавлен в инвентарь " << playerParty[0]->getName() << ".\n";
        }
        delete event.reward;
    }
    else
    {
        cout << "К сожалению, сокровище оказалось пустым.\n";
    }

    // Предложить управление инвентарем
    if (!playerParty.empty())
    {
        manageInventory();
    }

    cout << "\nНажмите Enter для продолжения...";
    cin.get();
}

void CampaignSystem::handleTextEvent(const Event &event)
{
    cout << "\n=== СОБЫТИЕ ===\n";
    cout << event.description << "\n";

    if (!event.choices.empty())
    {
        cout << "Варианты действий:\n";
        for (size_t i = 0; i < event.choices.size(); ++i)
        {
            cout << i + 1 << ". " << event.choices[i] << "\n";
        }

        int choice;
        cout << "Выберите действие (1-" << event.choices.size() << "): ";
        cin >> choice;
        cin.ignore();

        if (choice > 0 && choice <= static_cast<int>(event.choices.size()))
        {
            EventType outcome = event.outcomes[choice - 1];

            // Обрабатываем результат выбора
            switch (outcome)
            {
            case EventType::BATTLE:
                handleBattleEvent(Event{EventType::BATTLE, "Ваш выбор привел к бою!", {}, {}, nullptr, currentDifficulty});
                break;
            case EventType::TREASURE:
                handleTreasureEvent(Event{EventType::TREASURE, "Ваш выбор принес награду!", {}, {}, new Item("Магический артефакт", "Древний артефакт", ItemType::ACCESSORY, EquipmentSlot::NECK, {{"attack", 2}}), 0});
                break;
            default:
                cout << "Ваш выбор не принес заметных результатов.\n";
                break;
            }
        }
    }

    cout << "\nНажмите Enter для продолжения...";
    cin.get();
}

void CampaignSystem::handleExitEvent(const Event &event)
{
    cout << "\n=== ВЫХОД ===\n";
    cout << event.description << "\n";

    // Для карты просто уведомляем о переходе
    cout << "Вы переходите в следующую локацию...\n";
    cout << "Название новой локации: ";

    // Генерируем случайное название для новой локации
    vector<string> locationNames = {
        "Темный Лес", "Подземные Пещеры", "Заброшенный Город",
        "Древний Замок", "Морские Берега", "Горные Вершины",
        "Волшебный Лес", "Ледяные Пустоши", "Пылающая Пустыня"};

    string newLocationName = locationNames[rand() % locationNames.size()];
    cout << newLocationName << "\n";

    cout << "\nВы согласны перейти? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();

    if (choice == 'y' || choice == 'Y')
    {
        cout << "Вы переходите в " << newLocationName << "...\n";
        // Генерируем новую карту для новой локации
        gameMap.generate();
        visitedNodes.clear();
        currentDifficulty++;
    }
    else
    {
        cout << "Вы остаетесь на месте.\n";
        // Возвращаем игрока на предыдущую клетку (перед выходом)
        // Для простоты просто не делаем ничего, игрок остается на выходе
    }

    cout << "\nНажмите Enter для продолжения...";
    cin.get();
}

void CampaignSystem::handleBossBattleEvent(const Event &event)
{
    cout << "\n=== ФИНАЛЬНЫЙ БОЙ ===\n";
    cout << "Вы столкнулись с ВЛАСТЕЛИНОМ ТЬМЫ - финальным боссом игры!\n";

    // Создаем финального босса
    vector<Entity *> bossParty;
    Enemy *finalBoss = new Enemy("Властелин Тьмы", 300, 25, 10, 8, 3, 3, 15, 2, AbilityType::LIFE_STEAL, 200, 5, "final_boss", 0.1);
    bossParty.push_back(finalBoss);

    // Добавляем пару приспешников
    Enemy *minion1 = EnemyFactory::createEnemyByName("Рыцарь смерти", 2);
    Enemy *minion2 = EnemyFactory::createEnemyByName("Архидьявол", 2);
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

        // Упрощенная логика боя
        vector<Entity *> targets = battleSystem.getAvailableTargetsForCurrent();
        if (!targets.empty())
        {
            if (currentEntity == finalBoss)
            {
                // Ход босса - атакует случайную цель
                int targetIndex = rand() % targets.size();
                battleSystem.attack(currentEntity, targets[targetIndex]);
                cout << "Властелин Тьмы атакует " << targets[targetIndex]->getName() << "!\n";
            }
            else
            {
                // Ход игрока
                cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";
                cout << "Осталось стамины: " << currentEntity->getCurrentStamina() << "/" << currentEntity->getMaxStamina() << "\n";
                cout << "Доступные цели:\n";
                for (size_t i = 0; i < targets.size(); ++i)
                {
                    cout << i + 1 << ". " << targets[i]->getName() << " (HP: " << targets[i]->getCurrentHealthPoint() << ")\n";
                }

                int targetChoice;
                cout << "Выберите цель (1-" << targets.size() << "): ";
                cin >> targetChoice;
                cin.ignore();

                if (targetChoice > 0 && targetChoice <= static_cast<int>(targets.size()))
                {
                    battleSystem.attack(currentEntity, targets[targetChoice - 1]);
                }
            }
        }

        battleSystem.nextTurn();

        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    }

    if (battleSystem.isPlayerVictory())
    {
        cout << "\n[VICTORY] ПОБЕДА! Вы победили Властелина Тьмы и спасли мир!\n";
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
        Event battleEvent{EventType::BATTLE, "Вы наткнулись на группу врагов!", {}, {}, nullptr, currentDifficulty};
        handleBattleEvent(battleEvent);
        break;
    }
    case NodeType::EVENT:
    {
        Event eventEvent{EventType::TEXT_EVENT, "Вы обнаружили странное явление...", {"Исследовать", "Игнорировать"}, {EventType::TREASURE, EventType::BATTLE}, nullptr, 0};
        handleTextEvent(eventEvent);
        break;
    }
    case NodeType::TREASURE:
    {
        Event treasureEvent{EventType::TREASURE, "Вы нашли скрытое сокровище!", {}, {}, new Item("Золотая монета", "Ценная монета", ItemType::CONSUMABLE, EquipmentSlot::NONE, {{"health_restore", 10}}), 0};
        handleTreasureEvent(treasureEvent);
        break;
    }
    case NodeType::EXIT:
    {
        Event exitEvent{EventType::EXIT, "Вы нашли выход из этой локации."};
        handleExitEvent(exitEvent);
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
        // Если игрок не указан, выбираем первого
        player = playerParty[0];
    }

    while (true)
    {
        cout << "\n=== УПРАВЛЕНИЕ ИНВЕНТАРЕМ ===\n";
        cout << "Герой: " << player->getName() << "\n\n";

        player->displayInventory();
        cout << "\n";
        player->displayEquipment();

        cout << "\nВыберите действие:\n";
        cout << "1. Надеть предмет\n";
        cout << "2. Снять предмет\n";
        cout << "3. Использовать расходуемый предмет\n";
        cout << "4. Выйти из управления инвентарем\n";

        int choice;
        cout << "Выберите (1-4): ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            // Надеть предмет
            if (player->getInventory().empty())
            {
                cout << "Инвентарь пуст!\n";
                continue;
            }

            cout << "Выберите предмет для надевания:\n";
            for (size_t i = 0; i < player->getInventory().size(); ++i)
            {
                cout << i + 1 << ". " << player->getInventory()[i].getName() << "\n";
            }

            int itemChoice;
            cout << "Выберите предмет (1-" << player->getInventory().size() << "): ";
            cin >> itemChoice;
            cin.ignore();

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

            int slotChoice;
            cout << "Выберите слот (1-10): ";
            cin >> slotChoice;
            cin.ignore();

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
            for (size_t i = 0; i < player->getInventory().size(); ++i)
            {
                cout << i + 1 << ". " << player->getInventory()[i].getName() << "\n";
            }

            int itemChoice;
            cout << "Выберите предмет (1-" << player->getInventory().size() << "): ";
            cin >> itemChoice;
            cin.ignore();

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
    for (Player *player : playerParty)
    {
        delete player;
    }
    playerParty.clear();
}
