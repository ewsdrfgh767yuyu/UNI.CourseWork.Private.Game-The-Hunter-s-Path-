#include "HeroTemplates.h"
#include "entity.h"
#include <vector>
#include <map>
#include <string>

// Initialize static members
std::map<HeroClass, HeroTemplate> HeroFactory::heroTemplates;
std::map<AbilityType, AbilityInfo> HeroFactory::abilityDatabase;
std::vector<PartyPreset> HeroFactory::partyPresets;

void HeroFactory::initializeTemplates()
{
    // WARRIOR - tanky melee fighter
    heroTemplates[HeroClass::WARRIOR] = {
        "Warrior", "Mighty melee fighter specializing in defense and powerful strikes", HeroClass::WARRIOR, ProgressionType::LEVEL_BASED, 120, 15, 5, 3, 2, 12, 0, AbilityType::CHARGE, {AbilityType::CHARGE, AbilityType::SHIELD_WALL, AbilityType::BATTLE_CRY, AbilityType::FEAR}, false, 0.15};

    // PALADIN - defender with healing
    heroTemplates[HeroClass::PALADIN] = {
        "Paladin", "Holy warrior combining defense, healing and ally support", HeroClass::PALADIN, ProgressionType::MASTERY, 130, 12, 8, 2, 2, 10, 0, AbilityType::HEALING_WAVE, {AbilityType::HEALING_WAVE, AbilityType::FROST_ARMOR, AbilityType::COMMAND, AbilityType::REGENERATION}, false, 0.1};

    // BARBARIAN - berserker with high damage
    heroTemplates[HeroClass::BARBARIAN] = {
        "Barbarian", "Wild warrior drawing strength from rage and fury", HeroClass::BARBARIAN, ProgressionType::SACRIFICE, 110, 18, 3, 4, 3, 14, 0, AbilityType::BERSERK, {AbilityType::BERSERK, AbilityType::BLOOD_RITUAL, AbilityType::LIFE_STEAL, AbilityType::SHADOW_STEP}, false, 0.4 // High variance - can deal a lot or almost nothing
    };

    // ROGUE - stealth and critical strikes
    heroTemplates[HeroClass::ROGUE] = {
        "Rogue", "Agile assassin specializing in stealth and surprise attacks", HeroClass::ROGUE, ProgressionType::SKILL_POINTS, 90, 14, 2, 3, 3, 16, 1, AbilityType::STEALTH, {AbilityType::STEALTH, AbilityType::SHADOW_STEP, AbilityType::HEAL, AbilityType::INVISIBLE}, false, 0.25};

    // RANGER - ranged combat and mobility
    heroTemplates[HeroClass::RANGER] = {
        "Ranger", "Master of bow and forest, combining ranged combat with mobility", HeroClass::RANGER, ProgressionType::LEVEL_BASED, 100, 13, 3, 3, 2, 15, 2, AbilityType::ARCANE_MISSILE, {AbilityType::ARCANE_MISSILE, AbilityType::TELEPORT, AbilityType::HEAL, AbilityType::FLAME_BURST}, false, 0.2};

    // MAGE - magical damage and control
    heroTemplates[HeroClass::MAGE] = {
        "Mage", "Master of magic, specializing in spells and battlefield control", HeroClass::MAGE, ProgressionType::SKILL_POINTS, 80, 8, 1, 2, 2, 11, 3, AbilityType::CHAIN_LIGHTNING, {AbilityType::CHAIN_LIGHTNING, AbilityType::TELEPORT, AbilityType::FLAME_BURST, AbilityType::FROST_ARMOR}, false, 0.05 // Almost no variance - mages shoot accurately
    };

    // WARLOCK - demonic magic and sacrifices
    heroTemplates[HeroClass::WARLOCK] = {
        "Warlock", "Dark mage drawing power from demonic deals and sacrifices", HeroClass::WARLOCK, ProgressionType::SACRIFICE, 85, 10, 2, 3, 2, 13, 2, AbilityType::BLOOD_RITUAL, {AbilityType::BLOOD_RITUAL, AbilityType::LIFE_STEAL, AbilityType::FEAR, AbilityType::ARCANE_MISSILE}, false, 0.3};

    // DRUID - nature magic and transformation
    heroTemplates[HeroClass::DRUID] = {
        "Druid", "Guardian of nature, wielding earth magic and healing", HeroClass::DRUID, ProgressionType::TRANSCENDENCE, 105, 11, 4, 2, 2, 12, 1, AbilityType::REGENERATION, {AbilityType::REGENERATION, AbilityType::HEALING_WAVE, AbilityType::POISON, AbilityType::STEALTH}, false};

    // LONER - unique class without party
    heroTemplates[HeroClass::LONER] = {
        "Loner", "Legendary hero going alone against the whole world. Enhanced stats, but no allies", HeroClass::LONER, ProgressionType::TRANSCENDENCE, 200, 25, 10, 8, 4, 18, 1, AbilityType::BERSERK, {AbilityType::BERSERK, AbilityType::CHARGE, AbilityType::SHIELD_WALL, AbilityType::HEALING_WAVE, AbilityType::TELEPORT}, true, 0.5 // Maximum variance - can deal a lot or almost nothing
    };
}

