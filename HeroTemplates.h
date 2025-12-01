#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <string>

// Hero template structure
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
    double damageVariance = 0.2; // Default damage variance
};

// Party preset structure
struct PartyPreset
{
    std::string name = "";
    std::string description = "";
    std::vector<std::pair<HeroClass, std::string>> heroes = {}; // Class and hero name
};

// Structure for ability description
struct AbilityInfo
{
    AbilityType type = AbilityType::NONE;
    std::string name = "";
    std::string description = "";
    int maxLevel = 0;
    std::string effect = "";
};

// Factory for creating heroes
class HeroFactory
{
private:
    static std::map<HeroClass, HeroTemplate> heroTemplates;
    static std::map<AbilityType, AbilityInfo> abilityDatabase;
    static std::vector<PartyPreset> partyPresets;

    static void initializeTemplates();
    static void initializeAbilities();
    static void initializePartyPresets();

public:
    // Get list of available classes
    static std::vector<HeroClass> getAvailableClasses();

    // Get class information
    static const HeroTemplate &getHeroTemplate(HeroClass heroClass);

    // Create hero by class
    static Player *createHero(HeroClass heroClass, const std::string &customName = "");

    // Get ability information
    static const AbilityInfo &getAbilityInfo(AbilityType ability);

    // Get list of abilities for class
    static std::vector<AbilityType> getClassAbilities(HeroClass heroClass);

    // Get list of party presets
    static const std::vector<PartyPreset> &getPartyPresets();

    // Create party by preset
    static std::vector<Player *> createPartyFromPreset(int presetIndex);
};
