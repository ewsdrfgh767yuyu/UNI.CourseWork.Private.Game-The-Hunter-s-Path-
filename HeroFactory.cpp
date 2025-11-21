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
    // ВОИН - танкующий боец ближнего боя
    heroTemplates[HeroClass::WARRIOR] = {
        "Воин", "Могучий боец ближнего боя, специализирующийся на защите и мощных ударах", HeroClass::WARRIOR, ProgressionType::LEVEL_BASED, 120, 15, 5, 3, 2, 12, 0, AbilityType::CHARGE, {AbilityType::CHARGE, AbilityType::SHIELD_WALL, AbilityType::BATTLE_CRY, AbilityType::FEAR}, false, 0.15};

    // ПАЛАДИН - защитник с лечением
    heroTemplates[HeroClass::PALADIN] = {
        "Паладин", "Святой воин, сочетающий защиту, лечение и поддержку союзников", HeroClass::PALADIN, ProgressionType::MASTERY, 130, 12, 8, 2, 2, 10, 0, AbilityType::HEALING_WAVE, {AbilityType::HEALING_WAVE, AbilityType::FROST_ARMOR, AbilityType::COMMAND, AbilityType::REGENERATION}, false, 0.1};

    // ВАРВАР - берсерк с высоким уроном
    heroTemplates[HeroClass::BARBARIAN] = {
        "Варвар", "Дикий воин, черпающий силу из ярости и неистовства", HeroClass::BARBARIAN, ProgressionType::SACRIFICE, 110, 18, 3, 4, 3, 14, 0, AbilityType::BERSERK, {AbilityType::BERSERK, AbilityType::BLOOD_RITUAL, AbilityType::LIFE_STEAL, AbilityType::SHADOW_STEP}, false, 0.4 // Высокий разброс - может нанести очень много или почти ничего
    };

    // РАЗБОЙНИК - stealth и критические удары
    heroTemplates[HeroClass::ROGUE] = {
        "Разбойник", "Ловкий убийца, специализирующийся на скрытности и внезапных атаках", HeroClass::ROGUE, ProgressionType::SKILL_POINTS, 90, 14, 2, 3, 3, 16, 1, AbilityType::STEALTH, {AbilityType::STEALTH, AbilityType::SHADOW_STEP, AbilityType::HEAL, AbilityType::INVISIBLE}, false, 0.25};

    // СЛЕДОПЫТ - дальний бой и мобильность
    heroTemplates[HeroClass::RANGER] = {
        "Следопыт", "Мастер лука и леса, сочетающий дальний бой с мобильностью", HeroClass::RANGER, ProgressionType::LEVEL_BASED, 100, 13, 3, 3, 2, 15, 2, AbilityType::ARCANE_MISSILE, {AbilityType::ARCANE_MISSILE, AbilityType::TELEPORT, AbilityType::HEAL, AbilityType::FLAME_BURST}, false, 0.2};

    // МАГ - магический урон и контроль
    heroTemplates[HeroClass::MAGE] = {
        "Маг", "Повелитель магии, специализирующийся на заклинаниях и контроле поля боя", HeroClass::MAGE, ProgressionType::SKILL_POINTS, 80, 8, 1, 2, 2, 11, 3, AbilityType::CHAIN_LIGHTNING, {AbilityType::CHAIN_LIGHTNING, AbilityType::TELEPORT, AbilityType::FLAME_BURST, AbilityType::FROST_ARMOR}, false, 0.05 // Почти без разброса - маги стреляют точно
    };

    // КОЛДУН - демоническая магия и жертвы
    heroTemplates[HeroClass::WARLOCK] = {
        "Колдун", "Темный маг, черпающий силу из демонических сделок и жертв", HeroClass::WARLOCK, ProgressionType::SACRIFICE, 85, 10, 2, 3, 2, 13, 2, AbilityType::BLOOD_RITUAL, {AbilityType::BLOOD_RITUAL, AbilityType::LIFE_STEAL, AbilityType::FEAR, AbilityType::ARCANE_MISSILE}, false, 0.3};

    // ДРУИД - природная магия и трансформация
    heroTemplates[HeroClass::DRUID] = {
        "Друид", "Страж природы, владеющий магией земли и исцелением", HeroClass::DRUID, ProgressionType::TRANSCENDENCE, 105, 11, 4, 2, 2, 12, 1, AbilityType::REGENERATION, {AbilityType::REGENERATION, AbilityType::HEALING_WAVE, AbilityType::POISON, AbilityType::STEALTH}, false};

    // ОДИНОЧКА - уникальный класс без отряда
    heroTemplates[HeroClass::LONER] = {
        "Одиночка", "Легендарный герой, идущий один против всего мира. Повышенные характеристики, но без союзников", HeroClass::LONER, ProgressionType::TRANSCENDENCE, 200, 25, 10, 8, 4, 18, 1, AbilityType::BERSERK, {AbilityType::BERSERK, AbilityType::CHARGE, AbilityType::SHIELD_WALL, AbilityType::HEALING_WAVE, AbilityType::TELEPORT}, true, 0.5 // Максимальный разброс - может нанести очень много или почти ничего
    };
}

