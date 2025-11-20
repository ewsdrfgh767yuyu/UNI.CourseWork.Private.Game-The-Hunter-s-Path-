#include "ItemTemplates.h"
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>

// Initialize static members
std::vector<ItemTemplate> ItemFactory::itemTemplates;

void ItemFactory::initializeTemplates()
{
    // WEAPONS
    itemTemplates.push_back({
        "Ржавый Меч", "Старый, но надежный меч, видевший лучшие времена", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 8}, {"attack", 1}}});

    itemTemplates.push_back({
        "Эльфийский Клинок", "Легкий и острый меч, выкованный эльфийскими мастерами", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 12}, {"attack", 2}, {"initiative", 1}}});

    itemTemplates.push_back({
        "Молот Грома", "Тяжелый молот, заряженный силой бури", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 15}, {"attack", 3}, {"defense", -1}}});

    itemTemplates.push_back({
        "Лук Теней", "Таинственный лук, стреляющий стрелами из чистой тьмы", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 10}, {"attack", 2}, {"initiative", 2}}});

    itemTemplates.push_back({
        "Посох Молний", "Древний посох, проводящий электрическую энергию", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 14}, {"attack", 3}, {"stamina", 1}}});

    itemTemplates.push_back({
        "Кинжал Отравителя", "Коварный кинжал, покрытый смертельным ядом", ItemType::WEAPON, EquipmentSlot::OFF_HAND,
        {{"damage", 6}, {"attack", 1}, {"initiative", 1}}});

    itemTemplates.push_back({
        "Щит Дракона", "Массивный щит из драконьей чешуи", ItemType::WEAPON, EquipmentSlot::OFF_HAND,
        {{"defense", 8}, {"attack", 1}}});

    // ARMOR
    itemTemplates.push_back({
        "Шлем Варвара", "Грубый шлем из звериных шкур и костей", ItemType::ARMOR, EquipmentSlot::HEAD,
        {{"defense", 4}, {"health", 10}}});

    itemTemplates.push_back({
        "Корона Короля", "Золотая корона, символизирующая власть", ItemType::ARMOR, EquipmentSlot::HEAD,
        {{"defense", 2}, {"attack", 2}, {"initiative", 1}}});

    itemTemplates.push_back({
        "Нагрудник Рыцаря", "Тяжелая броня, защищающая от любых ударов", ItemType::ARMOR, EquipmentSlot::CHEST,
        {{"defense", 10}, {"health", 20}, {"initiative", -1}}});

    itemTemplates.push_back({
        "Роба Мага", "Легкая ткань, пропитанная магической энергией", ItemType::ARMOR, EquipmentSlot::CHEST,
        {{"defense", 3}, {"stamina", 2}, {"initiative", 1}}});

    itemTemplates.push_back({
        "Перчатки Воина", "Укрепленные перчатки для мощных ударов", ItemType::ARMOR, EquipmentSlot::HANDS,
        {{"damage", 3}, {"defense", 2}}});

    itemTemplates.push_back({
        "Сапоги Путника", "Комфортная обувь для долгих странствий", ItemType::ARMOR, EquipmentSlot::FEET,
        {{"defense", 3}, {"initiative", 2}, {"stamina", 1}}});

    itemTemplates.push_back({
        "Поножи Стража", "Надежные поножи, защищающие ноги", ItemType::ARMOR, EquipmentSlot::LEGS,
        {{"defense", 6}, {"health", 15}}});

    // ACCESSORIES
    itemTemplates.push_back({
        "Кольцо Силы", "Простое кольцо, усиливающее физическую мощь", ItemType::ACCESSORY, EquipmentSlot::RING1,
        {{"damage", 5}, {"attack", 1}}});

    itemTemplates.push_back({
        "Амулет Защиты", "Священный амулет, отводящий удары судьбы", ItemType::ACCESSORY, EquipmentSlot::NECK,
        {{"defense", 5}, {"health", 10}}});

    itemTemplates.push_back({
        "Кольцо Маны", "Кольцо, наполненное магической энергией", ItemType::ACCESSORY, EquipmentSlot::RING2,
        {{"stamina", 3}, {"initiative", 1}}});

    itemTemplates.push_back({
        "Перстень Теней", "Таинственное кольцо, дающее силу теней", ItemType::ACCESSORY, EquipmentSlot::RING1,
        {{"attack", 2}, {"initiative", 2}}});

    itemTemplates.push_back({
        "Ожерелье Жизни", "Драгоценное ожерелье, пульсирующее жизненной силой", ItemType::ACCESSORY, EquipmentSlot::NECK,
        {{"health", 25}, {"defense", 2}}});

    // CONSUMABLES
    itemTemplates.push_back({
        "Зелье Здоровья", "Красная жидкость, восстанавливающая жизненные силы", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"health_restore", 50}}});

    itemTemplates.push_back({
        "Зелье Выносливости", "Голубая жидкость, восстанавливающая энергию", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"stamina_restore", 2}}});

    itemTemplates.push_back({
        "Большое Зелье Здоровья", "Мощное зелье, полностью исцеляющее раны", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"health_restore", 100}}});

    itemTemplates.push_back({
        "Яд", "Смертельный яд, наносящий урон со временем", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"damage", 20}}}); // Для использования на врагах

    itemTemplates.push_back({
        "Эликсир Силы", "Временное усиление физических способностей", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"damage", 10}, {"attack", 2}}}); // Временный эффект

    itemTemplates.push_back({
        "Свиток Телепортации", "Магический свиток для мгновенного перемещения", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"teleport", 1}}}); // Специальный эффект

    itemTemplates.push_back({
        "Зелье Невидимости", "Делает невидимым на короткое время", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"invisible", 3}}}); // В секундах или ходах

    itemTemplates.push_back({
        "Кристалл Энергии", "Восстанавливает всю выносливость", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"stamina_restore", 10}}});

    itemTemplates.push_back({
        "Фрукт Жизни", "Мистический фрукт, дающий вечную молодость", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"health_restore", 200}, {"stamina_restore", 5}}});

    itemTemplates.push_back({
        "Порошок Восстановления", "Магический порошок для быстрого исцеления", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"health_restore", 30}, {"stamina_restore", 1}}});

    // Additional unique items
    itemTemplates.push_back({
        "Меч Пламени", "Клинок, объятый вечным огнем", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 18}, {"attack", 3}, {"fire_damage", 5}}});

    itemTemplates.push_back({
        "Ледяной Посох", "Посох, излучающий холодную энергию", ItemType::WEAPON, EquipmentSlot::MAIN_HAND,
        {{"damage", 16}, {"attack", 2}, {"ice_damage", 7}}});

    itemTemplates.push_back({
        "Шлем Дракона", "Шлем из головы древнего дракона", ItemType::ARMOR, EquipmentSlot::HEAD,
        {{"defense", 8}, {"health", 15}, {"attack", 1}}});

    itemTemplates.push_back({
        "Кольцо Власти", "Кольцо, дающее контроль над умами", ItemType::ACCESSORY, EquipmentSlot::RING1,
        {{"attack", 3}, {"initiative", 2}, {"command", 1}}});

    itemTemplates.push_back({
        "Амулет Регенерации", "Амулет, ускоряющий естественное исцеление", ItemType::ACCESSORY, EquipmentSlot::NECK,
        {{"health", 20}, {"regeneration", 5}}});

    itemTemplates.push_back({
        "Зелье Ярости", "Вызывает неконтролируемую ярость", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"damage", 15}, {"attack", 3}, {"defense", -2}}}); // Временный берсерк

    itemTemplates.push_back({
        "Свиток Огненного Шара", "Вызывает огненный шар", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"fire_damage", 50}}});

    itemTemplates.push_back({
        "Кристалл Льда", "Замораживает врагов", ItemType::CONSUMABLE, EquipmentSlot::NONE,
        {{"ice_damage", 40}, {"freeze", 2}}}); // Заморозка на ходы
}

