#include "EnemyTemplates.h"
#include "entity.h"
#include <vector>
#include <map>
#include <random>
#include <ctime>

// Initialize static member
std::map<LocationType, std::vector<EnemyTemplate>> EnemyFactory::enemyTemplates;
std::vector<std::string> defaultEnemies = {"Goblin", "Orc", "Troll", "Wolf", "Bandit", "Robber"};

void EnemyFactory::initializeTemplates()
{
    // FOREST - flying and fast creatures
    enemyTemplates[LocationType::FOREST] = {
        // Wyvern - basic flying enemy
        {"Wyvern", 80, 12, 3, 2, 2, 14, 1, AbilityType::FLYING, 40, 1, "wyvern", 0.25},

        // Wyvern-monarch - improved version
        {"Wyvern-monarch", 120, 16, 5, 3, 2, 16, 1, AbilityType::FLYING, 70, 2, "wyvern_monarch", 0.2},

        // Serpent - poisonous flying snake
        {"Serpent", 60, 10, 2, 1, 1, 12, 0, AbilityType::POISON, 35, 1, "serpent", 0.15},

        // Centaur - fast and aggressive
        {"Centaur", 100, 14, 4, 3, 2, 18, 0, AbilityType::BERSERK, 55, 2, "centaur", 0.35}};

    // CAVE - underground creatures
    enemyTemplates[LocationType::CAVE] = {
        // Troglodyte - basic cave dweller
        {"Troglodyte", 90, 11, 4, 2, 1, 9, 0, AbilityType::REGENERATION, 45, 1, "troglodyte", 0.2},

        // Infernal troglodyte - fire version
        {"Infernal troglodyte", 110, 15, 5, 3, 2, 11, 1, AbilityType::FIRE_DAMAGE, 75, 2, "infernal_troglodyte", 0.25},

        // Earth elemental - tanking enemy
        {"Earth elemental", 150, 8, 8, 1, 1, 6, 0, AbilityType::REGENERATION, 80, 3, "earth_elemental", 0.1},

        // Familiar - fast and weak, but with magic
        {"Familiar", 50, 6, 1, 2, 2, 15, 2, AbilityType::LIGHTNING, 30, 1, "familiar", 0.3}};

    // CASTLE - final location, guards and knights
    enemyTemplates[LocationType::CASTLE] = {
        // Guard - basic castle defender
        {"Guard", 100, 12, 6, 2, 1, 10, 0, AbilityType::NONE, 50, 2, "guard", 0.2},

        // Knight - heavy warrior
        {"Knight", 140, 16, 8, 3, 1, 12, 0, AbilityType::BERSERK, 80, 3, "knight", 0.25},

        // Castle mage - magical enemy
        {"Castle mage", 80, 10, 2, 2, 2, 14, 2, AbilityType::LIGHTNING, 60, 2, "castle_mage", 0.3},

        // Ghost guard - immaterial defender
        {"Ghost guard", 90, 11, 3, 2, 1, 16, 1, AbilityType::INVISIBLE, 70, 2, "ghost_guard", 0.15}};

    // DEAD_CITY - pure undead
    enemyTemplates[LocationType::DEAD_CITY] = {
        // Skeleton - basic skeleton
        {"Skeleton", 40, 6, 1, 1, 1, 8, 0, AbilityType::NONE, 20, 1, "skeleton", 0.3},

        // Skeleton-warrior - armed skeleton
        {"Skeleton-warrior", 70, 10, 3, 2, 1, 10, 0, AbilityType::NONE, 35, 1, "skeleton_warrior", 0.25},

        // Vampire - classic vampire
        {"Vampire", 100, 14, 4, 3, 2, 14, 1, AbilityType::LIFE_STEAL, 65, 2, "vampire", 0.2},

        // Ghost - immaterial enemy
        {"Ghost", 60, 8, 0, 2, 1, 12, 1, AbilityType::INVISIBLE, 50, 2, "ghost", 0.4}};
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

    // Choose template randomly
    int index = rand() % templates.size();
    const EnemyTemplate &selected = templates[index];

    // Apply difficulty modifier
    int modifiedHP = selected.maxHP + (difficultyModifier * 20);
    int modifiedDamage = selected.damage + difficultyModifier;
    int modifiedDefense = selected.defense + difficultyModifier;
    int modifiedExp = selected.expValue + (difficultyModifier * 10);

    // Create enemy of appropriate type
    if (selected.name == "Goblin" || selected.type == "goblin")
    {
        return new Goblin(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                          selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                          selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
    else if (selected.name == "Orc" || selected.type == "orc")
    {
        return new Orc(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                       selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                       selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
    else if (selected.name == "Vampire" || selected.type == "vampire")
    {
        return new Vampire(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                           selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                           selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
    else if (selected.name == "Wyvern" || selected.type == "wyvern" || selected.name == "Wyvern-monarch" || selected.type == "wyvern_monarch")
    {
        return new Wyvern(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                          selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                          selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
    else if (selected.name == "Ghost" || selected.type == "ghost")
    {
        return new Ghost(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                         selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                         selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
    else if (selected.name == "Troglodyte" || selected.type == "troglodyte" || selected.name == "Infernal troglodyte" || selected.type == "infernal_troglodyte")
    {
        return new Troglodyte(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                              selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                              selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
    else
    {
        return new Enemy(selected.name, modifiedHP, modifiedDamage, modifiedDefense, selected.attack,
                         selected.maxStamina, selected.maxStamina, selected.initiative, selected.attackRange,
                         selected.ability, modifiedExp, selected.difficulty + difficultyModifier, selected.type, selected.damageVariance);
    }
}

Enemy *EnemyFactory::createEnemyByName(const std::string &name, int difficultyModifier)
{
    if (enemyTemplates.empty())
    {
        initializeTemplates();
    }

    // Search for enemy across all locations
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

    // If not found, return default enemy with random name
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
