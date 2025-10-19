#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <random>
#include <ctime>

// Структура шаблона врага
struct EnemyTemplate {
    std::string name;
    int maxHP;
    int damage;
    int defense;
    int attack;
    int maxStamina;
    int initiative;
    int attackRange;
    AbilityType ability;
    int expValue;
    int difficulty;
    std::string type;
};

// Фабрика для создания врагов по шаблонам
class EnemyFactory {
private:
    static std::map<LocationType, std::vector<EnemyTemplate>> enemyTemplates;

    static void initializeTemplates();

public:
    // Получить случайного врага для локации
    static Enemy* createRandomEnemy(LocationType location, int difficultyModifier = 0);

    // Получить конкретного врага по имени
    static Enemy* createEnemyByName(const std::string& name, int difficultyModifier = 0);

    // Получить список доступных врагов для локации
    static std::vector<std::string> getAvailableEnemies(LocationType location);
};

// Инициализация шаблонов врагов
std::map<LocationType, std::vector<EnemyTemplate>> EnemyFactory::enemyTemplates;

void EnemyFactory::initializeTemplates() {
    // ЛЕС - летающие и быстрые существа
    enemyTemplates[LocationType::FOREST] = {
        // Виверна - базовый летающий враг
        {"Виверна", 80, 12, 3, 2, 2, 14, 1, AbilityType::FLYING, 40, 1, "wyvern"},

        // Виверна-монарх - улучшенная версия
        {"Виверна-монарх", 120, 16, 5, 3, 2, 16, 1, AbilityType::FLYING, 70, 2, "wyvern_monarch"},

        // Змий - ядовитый летающий змей
        {"Змий", 60, 10, 2, 1, 1, 12, 0, AbilityType::POISON, 35, 1, "serpent"},

        // Кентавр - быстрый и агрессивный
        {"Кентавр", 100, 14, 4, 3, 2, 18, 0, AbilityType::BERSERK, 55, 2, "centaur"}
    };

    // ПЕЩЕРА - подземные существа
    enemyTemplates[LocationType::CAVE] = {
        // Троглодит - базовый пещерный житель
        {"Троглодит", 90, 11, 4, 2, 1, 9, 0, AbilityType::REGENERATION, 45, 1, "troglodyte"},

        // Инфернальный троглодит - огненная версия
        {"Инфернальный троглодит", 110, 15, 5, 3, 2, 11, 1, AbilityType::FIRE_DAMAGE, 75, 2, "infernal_troglodyte"},

        // Земляной элементаль - танкующий враг
        {"Земляной элементаль", 150, 8, 8, 1, 1, 6, 0, AbilityType::REGENERATION, 80, 3, "earth_elemental"},

        // Фамильяр - быстрый и слабый, но с магией
        {"Фамильяр", 50, 6, 1, 2, 2, 15, 2, AbilityType::LIGHTNING, 30, 1, "familiar"}
    };

    // ЗАМОК - нежить и демоны
    enemyTemplates[LocationType::CASTLE] = {
        // Рыцарь смерти - танкующий нежить
        {"Рыцарь смерти", 140, 18, 7, 4, 2, 10, 0, AbilityType::FEAR, 90, 3, "death_knight"},

        // Лорд вампир - вампир с вампиризмом
        {"Лорд вампир", 120, 16, 6, 3, 2, 13, 1, AbilityType::LIFE_STEAL, 85, 3, "vampire_lord"},

        // Архидьявол - мощный демон с огнем
        {"Архидьявол", 180, 22, 8, 5, 3, 12, 2, AbilityType::FIRE_DAMAGE, 120, 4, "archdevil"},

        // Злобоглаз - летающий наблюдатель с молниями
        {"Злобоглаз", 70, 9, 2, 3, 1, 16, 3, AbilityType::LIGHTNING, 60, 2, "beholder"}
    };

    // ГОРОД МЕРТВЕЦОВ - чистая нежить
    enemyTemplates[LocationType::DEAD_CITY] = {
        // Скелет - базовый скелет
        {"Скелет", 40, 6, 1, 1, 1, 8, 0, AbilityType::NONE, 20, 1, "skeleton"},

        // Скелет-воин - вооруженный скелет
        {"Скелет-воин", 70, 10, 3, 2, 1, 10, 0, AbilityType::NONE, 35, 1, "skeleton_warrior"},

        // Вампир - классический вампир
        {"Вампир", 100, 14, 4, 3, 2, 14, 1, AbilityType::LIFE_STEAL, 65, 2, "vampire"},

        // Призрак - нематериальный враг
        {"Призрак", 60, 8, 0, 2, 1, 12, 1, AbilityType::INVISIBLE, 50, 2, "ghost"}
    };
}

