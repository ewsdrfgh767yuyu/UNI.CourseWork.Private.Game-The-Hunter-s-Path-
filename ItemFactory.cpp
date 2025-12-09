#include "ItemTemplates.h"
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>

// Initialize static members
std::vector<ItemTemplate> ItemFactory::itemTemplates;

std::string ItemFactory::generateAbilityString(const std::map<std::string, int> &stats)
{
    if (stats.empty())
        return "";
    std::string result = "(";
    bool first = true;
    for (const auto &pair : stats)
    {
        if (!first)
            result += ", ";
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
        itemTemplates.push_back({"Rusty Sword" + (abilityStr.empty() ? "" : " " + abilityStr), "Old but reliable sword that has seen better days", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 12}, {"attack", 2}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Elven Blade" + (abilityStr.empty() ? "" : " " + abilityStr), "Light and sharp sword forged by elven masters", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 15}, {"attack", 3}, {"defense", -1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Hammer of Thunder" + (abilityStr.empty() ? "" : " " + abilityStr), "Heavy hammer charged with storm power", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 10}, {"attack", 2}, {"initiative", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Bow of Shadows" + (abilityStr.empty() ? "" : " " + abilityStr), "A mysterious bow that shoots arrows of pure darkness", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 14}, {"attack", 3}, {"stamina", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Staff of Lightning" + (abilityStr.empty() ? "" : " " + abilityStr), "An ancient staff that conducts electrical energy", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 6}, {"attack", 1}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Assassin's Dagger" + (abilityStr.empty() ? "" : " " + abilityStr), "A cunning dagger coated with deadly poison", ItemType::WEAPON, EquipmentSlot::OFF_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 8}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Dragon Shield" + (abilityStr.empty() ? "" : " " + abilityStr), "A massive shield made of dragon scales", ItemType::WEAPON, EquipmentSlot::OFF_HAND, stats});
    }

    // ARMOR
    {
        auto stats = std::map<std::string, int>{{"defense", 4}, {"health", 10}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Barbarian Helmet" + (abilityStr.empty() ? "" : " " + abilityStr), "A rough helmet made of animal skins and bones", ItemType::ARMOR, EquipmentSlot::HEAD, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 2}, {"attack", 2}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"King's Crown" + (abilityStr.empty() ? "" : " " + abilityStr), "A golden crown symbolizing power", ItemType::ARMOR, EquipmentSlot::HEAD, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 10}, {"health", 20}, {"initiative", -1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Knight's Breastplate" + (abilityStr.empty() ? "" : " " + abilityStr), "Heavy armor that protects against any blows", ItemType::ARMOR, EquipmentSlot::CHEST, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 3}, {"stamina", 2}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Mage Robe" + (abilityStr.empty() ? "" : " " + abilityStr), "Light fabric imbued with magical energy", ItemType::ARMOR, EquipmentSlot::CHEST, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 3}, {"defense", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Warrior Gloves" + (abilityStr.empty() ? "" : " " + abilityStr), "Reinforced gloves for powerful strikes", ItemType::ARMOR, EquipmentSlot::HANDS, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 3}, {"initiative", 2}, {"stamina", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Traveler's Boots" + (abilityStr.empty() ? "" : " " + abilityStr), "Comfortable footwear for long journeys", ItemType::ARMOR, EquipmentSlot::FEET, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 6}, {"health", 15}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Guardian Greaves" + (abilityStr.empty() ? "" : " " + abilityStr), "Reliable greaves that protect the legs", ItemType::ARMOR, EquipmentSlot::LEGS, stats});
    }

    // ACCESSORIES
    {
        auto stats = std::map<std::string, int>{{"damage", 5}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Ring of Strength" + (abilityStr.empty() ? "" : " " + abilityStr), "A simple ring that enhances physical power", ItemType::ACCESSORY, EquipmentSlot::RING1, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 5}, {"health", 10}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Amulet of Protection" + (abilityStr.empty() ? "" : " " + abilityStr), "A sacred amulet that wards off the blows of fate", ItemType::ACCESSORY, EquipmentSlot::NECK, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"stamina", 3}, {"initiative", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Ring of Mana" + (abilityStr.empty() ? "" : " " + abilityStr), "A ring filled with magical energy", ItemType::ACCESSORY, EquipmentSlot::RING2, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"attack", 2}, {"initiative", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Ring of Shadows" + (abilityStr.empty() ? "" : " " + abilityStr), "A mysterious ring that grants the power of shadows", ItemType::ACCESSORY, EquipmentSlot::RING1, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health", 25}, {"defense", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Necklace of Life" + (abilityStr.empty() ? "" : " " + abilityStr), "A precious necklace pulsing with life force", ItemType::ACCESSORY, EquipmentSlot::NECK, stats});
    }

    // CONSUMABLES
    {
        auto stats = std::map<std::string, int>{{"health_restore", 50}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Health Potion" + (abilityStr.empty() ? "" : " " + abilityStr), "Red liquid that restores vital forces", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"stamina_restore", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Stamina Potion" + (abilityStr.empty() ? "" : " " + abilityStr), "Blue liquid that restores energy", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health_restore", 100}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Greater Health Potion" + (abilityStr.empty() ? "" : " " + abilityStr), "A powerful potion that fully heals wounds", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 20}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Poison" + (abilityStr.empty() ? "" : " " + abilityStr), "Deadly poison that deals damage over time", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // For use on enemies
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 10}, {"attack", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Elixir of Strength" + (abilityStr.empty() ? "" : " " + abilityStr), "Temporary boost to physical abilities", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Temporary effect
    }

    {
        auto stats = std::map<std::string, int>{{"teleport", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Teleportation Scroll" + (abilityStr.empty() ? "" : " " + abilityStr), "A magical scroll for instant relocation", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Special effect
    }

    {
        auto stats = std::map<std::string, int>{{"invisible", 3}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Invisibility Potion" + (abilityStr.empty() ? "" : " " + abilityStr), "Makes invisible for a short time", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // In seconds or turns
    }

    {
        auto stats = std::map<std::string, int>{{"stamina_restore", 10}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Energy Crystal" + (abilityStr.empty() ? "" : " " + abilityStr), "Restores all stamina", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health_restore", 200}, {"stamina_restore", 5}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Fruit of Life" + (abilityStr.empty() ? "" : " " + abilityStr), "A mystical fruit that grants eternal youth", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health_restore", 30}, {"stamina_restore", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Restoration Powder" + (abilityStr.empty() ? "" : " " + abilityStr), "Magical powder for quick healing", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    // Additional unique items
    {
        auto stats = std::map<std::string, int>{{"damage", 18}, {"attack", 3}, {"fire_damage", 5}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Sword of Flame" + (abilityStr.empty() ? "" : " " + abilityStr), "A blade engulfed in eternal fire", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 16}, {"attack", 2}, {"ice_damage", 7}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Ice Staff" + (abilityStr.empty() ? "" : " " + abilityStr), "A staff radiating cold energy", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"defense", 8}, {"health", 15}, {"attack", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Dragon Helmet" + (abilityStr.empty() ? "" : " " + abilityStr), "A helmet from the head of an ancient dragon", ItemType::ARMOR, EquipmentSlot::HEAD, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"attack", 3}, {"initiative", 2}, {"command", 1}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Ring of Dominion" + (abilityStr.empty() ? "" : " " + abilityStr), "A ring that gives control over minds", ItemType::ACCESSORY, EquipmentSlot::RING1, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"health", 20}, {"regeneration", 5}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Amulet of Regeneration" + (abilityStr.empty() ? "" : " " + abilityStr), "An amulet that accelerates natural healing", ItemType::ACCESSORY, EquipmentSlot::NECK, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"damage", 15}, {"attack", 3}, {"defense", -2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Rage Potion" + (abilityStr.empty() ? "" : " " + abilityStr), "Causes uncontrollable rage", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Temporary berserk
    }

    {
        auto stats = std::map<std::string, int>{{"fire_damage", 50}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Fireball Scroll" + (abilityStr.empty() ? "" : " " + abilityStr), "Summons a fireball", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats});
    }

    {
        auto stats = std::map<std::string, int>{{"ice_damage", 40}, {"freeze", 2}};
        std::string abilityStr = generateAbilityString(stats);
        itemTemplates.push_back({"Ice Crystal" + (abilityStr.empty() ? "" : " " + abilityStr), "Freezes enemies", ItemType::CONSUMABLE, EquipmentSlot::NONE, stats}); // Freeze for turns
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