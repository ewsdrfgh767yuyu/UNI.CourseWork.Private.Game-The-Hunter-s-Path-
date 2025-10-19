#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <string>

// Структура шаблона героя
struct HeroTemplate {
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
struct AbilityInfo {
    AbilityType type = AbilityType::NONE;
    std::string name = "";
    std::string description = "";
    int maxLevel = 0;
    std::string effect = "";
};

// Фабрика для создания героев
class HeroFactory {
private:
    static std::map<HeroClass, HeroTemplate> heroTemplates;
    static std::map<AbilityType, AbilityInfo> abilityDatabase;

    static void initializeTemplates();
    static void initializeAbilities();

public:
    // Получить список доступных классов
    static std::vector<HeroClass> getAvailableClasses();

    // Получить информацию о классе
    static const HeroTemplate& getHeroTemplate(HeroClass heroClass);

    // Создать героя по классу
    static Player* createHero(HeroClass heroClass, const std::string& customName = "");

    // Получить информацию о способности
    static const AbilityInfo& getAbilityInfo(AbilityType ability);

    // Получить список способностей для класса
    static std::vector<AbilityType> getClassAbilities(HeroClass heroClass);
};

// Инициализация шаблонов героев
std::map<HeroClass, HeroTemplate> HeroFactory::heroTemplates;
std::map<AbilityType, AbilityInfo> HeroFactory::abilityDatabase;

void HeroFactory::initializeTemplates() {
    // ВОИН - танкующий боец ближнего боя
    heroTemplates[HeroClass::WARRIOR] = {
        "Воин", "Могучий боец ближнего боя, специализирующийся на защите и мощных ударах",
        HeroClass::WARRIOR, ProgressionType::LEVEL_BASED,
        120, 15, 5, 3, 2, 12, 0, AbilityType::CHARGE,
        {AbilityType::CHARGE, AbilityType::SHIELD_WALL, AbilityType::BERSERK, AbilityType::BATTLE_CRY},
        false, 0.15
    };

    // ПАЛАДИН - защитник с лечением
    heroTemplates[HeroClass::PALADIN] = {
        "Паладин", "Святой воин, сочетающий защиту, лечение и поддержку союзников",
        HeroClass::PALADIN, ProgressionType::MASTERY,
        130, 12, 8, 2, 2, 10, 0, AbilityType::HEALING_WAVE,
        {AbilityType::HEALING_WAVE, AbilityType::SHIELD_WALL, AbilityType::COMMAND, AbilityType::FROST_ARMOR},
        false, 0.1
    };

    // ВАРВАР - берсерк с высоким уроном
    heroTemplates[HeroClass::BARBARIAN] = {
        "Варвар", "Дикий воин, черпающий силу из ярости и неистовства",
        HeroClass::BARBARIAN, ProgressionType::SACRIFICE,
        110, 18, 3, 4, 3, 14, 0, AbilityType::BERSERK,
        {AbilityType::BERSERK, AbilityType::CHARGE, AbilityType::BATTLE_CRY, AbilityType::BLOOD_RITUAL},
        false, 0.4 // Высокий разброс - может нанести очень много или почти ничего
    };

    // РАЗБОЙНИК - stealth и критические удары
    heroTemplates[HeroClass::ROGUE] = {
        "Разбойник", "Ловкий убийца, специализирующийся на скрытности и внезапных атаках",
        HeroClass::ROGUE, ProgressionType::SKILL_POINTS,
        90, 14, 2, 3, 3, 16, 1, AbilityType::STEALTH,
        {AbilityType::STEALTH, AbilityType::SHADOW_STEP, AbilityType::POISON, AbilityType::INVISIBLE},
        false, 0.25
    };

    // СЛЕДОПЫТ - дальний бой и мобильность
    heroTemplates[HeroClass::RANGER] = {
        "Следопыт", "Мастер лука и леса, сочетающий дальний бой с мобильностью",
        HeroClass::RANGER, ProgressionType::LEVEL_BASED,
        100, 13, 3, 3, 2, 15, 2, AbilityType::NONE,
        {AbilityType::ARCANE_MISSILE, AbilityType::TELEPORT, AbilityType::COMMAND, AbilityType::HEAL},
        false, 0.2
    };

    // МАГ - магический урон и контроль
    heroTemplates[HeroClass::MAGE] = {
        "Маг", "Повелитель магии, специализирующийся на заклинаниях и контроле поля боя",
        HeroClass::MAGE, ProgressionType::SKILL_POINTS,
        80, 8, 1, 2, 2, 11, 3, AbilityType::ARCANE_MISSILE,
        {AbilityType::ARCANE_MISSILE, AbilityType::CHAIN_LIGHTNING, AbilityType::FLAME_BURST, AbilityType::TELEPORT},
        false, 0.05 // Почти без разброса - маги стреляют точно
    };

    // КОЛДУН - демоническая магия и жертвы
    heroTemplates[HeroClass::WARLOCK] = {
        "Колдун", "Темный маг, черпающий силу из демонических сделок и жертв",
        HeroClass::WARLOCK, ProgressionType::SACRIFICE,
        85, 10, 2, 3, 2, 13, 2, AbilityType::BLOOD_RITUAL,
        {AbilityType::BLOOD_RITUAL, AbilityType::LIFE_STEAL, AbilityType::FEAR, AbilityType::CHAIN_LIGHTNING},
        false, 0.3
    };

    // ДРУИД - природная магия и трансформация
    heroTemplates[HeroClass::DRUID] = {
        "Друид", "Страж природы, владеющий магией земли и исцелением",
        HeroClass::DRUID, ProgressionType::TRANSCENDENCE,
        105, 11, 4, 2, 2, 12, 1, AbilityType::REGENERATION,
        {AbilityType::REGENERATION, AbilityType::HEALING_WAVE, AbilityType::POISON, AbilityType::TELEPORT},
        false
    };

    // ОДИНОЧКА - уникальный класс без отряда
    heroTemplates[HeroClass::LONER] = {
        "Одиночка", "Легендарный герой, идущий один против всего мира. Повышенные характеристики, но без союзников",
        HeroClass::LONER, ProgressionType::TRANSCENDENCE,
        200, 25, 10, 8, 4, 18, 1, AbilityType::BERSERK,
        {AbilityType::BERSERK, AbilityType::CHARGE, AbilityType::SHIELD_WALL, AbilityType::HEALING_WAVE, AbilityType::TELEPORT},
        true, 0.5 // Максимальный разброс - может нанести очень много или почти ничего
    };
}

void HeroFactory::initializeAbilities() {
    abilityDatabase[AbilityType::CHARGE] = {
        AbilityType::CHARGE, "Рывок", "Мгновенное перемещение к цели с последующей атакой",
        5, "Урон +50%, шанс оглушения"
    };

    abilityDatabase[AbilityType::SHIELD_WALL] = {
        AbilityType::SHIELD_WALL, "Стена щитов", "Создает барьер, блокирующий входящий урон",
        3, "Блокирует 80% урона на 2 хода"
    };

    abilityDatabase[AbilityType::BERSERK] = {
        AbilityType::BERSERK, "Берсерк", "Ярость увеличивает силу при низком здоровье",
        5, "Урон +30% при HP < 50%"
    };

    abilityDatabase[AbilityType::BATTLE_CRY] = {
        AbilityType::BATTLE_CRY, "Боевой клич", "Воодушевляет союзников и пугает врагов",
        3, "Бафф союзников +20%, страх врагов"
    };

    abilityDatabase[AbilityType::HEALING_WAVE] = {
        AbilityType::HEALING_WAVE, "Волна исцеления", "Исцеляет всех союзников в радиусе",
        4, "Исцеление 50 HP всем союзникам"
    };

    abilityDatabase[AbilityType::COMMAND] = {
        AbilityType::COMMAND, "Команда", "Усиливает способности союзников",
        3, "Бафф: +2 инициатива, +1 урон"
    };

    abilityDatabase[AbilityType::FROST_ARMOR] = {
        AbilityType::FROST_ARMOR, "Ледяная броня", "Защита + замедление атакующих",
        3, "Защита +5, замедление врагов"
    };

    abilityDatabase[AbilityType::STEALTH] = {
        AbilityType::STEALTH, "Скрытность", "Становится невидимым для внезапной атаки",
        4, "Невидимость + критический удар x2"
    };

    abilityDatabase[AbilityType::SHADOW_STEP] = {
        AbilityType::SHADOW_STEP, "Теневой шаг", "Телепортация в тень для внезапной атаки",
        3, "Телепортация + гарантированный удар"
    };

    abilityDatabase[AbilityType::POISON] = {
        AbilityType::POISON, "Яд", "Отравляет цель, нанося урон со временем",
        4, "Урон 10 HP/ход в течение 3 ходов"
    };

    abilityDatabase[AbilityType::INVISIBLE] = {
        AbilityType::INVISIBLE, "Невидимость", "Становится полностью невидимым",
        2, "Невидим до следующей атаки"
    };

    abilityDatabase[AbilityType::ARCANE_MISSILE] = {
        AbilityType::ARCANE_MISSILE, "Магический снаряд", "Запускает магический снаряд в цель",
        5, "Урон 15-25, игнорирует защиту"
    };

    abilityDatabase[AbilityType::TELEPORT] = {
        AbilityType::TELEPORT, "Телепортация", "Мгновенное перемещение на поле боя",
        3, "Перемещение в любую позицию"
    };

    abilityDatabase[AbilityType::CHAIN_LIGHTNING] = {
        AbilityType::CHAIN_LIGHTNING, "Цепная молния", "Молния поражает несколько целей",
        4, "Урон 20, цепная реакция на 3 цели"
    };

    abilityDatabase[AbilityType::FLAME_BURST] = {
        AbilityType::FLAME_BURST, "Взрыв пламени", "Огненный взрыв поражает область",
        3, "Урон 25 по области 3x3"
    };

    abilityDatabase[AbilityType::BLOOD_RITUAL] = {
        AbilityType::BLOOD_RITUAL, "Кровавый ритуал", "Жертва HP за временную силу",
        3, "Жертва 30 HP за урон +50% на 3 хода"
    };

    abilityDatabase[AbilityType::LIFE_STEAL] = {
        AbilityType::LIFE_STEAL, "Похищение жизни", "Часть урона восстанавливает здоровье",
        4, "Восстановление 50% от нанесенного урона"
    };

    abilityDatabase[AbilityType::FEAR] = {
        AbilityType::FEAR, "Страх", "Пугаеет врага, снижая его эффективность",
        3, "Снижение инициативы и урона на 50%"
    };

    abilityDatabase[AbilityType::REGENERATION] = {
        AbilityType::REGENERATION, "Регенерация", "Восстанавливает здоровье со временем",
        5, "Восстановление 5 HP/ход"
    };
}

std::vector<HeroClass> HeroFactory::getAvailableClasses() {
    if (heroTemplates.empty()) {
        initializeTemplates();
    }

    std::vector<HeroClass> classes;
    for (const auto& pair : heroTemplates) {
        classes.push_back(pair.first);
    }
    return classes;
}

const HeroTemplate& HeroFactory::getHeroTemplate(HeroClass heroClass) {
    if (heroTemplates.empty()) {
        initializeTemplates();
    }

    return heroTemplates[heroClass];
}

Player* HeroFactory::createHero(HeroClass heroClass, const std::string& customName) {
    if (heroTemplates.empty()) {
        initializeTemplates();
    }

    const HeroTemplate& tmpl = heroTemplates[heroClass];
    std::string heroName = customName.empty() ? tmpl.name : customName;

    Player* hero = new Player(
        heroName,
        tmpl.baseMaxHP,
        tmpl.baseDamage,
        tmpl.baseDefense,
        tmpl.baseAttack,
        tmpl.baseMaxStamina,
        tmpl.baseMaxStamina,
        tmpl.baseInitiative,
        tmpl.baseAttackRange,
        1, // level
        1000, // required exp
        0, // received exp
        heroClass,
        tmpl.progressionType,
        tmpl.baseAbility,
        tmpl.damageVariance
    );

    // Устанавливаем дополнительные свойства через методы
    hero->setAvailableAbilities(tmpl.availableAbilities);
    hero->setProgressionType(tmpl.progressionType);
    hero->setIsLoner(tmpl.isLoner);

    // Инициализируем уровни способностей
    for (AbilityType ability : tmpl.availableAbilities) {
        hero->setAbilityLevel(ability, 1); // Базовый уровень
    }

    return hero;
}

const AbilityInfo& HeroFactory::getAbilityInfo(AbilityType ability) {
    if (abilityDatabase.empty()) {
        initializeAbilities();
    }

    return abilityDatabase[ability];
}

std::vector<AbilityType> HeroFactory::getClassAbilities(HeroClass heroClass) {
    if (heroTemplates.empty()) {
        initializeTemplates();
    }

    return heroTemplates[heroClass].availableAbilities;
}