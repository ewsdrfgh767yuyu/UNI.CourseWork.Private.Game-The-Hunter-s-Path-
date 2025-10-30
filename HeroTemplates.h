#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <string>

// Структура шаблона героя
struct HeroTemplate
{
    std::string name = "";
    std::string description = "";
    HeroClass heroClass = HeroClass::WARRIOR;
    ProgressionType progressionType = ProgressionType::LEVEL_BASED;
    int baseMaxHP = 0;
    int baseDamage = 0;
    int baseDefense = 0;
    int baseAttack = 0;
    int baseMaxStamina = 0;
    int baseInitiative = 0;
    int baseAttackRange = 0;
    AbilityType baseAbility = AbilityType::NONE;
    std::vector<AbilityType> availableAbilities = {};
    bool isLoner = false;
    double damageVariance = 0.2; // Разброс урона по умолчанию
};

// Структура для описания способности
struct AbilityInfo
{
    AbilityType type = AbilityType::NONE;
    std::string name = "";
    std::string description = "";
    int maxLevel = 0;
    std::string effect = "";
};

// Фабрика для создания героев
class HeroFactory
{
private:
    static std::map<HeroClass, HeroTemplate> heroTemplates;
    static std::map<AbilityType, AbilityInfo> abilityDatabase;

    static void initializeTemplates();
    static void initializeAbilities();

public:
    // Получить список доступных классов
    static std::vector<HeroClass> getAvailableClasses();

    // Получить информацию о классе
    static const HeroTemplate &getHeroTemplate(HeroClass heroClass);

    // Создать героя по классу
    static Player *createHero(HeroClass heroClass, const std::string &customName = "");

    // Получить информацию о способности
    static const AbilityInfo &getAbilityInfo(AbilityType ability);

    // Получить список способностей для класса
    static std::vector<AbilityType> getClassAbilities(HeroClass heroClass);
};
