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
#include "CampaignSystem.h"

using namespace std;

// Функция для очистки ввода
void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Функция для очистки консоли
void clearConsole()
{
    system("cls");
}

// Функция для выбора из списка
int selectFromList(const vector<string> &options, const string &prompt)
{
    cout << "\n"
         << prompt << "\n";
    for (size_t i = 0; i < options.size(); ++i)
    {
        cout << i + 1 << ". " << options[i] << "\n";
    }

    int choice;
    while (true)
    {
        cout << "Выберите (1-" << options.size() << "): ";
        if (cin >> choice)
        {
            if (choice >= 1 && choice <= static_cast<int>(options.size()))
            {
                clearInput();
                return choice - 1;
            }
            else
            {
                cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
        else
        {
            cout << "Ошибка ввода. Попробуйте снова.\n";
        }
        clearInput();
    }
}

// Функция для создания отряда игроков
vector<Entity *> createPlayerParty()
{
    vector<Entity *> party;

    cout << "\n=== СОЗДАНИЕ ОТРЯДА ===\n";

    // Выбор размера отряда
    vector<string> partySizes = {"Одиночка (1 герой)", "Малый отряд (2 героя)", "Полный отряд (3 героя)"};
    int partySizeChoice = selectFromList(partySizes, "Выберите размер отряда:");

    int maxHeroes = partySizeChoice + 1;

    // Получаем доступные классы
    vector<HeroClass> availableClasses = HeroFactory::getAvailableClasses();
    vector<string> classNames;
    for (HeroClass cls : availableClasses)
    {
        const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(cls);
        classNames.push_back(tmpl.name + " - " + tmpl.description);
    }

    for (int i = 0; i < maxHeroes; ++i)
    {
        cout << "\n--- Выбор героя " << (i + 1) << " ---\n";

        int classChoice = selectFromList(classNames, "Выберите класс героя:");

        HeroClass selectedClass = availableClasses[classChoice];
        const HeroTemplate &tmpl = HeroFactory::getHeroTemplate(selectedClass);

        // Если выбран одиночка, создаем только его
        if (tmpl.isLoner)
        {
            cout << "Вы выбрали Одиночку! Отряд будет состоять только из него.\n";
            Player *loner = HeroFactory::createHero(selectedClass, "Одиночка");
            party.push_back(loner);
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
        party.push_back(hero);

        cout << "Создан " << hero->getName() << " (" << tmpl.name << ")\n";
        cout << "Характеристики: HP " << hero->getCurrentHealthPoint()
             << ", Урон " << hero->getDamage() << ", Защита " << hero->getDefense()
             << ", Дальность атаки " << hero->getAttackRange() << "\n";
    }

    return party;
}

// Функция для создания группы врагов
vector<Entity *> createEnemyGroup(LocationType location, int difficultyModifier = 0)
{
    vector<Entity *> enemies;

    // Получаем доступных врагов для локации
    vector<string> availableEnemies = EnemyFactory::getAvailableEnemies(location);

    if (availableEnemies.empty())
    {
        cout << "Ошибка: нет доступных врагов для этой локации!\n";
        return enemies;
    }

    // Выбор количества врагов
    vector<string> enemyCounts = {"1 враг", "2 врага", "3 врага"};
    int enemyCountChoice = selectFromList(enemyCounts, "Выберите количество врагов:");
    int enemyCount = enemyCountChoice + 1;

    cout << "\n=== ВРАГИ ЛЕСА ===\n";

    for (int i = 0; i < enemyCount; ++i)
    {
        // Случайный выбор врага из доступных
        int randomIndex = rand() % availableEnemies.size();
        string enemyName = availableEnemies[randomIndex];

        Enemy *enemy = EnemyFactory::createEnemyByName(enemyName, difficultyModifier);

        if (enemy)
        {
            enemies.push_back(enemy);
            cout << "Появился " << enemy->getName() << "!\n";
            cout << "Характеристики: HP " << enemy->getCurrentHealthPoint()
                 << ", Урон " << enemy->getDamage() << ", Защита " << enemy->getDefense()
                 << ", Дальность атаки " << enemy->getAttackRange() << "\n";

            // Показываем способность
            AbilityType ability = enemy->getAbility();
            if (ability != AbilityType::NONE)
            {
                const AbilityInfo &info = HeroFactory::getAbilityInfo(ability);
                cout << "Способность: " << info.name << " - " << info.description << "\n";
            }
            cout << "\n";
        }
    }

    return enemies;
}

// Функция для выполнения хода игрока
void executePlayerTurn(BattleSystem &battleSystem, Entity *currentEntity)
{
    cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";
    cout << "Осталось стамины: " << currentEntity->getCurrentStamina() << "/" << currentEntity->getMaxStamina() << "\n";

    while (currentEntity->getCurrentStamina() > 0)
    {
        // Показываем доступные действия
        vector<string> actions = {"Атаковать", "Переместиться", "Посмотреть характеристики", "Пропустить половину хода", "Закончить ход"};
        int actionChoice = selectFromList(actions, "Выберите действие:");

        switch (actionChoice)
        {
        case 0:
        { // Атаковать
            vector<pair<Entity *, int>> targets = battleSystem.getAvailableTargetsForCurrent();
            if (targets.empty())
            {
                cout << "Нет доступных целей для атаки!\n";
                continue; // Не завершаем ход
            }

            vector<string> targetNames;
            for (const auto &targetPair : targets)
            {
                Entity *target = targetPair.first;
                int position = targetPair.second;
                targetNames.push_back(target->getName() + " (HP: " + to_string(target->getCurrentHealthPoint()) + ", Pos: " + to_string(position) + ")");
            }

            int targetChoice = selectFromList(targetNames, "Выберите цель:");
            battleSystem.attack(currentEntity, targets[targetChoice].first);
            break;
        }

        case 1:
        { // Переместиться
            vector<string> positions = {"Позиция 0 (передняя линия)", "Позиция 1 (передняя линия)",
                                        "Позиция 2 (задняя линия)", "Позиция 3 (задняя линия)"};
            int positionChoice = selectFromList(positions, "Выберите позицию:");
            battleSystem.movePosition(currentEntity, positionChoice);
            break;
        }

        case 2:
        { // Посмотреть характеристики
            auto allEntities = battleSystem.getAllEntitiesWithStatus();
            vector<string> entityNames;
            vector<Entity *> entityList;

            for (const auto &entityPair : allEntities)
            {
                Entity *entity = entityPair.first;
                if (entity)
                {
                    entityNames.push_back(entity->getName() + " (" + entityPair.second + ")");
                    entityList.push_back(entity);
                }
            }

            int entityChoice = selectFromList(entityNames, "Выберите персонажа для просмотра характеристик:");
            battleSystem.displayEntityDetails(entityList[entityChoice]);
            continue; // Не тратим стамину на просмотр
        }

        case 3:
        { // Пропустить половину хода
            battleSystem.skipHalfTurn(currentEntity);
            break;
        }

        case 4:
        { // Закончить ход
            cout << currentEntity->getName() << " заканчивает ход.\n";
            return;
        }
        }
    }

    cout << currentEntity->getName() << " израсходовал всю стамину и заканчивает ход.\n";
}

// Функция для выполнения хода ИИ
void executeAITurn(BattleSystem &battleSystem, Entity *currentEntity)
{
    cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";

    // Простая ИИ логика: атаковать случайную цель или двигаться
    vector<pair<Entity *, int>> targets = battleSystem.getAvailableTargetsForCurrent();

    if (!targets.empty())
    {
        // 70% шанс атаковать, 30% шанс двигаться
        if (rand() % 100 < 70)
        {
            int targetIndex = rand() % targets.size();
            cout << currentEntity->getName() << " атакует " << targets[targetIndex].first->getName() << "!\n";
            battleSystem.attack(currentEntity, targets[targetIndex].first);
        }
        else
        {
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
            const auto &enemyPositions = battleSystem.getEnemyPositions();
            // Проверяем, есть ли пустые позиции на стороне врагов
            bool hasEmpty = false;
            for (int i = 0; i < 4; ++i)
            {
                bool occupied = false;
                for (const auto &pos : enemyPositions)
                {
                    if (pos.position == i)
                    {
                        occupied = true;
                        break;
                    }
                }
                if (!occupied)
                {
                    hasEmpty = true;
                    break;
                }
            }

            if (!hasEmpty)
            {
                // Если нет пустых позиций, ИИ может менять местами
                // Для простоты выбираем случайную позицию союзника
                vector<int> allyPositions;
                for (const auto &pos : enemyPositions)
                {
                    if (pos.entity && pos.entity != currentEntity)
                    {
                        allyPositions.push_back(pos.position);
                    }
                }
                if (!allyPositions.empty())
                {
                    int randomAllyIndex = rand() % allyPositions.size();
                    int newPosition = allyPositions[randomAllyIndex];
                    cout << currentEntity->getName() << " меняется местами с союзником на позицию " << newPosition << "!\n";
                    battleSystem.movePosition(currentEntity, newPosition);
                    canMove = true;
                }
            }

            if (!canMove)
            {
                cout << currentEntity->getName() << " не может перемещаться!\n";
            }
        }
    }
    else
    {
        cout << currentEntity->getName() << " не может действовать!\n";
    }
}

int main()
{
    // Установка кодовой страницы консоли для корректного отображения русского текста
    if (setlocale(LC_ALL, ".65001") == nullptr)
    {
        cerr << "Ошибка установки локали." << endl;
    }

    // Устанавливаем кодовую страницу консоли на UTF-8
    if (!SetConsoleOutputCP(65001))
    {
        cerr << "Ошибка установки кодовой страницы консоли." << endl;
    }
    if (!SetConsoleCP(65001))
    {
        cerr << "Ошибка установки кодовой страницы ввода консоли." << endl;
    }

    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));

    // Начальное меню
    bool exitGame = false;
    while (!exitGame)
    {
        clearConsole();
        cout << "+================================================================+\n";
        cout << "|                                                                |\n";
        cout << "|                    THE HUNTER'S PATH                           |\n";
        cout << "|                                                                |\n";
        cout << "|              Эпическая RPG с пошаговыми боями                  |\n";
        cout << "|                                                                |\n";
        cout << "|              Добро пожаловать, охотник!                        |\n";
        cout << "|                                                                |\n";
        cout << "+================================================================+\n";
        cout << "\n";

        vector<string> menuOptions = {"Начать игру", "Титры", "Выход из игры"};
        int menuChoice = selectFromList(menuOptions, "Выберите опцию:");

        switch (menuChoice)
        {
        case 0:
        { // Начать игру
            clearConsole();
            // Запускаем походовый режим
            CampaignSystem campaign;
            campaign.startCampaign();
            clearConsole();
            break;
        }
        case 1:
        { // Титры
            clearConsole();
            cout << "+================================================================+\n";
            cout << "|                                                              |\n";
            cout << "|                           CREDITS                             |\n";
            cout << "|                                                              |\n";
            cout << "|              Mikulski Stanislau - BSUIR student              |\n";
            cout << "|                                                              |\n";
            cout << "+================================================================+\n";
            cout << "\nНажмите Enter для возврата в меню...";
            cin.get();
            break;
        }
        case 2:
        { // Выход из игры
            exitGame = true;
            break;
        }
        }
    }

    cout << "\nСпасибо за игру! До новых встреч в мире The Hunter's Path.\n";

    return 0;
}