void HeroFactory::initializePartyPresets()
{
    // Preset 1: Lone - Legendary hero
    partyPresets.push_back({"Legendary Lone Hero",
                            "In dark times, when the shadows of the Lord of Darkness engulfed the kingdoms, a lone wanderer, marked by ancient curses and blessings, decided to stand against evil. His path is one of despair and greatness, where one sword decides the fate of the world.",
                            {{HeroClass::LONER, "Legend"}}});

    // Preset 2: Warrior and Mage - Classic pair
    partyPresets.push_back({"Brothers in Arms",
                            "Lotric, the unyielding warrior from the ruined castle, and Lorian, his mage brother, lost everything to the invasion of the forces of Darkness. Together they swore to avenge the Lord, whose servants burned their home and destroyed their family.",
                            {{HeroClass::WARRIOR, "Lotric"}, {HeroClass::MAGE, "Lorian"}}});

    // Preset 3: Barbarian and Rogue - Wild pair
    partyPresets.push_back({"Wild Avengers",
                            "Krag, barbarian from the northern lands, saw hordes of undead destroy his tribe. Laslo, cunning rogue from the slums, lost his sister in captivity to the Lord's servants. Together they sow chaos in the enemy ranks, avenging the lost.",
                            {{HeroClass::BARBARIAN, "Krag"}, {HeroClass::ROGUE, "Laslo"}}});

    // Preset 4: Barbarian, Rogue and Druid - Full trio
    partyPresets.push_back({"Guardians of the Ancient Forest",
                            "When the Lord of Darkness began poisoning the sacred forests, Krag, Laslo and Kendrick united. The barbarian carries the fury of nature, the rogue the cunning of shadows, and the druid the wisdom of ancient spirits. Their mission is to restore the balance of the world.",
                            {{HeroClass::BARBARIAN, "Krag"}, {HeroClass::ROGUE, "Laslo"}, {HeroClass::DRUID, "Kendrick"}}});

    // Preset 5: Paladin and Ranger - Defensive pair
    partyPresets.push_back({"Holy Wanderers",
                            "Eldrin, paladin of a forgotten order, lost faith when the Lord destroyed his temple. Talia, ranger from the elven forests, saw darkness devour her homeland. Together they carry the light of hope through the darkness.",
                            {{HeroClass::PALADIN, "Eldrin"}, {HeroClass::RANGER, "Talia"}}});

    // Preset 6: Mage and Warlock - Magical power
    partyPresets.push_back({"Masters of Forbidden Knowledge",
                            "Azrael and Morrigan, two mages from the ancient academy, witnessed the fall of their teachers to the power of the Lord of Darkness. One draws strength from pure magic, the other from dark deals. Their path leads to the final battle for the soul of magic.",
                            {{HeroClass::MAGE, "Azrael"}, {HeroClass::WARLOCK, "Morrigan"}}});

    // Preset 7: Warrior, Mage and Druid - Classic trio
    partyPresets.push_back({"Three Guardians of the Kingdom",
                            "Harold, the last knight of the fallen kingdom, Merilin, the court sorceress, and Sylvana, druid of the ancient circle - three whose fates intertwined in the fire of war. The Lord of Darkness destroyed their world, and now they go to restore it.",
                            {{HeroClass::WARRIOR, "Harold"}, {HeroClass::MAGE, "Merilin"}, {HeroClass::DRUID, "Sylvana"}}});

    // Preset 8: Rogue and Ranger - Stealthy pair
    partyPresets.push_back({"Shadows of Forgotten Forests",
                            "Shadow, master thief from the underworld, and Huntress, ranger from the mysterious forests, united after the Lord kidnapped their loved ones. They glide through the shadows, sowing death among the servants of Darkness.",
                            {{HeroClass::ROGUE, "Shadow"}, {HeroClass::RANGER, "Huntress"}}});

    // Preset 9: Paladin, Barbarian and Warlock - Balanced trio
    partyPresets.push_back({"Sacred Fury",
                            "Galahad, paladin rejected by his order for too radical methods; Grom, barbarian exiled from his tribe for uncontrollable rage; Sandro, warlock who sold his soul for power. The Lord of Darkness is their common enemy, uniting the outcasts.",
                            {{HeroClass::PALADIN, "Galahad"}, {HeroClass::BARBARIAN, "Grom"}, {HeroClass::WARLOCK, "Sandro"}}});

    // Preset 10: Full quartet - Warrior, Mage, Rogue, Druid
    partyPresets.push_back({"Legendary Quartet",
                            "Arthur, noble warrior; Merlin, wise mage; Robin, cunning rogue; Morgana, mysterious druid. Prophecy foretold their meeting in the hour of greatest need. The Lord of Darkness awakened ancient evil, and only this quartet can stop it.",
                            {{HeroClass::WARRIOR, "Arthur"}, {HeroClass::MAGE, "Merlin"}, {HeroClass::ROGUE, "Robin"}, {HeroClass::DRUID, "Morgana"}}});
}

