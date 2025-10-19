#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
#include <clocale>
#include <io.h>
#include <fcntl.h>
#include "entity.h"
#include "BattleSystem.h"
#include "EnemyTemplates.h"
#include "HeroTemplates.h"

using namespace std;

// Функция для очистки ввода
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Функция для выбора из списка
int selectFromList(const vector<string>& options, const string& prompt) {
    cout << "\n" << prompt << "\n";
    for (size_t i = 0; i < options.size(); ++i) {
        cout << i + 1 << ". " << options[i] << "\n";
    }

    int choice;
    while (true) {
        cout << "Выберите (1-" << options.size() << "): ";
        if (cin >> choice) {
            if (choice >= 1 && choice <= static_cast<int>(options.size())) {
                clearInput();
                return choice - 1;
            } else {
                cout << "Неверный выбор. Попробуйте снова.\n";
            }
        } else {
            cout << "Ошибка ввода. Попробуйте снова.\n";
        }
        clearInput();
    }
}

// Функция для создания отряда игроков
vector<Entity*> createPlayerParty() {
    vector<Entity*> party;

    cout << "\n=== СОЗДАНИЕ ОТРЯДА ===\n";

    // Выбор размера отряда
    vector<string> partySizes = {"Одиночка (1 герой)", "Малый отряд (2 героя)", "Полный отряд (3 героя)"};
    int partySizeChoice = selectFromList(partySizes, "Выберите размер отряда:");

    int maxHeroes = partySizeChoice + 1;

    // Получаем доступные классы
    vector<HeroClass> availableClasses = HeroFactory::getAvailableClasses();
    vector<string> classNames;
    for (HeroClass cls : availableClasses) {
        const HeroTemplate& tmpl = HeroFactory::getHeroTemplate(cls);
        classNames.push_back(tmpl.name + " - " + tmpl.description);
    }

    for (int i = 0; i < maxHeroes; ++i) {
        cout << "\n--- Выбор героя " << (i + 1) << " ---\n";

        int classChoice = selectFromList(classNames, "Выберите класс героя:");

        HeroClass selectedClass = availableClasses[classChoice];
        const HeroTemplate& tmpl = HeroFactory::getHeroTemplate(selectedClass);

        // Если выбран одиночка, создаем только его
        if (tmpl.isLoner) {
            cout << "Вы выбрали Одиночку! Отряд будет состоять только из него.\n";
            Player* loner = HeroFactory::createHero(selectedClass, "Одиночка");
            party.push_back(loner);
            break;
        }

        // Ввод имени героя
        string heroName;
        cout << "Введите имя героя: ";
        getline(cin, heroName);
        if (heroName.empty()) {
            heroName = tmpl.name;
        }

        Player* hero = HeroFactory::createHero(selectedClass, heroName);
        party.push_back(hero);

        cout << "Создан " << hero->getName() << " (" << tmpl.name << ")\n";
        cout << "Характеристики: HP " << hero->getCurrentHealthPoint()
             << ", Урон " << hero->getDamage() << ", Защита " << hero->getDefense()
             << ", Дальность атаки " << hero->getAttackRange() << "\n";
    }

    return party;
}

// Функция для создания группы врагов
vector<Entity*> createEnemyGroup(LocationType location, int difficultyModifier = 0) {
    vector<Entity*> enemies;

    // Получаем доступных врагов для локации
    vector<string> availableEnemies = EnemyFactory::getAvailableEnemies(location);

    if (availableEnemies.empty()) {
        cout << "Ошибка: нет доступных врагов для этой локации!\n";
        return enemies;
    }

    // Выбор количества врагов
    vector<string> enemyCounts = {"1 враг", "2 врага", "3 врага"};
    int enemyCountChoice = selectFromList(enemyCounts, "Выберите количество врагов:");
    int enemyCount = enemyCountChoice + 1;

    cout << "\n=== ВРАГИ ЛЕСА ===\n";

    for (int i = 0; i < enemyCount; ++i) {
        // Случайный выбор врага из доступных
        int randomIndex = rand() % availableEnemies.size();
        string enemyName = availableEnemies[randomIndex];

        Enemy* enemy = EnemyFactory::createEnemyByName(enemyName, difficultyModifier);

        if (enemy) {
            enemies.push_back(enemy);
            cout << "Появился " << enemy->getName() << "!\n";
            cout << "Характеристики: HP " << enemy->getCurrentHealthPoint()
                 << ", Урон " << enemy->getDamage() << ", Защита " << enemy->getDefense()
                 << ", Дальность атаки " << enemy->getAttackRange() << "\n";

            // Показываем способность
            AbilityType ability = enemy->getAbility();
            if (ability != AbilityType::NONE) {
                const AbilityInfo& info = HeroFactory::getAbilityInfo(ability);
                cout << "Способность: " << info.name << " - " << info.description << "\n";
            }
            cout << "\n";
        }
    }

    return enemies;
}

