#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include "ItemTemplates.h"
#include <vector>
#include <map>
#include <string>

class ItemFactory
{
private:
    static std::vector<ItemTemplate> itemTemplates;

    static std::string generateAbilityString(const std::map<std::string, int> &stats);
    static void initializeTemplates();

public:
    static const std::vector<ItemTemplate> &getAllItems();
    static std::vector<ItemTemplate> getItemsByType(ItemType type);
    static Item createItem(int index);
    static Item createItem(const std::string &name);
    static Item createRandomItem();
    static Item createRandomItemOfType(ItemType type);
    static const ItemTemplate &getItemTemplate(int index);
};

#endif // ITEMFACTORY_H