void HeroFactory::initializePartyPresets()
{
    // Пресет 1: Одиночка - Легендарный герой
    partyPresets.push_back({
        "Легендарный Одиночка",
        "В темные времена, когда тени Властелина Тьмы поглотили королевства, одинокий странник, отмеченный древними проклятиями и благословениями, решил встать против зла. Его путь - это путь отчаяния и величия, где один меч решает судьбу мира.",
        {{HeroClass::LONER, "Легенда"}}
    });

    // Пресет 2: Воин и Маг - Классическая пара
    partyPresets.push_back({
        "Братья по Оружию",
        "Лотрик, непреклонный воин из разрушенного замка, и Лориан, его брат-маг, потеряли все из-за нашествия сил Тьмы. Вместе они клялись отомстить Властелину, чьи слуги сожгли их родной дом и погубили их семью.",
        {{HeroClass::WARRIOR, "Лотрик"}, {HeroClass::MAGE, "Лориан"}}
    });

    // Пресет 3: Варвар и Разбойник - Дикая пара
    partyPresets.push_back({
        "Дикие Мстители",
        "Краг, варвар из северных земель, видел, как орды нежити уничтожили его племя. Ласло, хитрый разбойник из трущоб, потерял сестру в плену у слуг Властелина. Вместе они сеют хаос в рядах врага, мстя за утраченное.",
        {{HeroClass::BARBARIAN, "Краг"}, {HeroClass::ROGUE, "Ласло"}}
    });

    // Пресет 4: Варвар, Разбойник и Друид - Полная тройка
    partyPresets.push_back({
        "Стражи Древнего Леса",
        "Когда Властелин Тьмы начал отравлять священные леса, Краг, Ласло и Кендрик объединились. Варвар несет ярость природы, разбойник - хитрость теней, а друид - мудрость древних духов. Их миссия - восстановить баланс мира.",
        {{HeroClass::BARBARIAN, "Краг"}, {HeroClass::ROGUE, "Ласло"}, {HeroClass::DRUID, "Кендрик"}}
    });

    // Пресет 5: Паладин и Следопыт - Защитная пара
    partyPresets.push_back({
        "Святые Странники",
        "Элдрин, паладин забытого ордена, потерял веру, когда Властелин уничтожил его храм. Талия, следопыт из эльфийских лесов, видела, как тьма пожирает ее родину. Вместе они несут свет надежды сквозь мрак.",
        {{HeroClass::PALADIN, "Элдрин"}, {HeroClass::RANGER, "Талия"}}
    });

    // Пресет 6: Маг и Колдун - Магическая мощь
    partyPresets.push_back({
        "Повелители Запретного Знания",
        "Азраэль и Морриган, два мага из древней академии, были свидетелями падения их учителей под властью Властелина Тьмы. Один черпает силу из чистой магии, другой - из темных сделок. Их путь ведет к финальной битве за душу магии.",
        {{HeroClass::MAGE, "Азраэль"}, {HeroClass::WARLOCK, "Морриган"}}
    });

    // Пресет 7: Воин, Маг и Друид - Классическая тройка
    partyPresets.push_back({
        "Три Стража Королевства",
        "Гарольд, последний рыцарь павшего королевства, Мерилин, придворная волшебница, и Сильвана, друид древнего круга - трое, чьи судьбы переплелись в огне войны. Властелин Тьмы разрушил их мир, и теперь они идут, чтобы восстановить его.",
        {{HeroClass::WARRIOR, "Гарольд"}, {HeroClass::MAGE, "Мерилин"}, {HeroClass::DRUID, "Сильвана"}}
    });

    // Пресет 8: Разбойник и Следопыт - Скрытная пара
    partyPresets.push_back({
        "Тени Забытых Лесов",
        "Шэдоу, мастер воровства из подземного мира, и Хантресс, следопыт из таинственных лесов, объединились после того, как Властелин похитил их близких. Они скользят сквозь тени, сея смерть среди слуг Тьмы.",
        {{HeroClass::ROGUE, "Шэдоу"}, {HeroClass::RANGER, "Хантресс"}}
    });

    // Пресет 9: Паладин, Варвар и Колдун - Сбалансированная тройка
    partyPresets.push_back({
        "Священная Ярость",
        "Галахад, паладин, отвергнутый своим орденом за слишком радикальные методы; Гром, варвар, изгнанный из племени за неконтролируемую ярость; Сандро, колдун, продавший душу за силу. Властелин Тьмы - их общий враг, объединивший отверженных.",
        {{HeroClass::PALADIN, "Галахад"}, {HeroClass::BARBARIAN, "Гром"}, {HeroClass::WARLOCK, "Сандро"}}
    });

    // Пресет 10: Полная четверка - Воин, Маг, Разбойник, Друид
    partyPresets.push_back({
        "Легендарная Четверка",
        "Артур, благородный воин; Мерлин, мудрый маг; Робин, хитрый разбойник; Моргана, таинственная друидка. Пророчество предсказало их встречу в час величайшей нужды. Властелин Тьмы пробудил древнее зло, и только эта четверка может остановить его.",
        {{HeroClass::WARRIOR, "Артур"}, {HeroClass::MAGE, "Мерлин"}, {HeroClass::ROGUE, "Робин"}, {HeroClass::DRUID, "Моргана"}}
    });
}