const std::vector<ItemTemplate> &ItemFactory::getAllItems()
{
    if (itemTemplates.empty())
    {
        initializeTemplates();
    }
    return itemTemplates;
}

std::vector<ItemTemplate> ItemFactory::getItemsByType(ItemType type)
{
    if (itemTemplates.empty())
    {
        initializeTemplates();
    }

    std::vector<ItemTemplate> filtered;
    for (const auto &item : itemTemplates)
    {
        if (item.type == type)
        {
            filtered.push_back(item);
        }
    }
    return filtered;
}

Item ItemFactory::createItem(int index)
{
    if (itemTemplates.empty())
    {
        initializeTemplates();
    }

    if (index < 0 || index >= static_cast<int>(itemTemplates.size()))
    {
        return Item(); // Return default item
    }

    const ItemTemplate &tmpl = itemTemplates[index];
    return Item(tmpl.name, tmpl.description, tmpl.type, tmpl.slot, tmpl.stats);
}

Item ItemFactory::createRandomItem()
{
    if (itemTemplates.empty())
    {
        initializeTemplates();
    }

    int index = rand() % itemTemplates.size();
    return createItem(index);
}

Item ItemFactory::createRandomItemOfType(ItemType type)
{
    std::vector<ItemTemplate> typeItems = getItemsByType(type);
    if (typeItems.empty())
    {
        return Item(); // Return default item
    }

    int index = rand() % typeItems.size();
    const ItemTemplate &tmpl = typeItems[index];
    return Item(tmpl.name, tmpl.description, tmpl.type, tmpl.slot, tmpl.stats);
}