// Функция для выполнения хода игрока
void executePlayerTurn(BattleSystem& battleSystem, Entity* currentEntity) {
    cout << "\n=== ХОД " << currentEntity->getName() << " (" << getEntityClassName(currentEntity) << ") ===\n";
    cout << "Осталось стамины: " << currentEntity->getCurrentStamina() << "/" << currentEntity->getMaxStamina() << "\n";

    while (currentEntity->getCurrentStamina() > 0) {
        // Показываем доступные действия
        vector<string> actions = {"Атаковать", "Переместиться", "Посмотреть характеристики", "Закончить ход"};
        int actionChoice = selectFromList(actions, "Выберите действие:");

        switch (actionChoice) {
        case 0: { // Атаковать
            vector<Entity*> targets = battleSystem.getAvailableTargetsForCurrent();
            if (targets.empty()) {
                cout << "Нет доступных целей для атаки!\n";
                continue; // Не завершаем ход
            }

            vector<string> targetNames;
            for (Entity* target : targets) {
                targetNames.push_back(target->getName() + " (HP: " + to_string(target->getCurrentHealthPoint()) + ")");
            }

            int targetChoice = selectFromList(targetNames, "Выберите цель:");
            battleSystem.attack(currentEntity, targets[targetChoice]);
            break;
        }

        case 1: { // Переместиться
            vector<string> positions = {"Позиция 0 (передняя линия)", "Позиция 1 (передняя линия)",
                                       "Позиция 2 (задняя линия)", "Позиция 3 (задняя линия)"};
            int positionChoice = selectFromList(positions, "Выберите позицию:");
            battleSystem.movePosition(currentEntity, positionChoice);
            break;
        }

        case 2: { // Посмотреть характеристики
            auto allEntities = battleSystem.getAllEntitiesWithStatus();
            vector<string> entityNames;
            vector<Entity*> entityList;

            for (const auto& entityPair : allEntities) {
                Entity* entity = entityPair.first;
                if (entity) {
                    entityNames.push_back(entity->getName() + " (" + entityPair.second + ")");
                    entityList.push_back(entity);
                }
            }

            int entityChoice = selectFromList(entityNames, "Выберите персонажа для просмотра характеристик:");
            battleSystem.displayEntityDetails(entityList[entityChoice]);
            continue; // Не тратим стамину на просмотр
        }

        case 3: { // Закончить ход
            cout << currentEntity->getName() << " заканчивает ход.\n";
            return;
        }
        }
    }

    cout << currentEntity->getName() << " израсходовал всю стамину и заканчивает ход.\n";
}

// Функция для отображения класса персонажа
string getEntityClassName(Entity* entity) {
    // Для игроков возвращаем класс героя
    for (const auto& pair : HeroFactory::getAvailableClasses()) {
        // Это упрощенная проверка, в реальности нужно хранить класс в Entity
        // Пока что возвращаем просто имя
        return entity->getName();
    }
    return entity->getName(); // Для врагов возвращаем имя
}

// Функция для выполнения хода ИИ
void executeAITurn(BattleSystem& battleSystem, Entity* currentEntity) {
    cout << "\n=== ХОД " << currentEntity->getName() << " (" << getEntityClassName(currentEntity) << ") ===\n";

    // Простая ИИ логика: атаковать случайную цель или двигаться
    vector<Entity*> targets = battleSystem.getAvailableTargetsForCurrent();

    if (!targets.empty()) {
        // 70% шанс атаковать, 30% шанс двигаться
        if (rand() % 100 < 70) {
            int targetIndex = rand() % targets.size();
            cout << currentEntity->getName() << " атакует " << targets[targetIndex]->getName() << "!\n";
            battleSystem.attack(currentEntity, targets[targetIndex]);
        } else {
            // Для ИИ тоже проверяем логику перемещения
            // Определяем, игрок это или враг
            bool isPlayerEntity = false;
            // ИИ всегда враг, но для проверки логики перемещения нужно определить сторону
            // В данной реализации ИИ всегда на стороне врагов

            // Проверяем, есть ли пустые позиции на стороне врагов
            // Если есть пустые позиции, ИИ не может перемещаться
            // Если нет пустых позиций, ИИ может менять местами с союзниками

            // Для простоты ИИ будет пытаться перемещаться только если это возможно
            // (т.е. менять местами с союзниками, если нет пустых позиций)
            bool canMove = false;

            // Определяем сторону ИИ (враги)
            const auto& enemyPositions = battleSystem.getEnemyPositions();
            // Проверяем, есть ли пустые позиции на стороне врагов
            bool hasEmpty = false;
            for (int i = 0; i < 4; ++i) {
                bool occupied = false;
                for (const auto& pos : enemyPositions) {
                    if (pos.position == i) {
                        occupied = true;
                        break;
                    }
                }
                if (!occupied) {
                    hasEmpty = true;
                    break;
                }
            }

            if (!hasEmpty) {
                // Если нет пустых позиций, ИИ может менять местами
                // Для простоты выбираем случайную позицию союзника
                vector<int> allyPositions;
                for (const auto& pos : enemyPositions) {
                    if (pos.entity && pos.entity != currentEntity && !pos.isCorpse) {
                        allyPositions.push_back(pos.position);
                    }
                }
                if (!allyPositions.empty()) {
                    int randomAllyIndex = rand() % allyPositions.size();
                    int newPosition = allyPositions[randomAllyIndex];
                    cout << currentEntity->getName() << " меняется местами с союзником на позицию " << newPosition << "!\n";
                    battleSystem.movePosition(currentEntity, newPosition);
                    canMove = true;
                }
            }

            if (!canMove) {
                cout << currentEntity->getName() << " не может перемещаться!\n";
            }
        }
    } else {
        cout << currentEntity->getName() << " не может действовать!\n";
    }
}