void HeroFactory::initializeAbilities()
{
    abilityDatabase[AbilityType::CHARGE] = {
        AbilityType::CHARGE, "Рывок", "Мгновенное перемещение к цели с мощной атакой",
        5, "Урон +50%, 30% шанс оглушения (-2 инициативы) на 1 ход"};

    abilityDatabase[AbilityType::SHIELD_WALL] = {
        AbilityType::SHIELD_WALL, "Стена щитов", "Создает мощный барьер, блокирующий урон",
        3, "Защита +5, блокирует 80% урона на 2 хода"};

    abilityDatabase[AbilityType::BERSERK] = {
        AbilityType::BERSERK, "Берсерк", "Впадает в ярость, увеличивая силу",
        5, "Урон +5, защита -2 на 3 хода"};

    abilityDatabase[AbilityType::BATTLE_CRY] = {
        AbilityType::BATTLE_CRY, "Боевой клич", "Воодушевляет союзников и устрашает врагов",
        3, "Союзники: +2 атаки и защиты на 2 хода, Враги: -2 атаки и инициативы на 2 хода"};

    abilityDatabase[AbilityType::HEALING_WAVE] = {
        AbilityType::HEALING_WAVE, "Волна исцеления", "Исцеляет всех союзников волной целительной энергии",
        4, "Исцеление 50 HP всем союзникам"};

    abilityDatabase[AbilityType::COMMAND] = {
        AbilityType::COMMAND, "Команда", "Отдает приказ, усиливая боевой дух союзников",
        3, "Союзники: +2 инициативы, +1 урона на 2 хода"};

    abilityDatabase[AbilityType::FROST_ARMOR] = {
        AbilityType::FROST_ARMOR, "Ледяная броня", "Покрывает ледяной броней, замедляя врагов",
        3, "Защита +5 на 3 хода, враги: -2 инициативы на 2 хода"};

    abilityDatabase[AbilityType::STEALTH] = {
        AbilityType::STEALTH, "Скрытность", "Сливается с тенями для внезапной атаки",
        4, "Невидимость, следующая атака: x2 урон"};

    abilityDatabase[AbilityType::SHADOW_STEP] = {
        AbilityType::SHADOW_STEP, "Теневой шаг", "Телепортируется сквозь тени для внезапного удара",
        3, "Телепортация к цели, урон x2 (игнорирует защиту)"};

    abilityDatabase[AbilityType::POISON] = {
        AbilityType::POISON, "Яд", "Отравляет цель ядом, наносящим урон со временем",
        4, "Цель получает 8 урона в начале каждого своего хода в течение 3 ходов"};

    abilityDatabase[AbilityType::INVISIBLE] = {
        AbilityType::INVISIBLE, "Невидимость", "Становится полностью невидимым",
        2, "Невидим до следующей атаки или способности"};

    abilityDatabase[AbilityType::ARCANE_MISSILE] = {
        AbilityType::ARCANE_MISSILE, "Магический снаряд", "Запускает мощный магический снаряд",
        5, "Урон 15-25, игнорирует защиту цели"};

    abilityDatabase[AbilityType::TELEPORT] = {
        AbilityType::TELEPORT, "Телепортация", "Мгновенно перемещается в указанную точку",
        3, "Перемещение на любую свободную позицию"};

    abilityDatabase[AbilityType::CHAIN_LIGHTNING] = {
        AbilityType::CHAIN_LIGHTNING, "Цепная молния", "Вызывает молнию, поражающую несколько целей",
        4, "Урон 20 по 3 ближайшим врагам"};

    abilityDatabase[AbilityType::FLAME_BURST] = {
        AbilityType::FLAME_BURST, "Взрыв пламени", "Создает огненный взрыв вокруг себя",
        3, "Урон 25 всем врагам"};

    abilityDatabase[AbilityType::BLOOD_RITUAL] = {
        AbilityType::BLOOD_RITUAL, "Кровавый ритуал", "Жертвует кровью за демоническую силу",
        3, "Жертва 30 HP, урон +50% на 3 хода"};

    abilityDatabase[AbilityType::LIFE_STEAL] = {
        AbilityType::LIFE_STEAL, "Похищение жизни", "Крадет жизненную силу у врагов",
        4, "Восстановление 50% от нанесенного урона"};

    abilityDatabase[AbilityType::FEAR] = {
        AbilityType::FEAR, "Страх", "Внушает ужас, снижая боеспособность врагов",
        3, "Враги: инициатива -2, урон -3 на 2 хода"};

    abilityDatabase[AbilityType::REGENERATION] = {
        AbilityType::REGENERATION, "Регенерация", "Восстанавливает здоровье естественным путем",
        5, "Восстановление 30 HP в начале хода"};

    abilityDatabase[AbilityType::FLYING] = {
        AbilityType::FLYING, "Полет", "Взлетает и перемещается по воздуху",
        4, "Перемещение на любую позицию"};

    abilityDatabase[AbilityType::FIRE_DAMAGE] = {
        AbilityType::FIRE_DAMAGE, "Огненный урон", "Атаки наносят дополнительный огненный урон",
        4, "Дополнительный урон 30% от базового при каждой атаке"};
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

    // Устанавливаем дополнительные свойства через методы
    hero->setAvailableAbilities(tmpl.availableAbilities);
    hero->setProgressionType(tmpl.progressionType);
    hero->setIsLoner(tmpl.isLoner);

    // Инициализируем уровни способностей
    for (AbilityType ability : tmpl.availableAbilities)
    {
        hero->setAbilityLevel(ability, 1); // Базовый уровень
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
