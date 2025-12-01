#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <string>

// Item template structure
struct ItemTemplate
{
    std::string name = "";
    std::string description = "";
    ItemType type = ItemType::CONSUMABLE;
    EquipmentSlot slot = EquipmentSlot::NONE;
    std::map<std::string, int> stats = {};
};

// Factory for creating items
class ItemFactory
{
private:
    static std::vector<ItemTemplate> itemTemplates;

    static void initializeTemplates();

public:
    // Get list of all items
    static const std::vector<ItemTemplate> &getAllItems();

    // Get items by type
    static std::vector<ItemTemplate> getItemsByType(ItemType type);

    // Create item by index
    static Item createItem(int index);

    // Create random item
    static Item createRandomItem();

    // Create random item of specific type
    static Item createRandomItemOfType(ItemType type);

    // Generate ability string
    static std::string generateAbilityString(const std::map<std::string, int>& stats);
};