// Основная функция боя в лесу
void forestBattle() {
    cout << "\n[TREE][TREE][TREE] БОЙ В ЛЕСУ [TREE][TREE][TREE]\n";
    cout << "Вы углубляетесь в темный лес и натыкаетесь на враждебных существ!\n";

    // Создаем отряд игрока
    vector<Entity*> playerParty = createPlayerParty();

    // Создаем группу врагов леса
    vector<Entity*> enemyParty = createEnemyGroup(LocationType::FOREST, 0);

    // Создаем систему боя
    BattleSystem battleSystem;

    // Начинаем бой
    battleSystem.startBattle(playerParty, enemyParty);

    // Основной цикл боя
    while (battleSystem.isBattleActive()) {
        // Показываем текущее состояние поля
        cout << "\n" << battleSystem.getBattleStatus() << "\n";
        battleSystem.printBattlefield();
        battleSystem.printTurnOrder();

        // Получаем текущего действующего персонажа
        Entity* currentEntity = battleSystem.getCurrentTurnEntity();

        if (!currentEntity) {
            cout << "Ошибка: нет активного персонажа!\n";
            break;
        }

        // Определяем, игрок это или ИИ
        bool isPlayerEntity = false;
        for (Entity* player : playerParty) {
            if (player == currentEntity) {
                isPlayerEntity = true;
                break;
            }
        }

        if (isPlayerEntity) {
            // Ход игрока
            executePlayerTurn(battleSystem, currentEntity);
        } else {
            // Ход ИИ
            executeAITurn(battleSystem, currentEntity);
        }

        // Переход к следующему ходу
        battleSystem.nextTurn();

        // Небольшая пауза для читаемости
        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    }

    // Проверяем результат боя
    cout << "\n" << battleSystem.getBattleStatus() << "\n";
    battleSystem.printBattlefield();

    if (battleSystem.isPlayerVictory()) {
        cout << "\n[PARTY] ПОБЕДА! Вы успешно победили врагов леса!\n";
        cout << "Ваш отряд получает опыт и может продолжить путешествие.\n";
        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    } else if (battleSystem.isPlayerDefeat()) {
        cout << "\n[SKULL] ПОРАЖЕНИЕ! Ваш отряд пал в бою!\n";
        cout << "Игра окончена. Попробуйте начать новую попытку.\n";
        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    }

    // Завершаем бой
    battleSystem.endBattle();

    // Освобождаем память
    for (Entity* entity : playerParty) {
        delete entity;
    }
    for (Entity* entity : enemyParty) {
        delete entity;
    }
}

int main() {
    // Установка кодовой страницы консоли для корректного отображения русского текста
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // Установка локали для корректного отображения русского текста
    setlocale(LC_ALL, ".65001");

    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));

    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                                                              ║\n";
    cout << "║                    THE HUNTER'S PATH                         ║\n";
    cout << "║                                                              ║\n";
    cout << "║              Эпическая RPG с пошаговыми боями               ║\n";
    cout << "║                                                              ║\n";
    cout << "║              Добро пожаловать, охотник!                      ║\n";
    cout << "║                                                              ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    cout << "\n";

    // Запускаем бой в лесу
    forestBattle();

    cout << "\nСпасибо за игру! До новых встреч в мире The Hunter's Path.\n";

    return 0;
}