void HeroFactory::initializeAbilities()
{
    abilityDatabase[AbilityType::CHARGE] = {
        AbilityType::CHARGE, "Charge", "Instant movement to target with powerful attack",
        5, "Мгновенно перемещается к цели и наносит мощный удар. Урон увеличивается на 50%, 30% шанс оглушить цель (инициатива -2) на 1 ход."};

    abilityDatabase[AbilityType::SHIELD_WALL] = {
        AbilityType::SHIELD_WALL, "Shield Wall", "Creates a powerful barrier blocking damage",
        3, "Создает мощный барьер, блокирующий урон. Защита +5, блокирует 80% урона на 2 хода."};

    abilityDatabase[AbilityType::BERSERK] = {
        AbilityType::BERSERK, "Berserk", "Enters rage, increasing strength",
        5, "Входит в ярость, увеличивая силу. Урон +5, защита -2 на 3 хода."};

    abilityDatabase[AbilityType::BATTLE_CRY] = {
        AbilityType::BATTLE_CRY, "Battle Cry", "Inspires allies and intimidates enemies",
        3, "Вдохновляет союзников и запугивает врагов. Союзники: атака и защита +2 на 2 хода, Враги: атака и инициатива -2 на 2 хода."};

    abilityDatabase[AbilityType::HEALING_WAVE] = {
        AbilityType::HEALING_WAVE, "Healing Wave", "Heals all allies with a wave of healing energy",
        4, "Исцеляет всех союзников волной целительной энергии. Восстанавливает 50 HP всем союзникам."};

    abilityDatabase[AbilityType::COMMAND] = {
        AbilityType::COMMAND, "Command", "Gives an order, boosting allies' morale",
        3, "Отдает приказ, повышая боевой дух союзников. Союзники: инициатива +2, урон +1 на 2 хода."};

    abilityDatabase[AbilityType::FROST_ARMOR] = {
        AbilityType::FROST_ARMOR, "Frost Armor", "Covers with ice armor, slowing enemies",
        3, "Покрывает ледяной броней, замедляя врагов. Защита +5 на 3 хода, враги: инициатива -2 на 2 хода."};

    abilityDatabase[AbilityType::STEALTH] = {
        AbilityType::STEALTH, "Stealth", "Blends with shadows for surprise attack",
        4, "Сливается с тенями для внезапной атаки. Невидим, следующая атака: урон x2."};

    abilityDatabase[AbilityType::SHADOW_STEP] = {
        AbilityType::SHADOW_STEP, "Shadow Step", "Teleports through shadows for surprise strike",
        3, "Телепортируется сквозь тени для внезапного удара. Телепорт к цели, урон x2 (игнорирует защиту)."};

    abilityDatabase[AbilityType::POISON] = {
        AbilityType::POISON, "Poison", "Poisons target with damage over time",
        4, "Отравляет цель уроном со временем. Цель получает 8 урона в начале каждого хода на 3 хода."};

    abilityDatabase[AbilityType::INVISIBLE] = {
        AbilityType::INVISIBLE, "Invisible", "Becomes completely invisible",
        2, "Становится полностью невидимым. Невидим до следующей атаки или способности."};

    abilityDatabase[AbilityType::ARCANE_MISSILE] = {
        AbilityType::ARCANE_MISSILE, "Arcane Missile", "Launches a powerful magic missile",
        5, "Запускает мощную магическую ракету. Урон 15-25, игнорирует защиту цели."};

    abilityDatabase[AbilityType::TELEPORT] = {
        AbilityType::TELEPORT, "Teleport", "Instantly moves to a specified point",
        3, "Мгновенно перемещается в указанную точку. Перемещение на любую свободную позицию."};

    abilityDatabase[AbilityType::CHAIN_LIGHTNING] = {
        AbilityType::CHAIN_LIGHTNING, "Chain Lightning", "Calls lightning striking multiple targets",
        4, "Призывает молнию, поражающую несколько целей. 20 урона 3 ближайшим врагам."};

    abilityDatabase[AbilityType::FLAME_BURST] = {
        AbilityType::FLAME_BURST, "Flame Burst", "Creates a flame explosion around self",
        3, "Создает огненный взрыв вокруг себя. 25 урона всем врагам."};

    abilityDatabase[AbilityType::BLOOD_RITUAL] = {
        AbilityType::BLOOD_RITUAL, "Blood Ritual", "Sacrifices blood for demonic power",
        3, "Жертвует кровью за демоническую силу. Жертвует 30 HP, урон +50% на 3 хода."};

    abilityDatabase[AbilityType::LIFE_STEAL] = {
        AbilityType::LIFE_STEAL, "Life Steal", "Steals life force from enemies",
        4, "Крадет жизненную силу у врагов. Восстанавливает 50% нанесенного урона."};

    abilityDatabase[AbilityType::FEAR] = {
        AbilityType::FEAR, "Fear", "Instills terror, reducing enemies' combat ability",
        3, "Внушает ужас, снижая боеспособность врагов. Враги: инициатива -2, урон -3 на 2 хода."};

    abilityDatabase[AbilityType::HEAL] = {
        AbilityType::HEAL, "Heal", "Restores health instantly",
        4, "Восстанавливает здоровье мгновенно. Восстанавливает 40 HP."};

    abilityDatabase[AbilityType::REGENERATION] = {
        AbilityType::REGENERATION, "Regeneration", "Restores health naturally",
        5, "Восстанавливает здоровье естественным образом. Восстанавливает 30 HP в начале хода."};

    abilityDatabase[AbilityType::FLYING] = {
        AbilityType::FLYING, "Flying", "Takes off and moves through air",
        4, "Взлетает и перемещается по воздуху. Перемещение на любую позицию."};

    abilityDatabase[AbilityType::FIRE_DAMAGE] = {
        AbilityType::FIRE_DAMAGE, "Fire Damage", "Attacks deal additional fire damage",
        4, "Атаки наносят дополнительный огненный урон. Дополнительный урон 30% от базового за атаку."};
}

