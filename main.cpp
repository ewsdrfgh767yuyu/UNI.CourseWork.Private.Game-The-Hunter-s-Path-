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
             << ", Урон " << hero->getDamage() << ", Защита " << hero->getDefense() << "\n";
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
                 << ", Урон " << enemy->getDamage() << ", Защита " << enemy->getDefense() << "\n";

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
    cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";

    // Показываем доступные действия
    vector<string> actions = {"Атаковать", "Переместиться", "Использовать предмет", "Пропустить ход"};
    int actionChoice = selectFromList(actions, "Выберите действие:");

    switch (actionChoice) {
    case 0: { // Атаковать
        vector<Entity*> targets = battleSystem.getAvailableTargetsForCurrent();
        if (targets.empty()) {
            cout << "Нет доступных целей для атаки!\n";
            break;
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

    case 2: { // Использовать предмет
        cout << "Использование предметов пока не реализовано.\n";
        break;
    }

    case 3: { // Пропустить ход
        cout << currentEntity->getName() << " пропускает ход.\n";
        break;
    }
    }
}

// Функция для выполнения хода ИИ
void executeAITurn(BattleSystem& battleSystem, Entity* currentEntity) {
    cout << "\n=== ХОД " << currentEntity->getName() << " ===\n";

    // Простая ИИ логика: атаковать случайную цель или двигаться
    vector<Entity*> targets = battleSystem.getAvailableTargetsForCurrent();

    if (!targets.empty()) {
        // 70% шанс атаковать, 30% шанс двигаться
        if (rand() % 100 < 70) {
            int targetIndex = rand() % targets.size();
            cout << currentEntity->getName() << " атакует " << targets[targetIndex]->getName() << "!\n";
            battleSystem.attack(currentEntity, targets[targetIndex]);
        } else {
            int newPosition = rand() % 4;
            cout << currentEntity->getName() << " перемещается на позицию " << newPosition << "!\n";
            battleSystem.movePosition(currentEntity, newPosition);
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

        // Небольшая пауза для читаемости
        cout << "\nНажмите Enter для продолжения...";
        cin.get();
    }

    // Проверяем результат боя
    cout << "\n" << battleSystem.getBattleStatus() << "\n";

    if (battleSystem.isPlayerVictory()) {
        cout << "\n[PARTY] ПОБЕДА! Вы успешно победили врагов леса!\n";
        cout << "Ваш отряд получает опыт и может продолжить путешествие.\n";
    } else if (battleSystem.isPlayerDefeat()) {
        cout << "\n[SKULL] ПОРАЖЕНИЕ! Ваш отряд пал в бою!\n";
        cout << "Игра окончена. Попробуйте начать новую попытку.\n";
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
    
    // Лог для проверки отображения русского текста
    cout << "Тест: Русский текст должен отображаться корректно. Если видите кракозябры, проблема с кодировкой." << endl;
    
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));

    cout << "[GAME] ДОБРО ПОЖАЛОВАТЬ В 'THE HUNTER'S PATH'! [GAME]\n";
    cout << "Игра в стиле Darkest Dungeon с пошаговыми боями.\n";

    // Запускаем бой в лесу
    forestBattle();

    cout << "\nСпасибо за игру! До новых встреч в мире The Hunter's Path.\n";

    return 0;
}
