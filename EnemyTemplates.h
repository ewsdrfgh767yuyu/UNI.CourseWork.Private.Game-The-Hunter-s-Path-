#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <random>
#include <ctime>

// Структура шаблона врага
struct EnemyTemplate
{
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
    double damageVariance = 0.2; // Разброс урона по умолчанию
};

// Фабрика для создания врагов по шаблонам
class EnemyFactory
{
private:
    static std::map<LocationType, std::vector<EnemyTemplate>> enemyTemplates;
    static bool initialized;

    static void initializeTemplates();

public:
    // Получить случайного врага для локации
    static Enemy *createRandomEnemy(LocationType location, int difficultyModifier = 0);

    // Получить конкретного врага по имени
    static Enemy *createEnemyByName(const std::string &name, int difficultyModifier = 0);

    // Получить список доступных врагов для локации
    static std::vector<std::string> getAvailableEnemies(LocationType location);
};