Enemy* EnemyFactory::createRandomEnemy(LocationType location, int difficultyModifier) {
    if (enemyTemplates.empty()) {
        initializeTemplates();
    }

    auto it = enemyTemplates.find(location);
    if (it == enemyTemplates.end() || it->second.empty()) {
        return new Enemy("Неизвестный враг", 50, 8, 2, 2, 1, 1, 8, 0, AbilityType::NONE, 25, 1, "unknown");
    }

    const auto& templates = it->second;

    // Выбираем случайный шаблон
    srand(static_cast<unsigned int>(time(nullptr)));
    int randomIndex = rand() % templates.size();
    const EnemyTemplate& selected = templates[randomIndex];

    // Применяем модификатор сложности
    int modifiedHP = selected.maxHP + (difficultyModifier * 20);
    int modifiedDamage = selected.damage + difficultyModifier;
    int modifiedDefense = selected.defense + difficultyModifier;
    int modifiedExp = selected.expValue + (difficultyModifier * 10);

    return new Enemy(
        selected.name,
        modifiedHP,
        modifiedDamage,
        modifiedDefense,
        selected.attack,
        selected.maxStamina,
        selected.maxStamina,
        selected.initiative,
        selected.attackRange,
        selected.ability,
        modifiedExp,
        selected.difficulty + difficultyModifier,
        selected.type
    );
}

Enemy* EnemyFactory::createEnemyByName(const std::string& name, int difficultyModifier) {
    if (enemyTemplates.empty()) {
        initializeTemplates();
    }

    // Ищем врага по всем локациям
    for (const auto& locationPair : enemyTemplates) {
        for (const auto& enemyTemplate : locationPair.second) {
            if (enemyTemplate.name == name) {
                int modifiedHP = enemyTemplate.maxHP + (difficultyModifier * 20);
                int modifiedDamage = enemyTemplate.damage + difficultyModifier;
                int modifiedDefense = enemyTemplate.defense + difficultyModifier;
                int modifiedExp = enemyTemplate.expValue + (difficultyModifier * 10);

                return new Enemy(
                    enemyTemplate.name,
                    modifiedHP,
                    modifiedDamage,
                    modifiedDefense,
                    enemyTemplate.attack,
                    enemyTemplate.maxStamina,
                    enemyTemplate.maxStamina,
                    enemyTemplate.initiative,
                    enemyTemplate.attackRange,
                    enemyTemplate.ability,
                    modifiedExp,
                    enemyTemplate.difficulty + difficultyModifier,
                    enemyTemplate.type
                );
            }
        }
    }

    // Если не нашли, возвращаем дефолтного врага
    return new Enemy("Неизвестный враг", 50, 8, 2, 2, 1, 1, 8, 0, AbilityType::NONE, 25, 1, "unknown");
}

std::vector<std::string> EnemyFactory::getAvailableEnemies(LocationType location) {
    if (enemyTemplates.empty()) {
        initializeTemplates();
    }

    std::vector<std::string> names;
    auto it = enemyTemplates.find(location);
    if (it != enemyTemplates.end()) {
        for (const auto& enemyTemplate : it->second) {
            names.push_back(enemyTemplate.name);
        }
    }
    return names;
}