std::vector<HeroClass> HeroFactory::getAvailableClasses()
{
    if (heroTemplates.empty())
    {
        initializeTemplates();
    }

    std::vector<HeroClass> classes;
    for (const auto &pair : heroTemplates)
    {
        classes.push_back(pair.first);
    }
    return classes;
}

const HeroTemplate &HeroFactory::getHeroTemplate(HeroClass heroClass)
{
    if (heroTemplates.empty())
    {
        initializeTemplates();
    }

    return heroTemplates[heroClass];
}

Player *HeroFactory::createHero(HeroClass heroClass, const std::string &customName)
{
    if (heroTemplates.empty())
    {
        initializeTemplates();
    }

    const HeroTemplate &tmpl = heroTemplates[heroClass];
    std::string heroName = customName.empty() ? tmpl.name : customName;

    Player *hero = new Player(
        heroName,
        tmpl.baseMaxHP,
        tmpl.baseDamage,
        tmpl.baseDefense,
        tmpl.baseAttack,
        tmpl.baseMaxStamina,
        tmpl.baseMaxStamina,
        tmpl.baseInitiative,
        tmpl.baseAttackRange,
        1,    // level
        1000, // required exp
        0,    // received exp
        heroClass,
        tmpl.progressionType,
        tmpl.baseAbility,
        tmpl.damageVariance);

    // Set additional properties through methods
    hero->setAvailableAbilities(tmpl.availableAbilities);
    hero->setProgressionType(tmpl.progressionType);
    hero->setIsLoner(tmpl.isLoner);

    // Initialize ability levels
    for (AbilityType ability : tmpl.availableAbilities)
    {
        hero->setAbilityLevel(ability, 1); // Base level
    }

    return hero;
}

const AbilityInfo &HeroFactory::getAbilityInfo(AbilityType ability)
{
    if (abilityDatabase.empty())
    {
        initializeAbilities();
    }

    return abilityDatabase[ability];
}

std::vector<AbilityType> HeroFactory::getClassAbilities(HeroClass heroClass)
{
    if (heroTemplates.empty())
    {
        initializeTemplates();
    }

    return heroTemplates[heroClass].availableAbilities;
}

const std::vector<PartyPreset> &HeroFactory::getPartyPresets()
{
    if (partyPresets.empty())
    {
        initializePartyPresets();
    }

    return partyPresets;
}

std::vector<Player *> HeroFactory::createPartyFromPreset(int presetIndex)
{
    if (partyPresets.empty())
    {
        initializePartyPresets();
    }

    if (presetIndex < 0 || presetIndex >= static_cast<int>(partyPresets.size()))
    {
        return {};
    }

    const PartyPreset &preset = partyPresets[presetIndex];
    std::vector<Player *> party;

    for (const auto &heroPair : preset.heroes)
    {
        Player *hero = createHero(heroPair.first, heroPair.second);
        if (hero)
        {
            party.push_back(hero);
        }
    }

    return party;
}
