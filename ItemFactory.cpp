#include "ItemTemplates.h"
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>

// Initialize static members
std::vector<ItemTemplate> ItemFactory::itemTemplates;

std::string ItemFactory::generateAbilityString(const std::map<std::string, int>& stats)
{
    if (stats.empty()) return "";
    std::string result = "(";
    bool first = true;
    for (const auto& pair : stats) {
        if (!first) result += ", ";
        result += pair.first + " " + (pair.second >= 0 ? "+" : "") + std::to_string(pair.second);
        first = false;
    }
    result += ")";
    return result;
}

void ItemFactory::initializeTemplates()
{
    // WEAPONS
    {
        auto stats = std::map<std::string, int>{{"damage", 8}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Ржавый Меч" + (abilityStr.empty() ? "" : " " + abilityStr), "Старый, но надежный меч, видевший лучшие времена", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 12}, {"attack", 2}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Эльфийский Клинок" + (abilityStr.empty() ? "" : " " + abilityStr), "Легкий и острый меч, выкованный эльфийскими мастерами", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 15}, {"attack", 3}, {"defense", -1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Молот Грома" + (abilityStr.empty() ? "" : " " + abilityStr), "Тяжелый молот, заряженный силой бури", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 10}, {"attack", 2}, {"initiative", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Лук Теней" + (abilityStr.empty() ? "" : " " + abilityStr), "Таинственный лук, стреляющий стрелами из чистой тьмы", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 14}, {"attack", 3}, {"stamina", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Посох Молний" + (abilityStr.empty() ? "" : " " + abilityStr), "Древний посох, проводящий электрическую энергию", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 6}, {"attack", 1}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Кинжал Отравителя" + (abilityStr.empty() ? "" : " " + abilityStr), "Коварный кинжал, покрытый смертельным ядом", ItemType::WEAPON, EquipmentSlot::OFF_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 8}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Щит Дракона" + (abilityStr.empty() ? "" : " " + abilityStr), "Массивный щит из драконьей чешуи", ItemType::WEAPON, EquipmentSlot::OFF_HAND, stats});
    }

    // ARMOR
    {
        auto stats = std::map<std::string, int>{{"defense", 4}, {"health", 10}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Шлем Варвара" + (abilityStr.empty() ? "" : " " + abilityStr), "Грубый шлем из звериных шкур и костей", ItemType::ARMOR, EquipmentSlot::HEAD, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 2}, {"attack", 2}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Корона Короля" + (abilityStr.empty() ? "" : " " + abilityStr), "Золотая корона, символизирующая власть", ItemType::ARMOR, EquipmentSlot::HEAD, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 10}, {"health", 20}, {"initiative", -1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Нагрудник Рыцаря" + (abilityStr.empty() ? "" : " " + abilityStr), "Тяжелая броня, защищающая от любых ударов", ItemType::ARMOR, EquipmentSlot::CHEST, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 3}, {"stamina", 2}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Роба Мага" + (abilityStr.empty() ? "" : " " + abilityStr), "Легкая ткань, пропитанная магической энергией", ItemType::ARMOR, EquipmentSlot::CHEST, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 3}, {"defense", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Перчатки Воина" + (abilityStr.empty() ? "" : " " + abilityStr), "Укрепленные перчатки для мощных ударов", ItemType::ARMOR, EquipmentSlot::HANDS, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 3}, {"initiative", 2}, {"stamina", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Сапоги Путника" + (abilityStr.empty() ? "" : " " + abilityStr), "Комфортная обувь для долгих странствий", ItemType::ARMOR, EquipmentSlot::FEET, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 6}, {"health", 15}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Поножи Стража" + (abilityStr.empty() ? "" : " " + abilityStr), "Надежные поножи, защищающие ноги", ItemType::ARMOR, EquipmentSlot::LEGS, stats});
    }

    // ACCESSORIES
    {
        auto stats = std::map<std::string, int>{{"damage", 5}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Кольцо Силы" + (abilityStr.empty() ? "" : " " + abilityStr), "Простое кольцо, усиливающее физическую мощь", ItemType::ACCESSORY, EquipmentSlot::RING1, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 5}, {"health", 10}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Амулет Защиты" + (abilityStr.empty() ? "" : " " + abilityStr), "Священный амулет, отводящий удары судьбы", ItemType::ACCESSORY, EquipmentSlot::NECK, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"stamina", 3}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Кольцо Маны" + (abilityStr.empty() ? "" : " " + abilityStr), "Кольцо, наполненное магической энергией", ItemType::ACCESSORY, EquipmentSlot::RING2, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"attack", 2}, {"initiative", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Перстень Теней" + (abilityStr.empty() ? "" : " " + abilityStr), "Таинственное кольцо, дающее силу теней", ItemType::ACCESSORY, EquipmentSlot::RING1, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health", 25}, {"defense", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Ожерелье Жизни" + (abilityStr.empty() ? "" : " " + abilityStr), "Драгоценное ожерелье, пульсирующее жизненной силой", ItemType::ACCESSORY, EquipmentSlot::NECK, stats});
    }

    // CONSUMABLES
    {
        auto stats = std::map<std::string, int>{{"health_restore", 50}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Зелье Здоровья" + (abilityStr.empty() ? "" : " " + abilityStr), "Красная жидкость, восстанавливающая жизненные силы", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"stamina_restore", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Зелье Выносливости" + (abilityStr.empty() ? "" : " " + abilityStr), "Голубая жидкость, восстанавливающая энергию", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health_restore", 100}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Большое Зелье Здоровья" + (abilityStr.empty() ? "" : " " + abilityStr), "Мощное зелье, полностью исцеляющее раны", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 20}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Яд" + (abilityStr.empty() ? "" : " " + abilityStr), "Смертельный яд, наносящий урон со временем", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Для использования на врагах
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 10}, {"attack", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Эликсир Силы" + (abilityStr.empty() ? "" : " " + abilityStr), "Временное усиление физических способностей", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Временный эффект
    }

    {
        auto stats = std::map<std::string, int>{{"teleport", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Свиток Телепортации" + (abilityStr.empty() ? "" : " " + abilityStr), "Магический свиток для мгновенного перемещения", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Специальный эффект
    }

    {
        auto stats = std::map<std::string, int>{{"invisible", 3}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Зелье Невидимости" + (abilityStr.empty() ? "" : " " + abilityStr), "Делает невидимым на короткое время", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // В секундах или ходах
    }

    {
        auto stats = std::map<std::string, int>{{"stamina_restore", 10}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Кристалл Энергии" + (abilityStr.empty() ? "" : " " + abilityStr), "Восстанавливает всю выносливость", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health_restore", 200}, {"stamina_restore", 5}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Фрукт Жизни" + (abilityStr.empty() ? "" : " " + abilityStr), "Мистический фрукт, дающий вечную молодость", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health_restore", 30}, {"stamina_restore", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Порошок Восстановления" + (abilityStr.empty() ? "" : " " + abilityStr), "Магический порошок для быстрого исцеления", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    // Additional unique items
    {
        auto stats = std::map<std::string, int>{{"damage", 18}, {"attack", 3}, {"fire_damage", 5}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Меч Пламени" + (abilityStr.empty() ? "" : " " + abilityStr), "Клинок, объятый вечным огнем", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 16}, {"attack", 2}, {"ice_damage", 7}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Ледяной Посох" + (abilityStr.empty() ? "" : " " + abilityStr), "Посох, излучающий холодную энергию", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 8}, {"health", 15}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Шлем Дракона" + (abilityStr.empty() ? "" : " " + abilityStr), "Шлем из головы древнего дракона", ItemType::ARMOR, EquipmentSlot::HEAD, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"attack", 3}, {"initiative", 2}, {"command", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Кольцо Власти" + (abilityStr.empty() ? "" : " " + abilityStr), "Кольцо, дающее контроль над умами", ItemType::ACCESSORY, EquipmentSlot::RING1, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health", 20}, {"regeneration", 5}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Амулет Регенерации" + (abilityStr.empty() ? "" : " " + abilityStr), "Амулет, ускоряющий естественное исцеление", ItemType::ACCESSORY, EquipmentSlot::NECK, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 15}, {"attack", 3}, {"defense", -2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Зелье Ярости" + (abilityStr.empty() ? "" : " " + abilityStr), "Вызывает неконтролируемую ярость", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Временный берсерк
    }

    {
        auto stats = std::map<std::string, int>{{"fire_damage", 50}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Свиток Огненного Шара" + (abilityStr.empty() ? "" : " " + abilityStr), "Вызывает огненный шар", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"ice_damage", 40}, {"freeze", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({
            "Кристалл Льда" + (abilityStr.empty() ? "" : " " + abilityStr), "Замораживает врагов", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Заморозка на ходы
    }
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