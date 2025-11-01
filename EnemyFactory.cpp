#include "EnemyTemplates.h"
#include "entity.h"
#include <vector>
#include <map>
#include <random>
#include <ctime>

// Initialize static member
std::map<LocationType, std::vector<EnemyTemplate>> EnemyFactory::enemyTemplates;
std::vector<std::string> defaultEnemies = {"Гоблин", "Орк", "Тролль", "Волк", "Бандит", "Разбойник"};

void EnemyFactory::initializeTemplates()
{
    // ЛЕС - летающие и быстрые существа
    enemyTemplates[LocationType::FOREST] = {
        // Виверна - базовый летающий враг
        {"Виверна", 80, 12, 3, 2, 2, 14, 1, AbilityType::FLYING, 40, 1, "wyvern", 0.25},

        // Виверна-монарх - улучшенная версия
        {"Виверна-монарх", 120, 16, 5, 3, 2, 16, 1, AbilityType::FLYING, 70, 2, "wyvern_monarch", 0.2},

        // Змий - ядовитый летающий змей
        {"Змий", 60, 10, 2, 1, 1, 12, 0, AbilityType::POISON, 35, 1, "serpent", 0.15},

        // Кентавр - быстрый и агрессивный
        {"Кентавр", 100, 14, 4, 3, 2, 18, 0, AbilityType::BERSERK, 55, 2, "centaur", 0.35}};

    // ПЕЩЕРА - подземные существа
    enemyTemplates[LocationType::CAVE] = {
        // Троглодит - базовый пещерный житель
        {"Троглодит", 90, 11, 4, 2, 1, 9, 0, AbilityType::REGENERATION, 45, 1, "troglodyte", 0.2},

        // Инфернальный троглодит - огненная версия
        {"Инфернальный троглодит", 110, 15, 5, 3, 2, 11, 1, AbilityType::FIRE_DAMAGE, 75, 2, "infernal_troglodyte", 0.25},

        // Земляной элементаль - танкующий враг
        {"Земляной элементаль", 150, 8, 8, 1, 1, 6, 0, AbilityType::REGENERATION, 80, 3, "earth_elemental", 0.1},

        // Фамильяр - быстрый и слабый, но с магией
        {"Фамильяр", 50, 6, 1, 2, 2, 15, 2, AbilityType::LIGHTNING, 30, 1, "familiar", 0.3}};

    // ЗАМОК - финальная локация, регулярные враги отсутствуют (только босс-бой)
    enemyTemplates[LocationType::CASTLE] = {};

    // ГОРОД МЕРТВЕЦОВ - чистая нежить
    enemyTemplates[LocationType::DEAD_CITY] = {
        // Скелет - базовый скелет
        {"Скелет", 40, 6, 1, 1, 1, 8, 0, AbilityType::NONE, 20, 1, "skeleton", 0.3},

        // Скелет-воин - вооруженный скелет
        {"Скелет-воин", 70, 10, 3, 2, 1, 10, 0, AbilityType::NONE, 35, 1, "skeleton_warrior", 0.25},

        // Вампир - классический вампир
        {"Вампир", 100, 14, 4, 3, 2, 14, 1, AbilityType::LIFE_STEAL, 65, 2, "vampire", 0.2},

        // Призрак - нематериальный враг
        {"Призрак", 60, 8, 0, 2, 1, 12, 1, AbilityType::INVISIBLE, 50, 2, "ghost", 0.4}};
}

Enemy *EnemyFactory::createRandomEnemy(LocationType location, int difficultyModifier)
{
    if (enemyTemplates.empty())
    {
        initializeTemplates();
    }

    auto it = enemyTemplates.find(location);
    if (it == enemyTemplates.end() || it->second.empty())
    {
        int randomIndex = rand() % defaultEnemies.size();
        return new Enemy(defaultEnemies[randomIndex], 50, 8, 2, 2, 1, 1, 8, 0, AbilityType::NONE, 25, 1, "unknown", 0.2);
    }

    const auto &templates = it->second;

    // Выбираем шаблон случайно
    int index = rand() % templates.size();
    const EnemyTemplate &selected = templates[index];

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
        selected.type,
        selected.damageVariance);
}

Enemy *EnemyFactory::createEnemyByName(const std::string &name, int difficultyModifier)
{
    if (enemyTemplates.empty())
    {
        initializeTemplates();
    }

    // Ищем врага по всем локациям
    for (const auto &locationPair : enemyTemplates)
    {
        for (const auto &enemyTemplate : locationPair.second)
        {
            if (enemyTemplate.name == name)
            {
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
                    enemyTemplate.type,
                    enemyTemplate.damageVariance);
            }
        }
    }

    // Если не нашли, возвращаем дефолтного врага с случайным именем
    int randomIndex = rand() % defaultEnemies.size();
    return new Enemy(defaultEnemies[randomIndex], 50, 8, 2, 2, 1, 1, 8, 0, AbilityType::NONE, 25, 1, "unknown", 0.2);
}

std::vector<std::string> EnemyFactory::getAvailableEnemies(LocationType location)
{
    if (enemyTemplates.empty())
    {
        initializeTemplates();
    }

    std::vector<std::string> names;
    auto it = enemyTemplates.find(location);
    if (it != enemyTemplates.end())
    {
        for (const auto &enemyTemplate : it->second)
        {
            names.push_back(enemyTemplate.name);
        }
    }
    return names;
}
