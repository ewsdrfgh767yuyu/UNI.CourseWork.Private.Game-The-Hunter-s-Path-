#include "ItemTemplates.h"
#include <iostream>
#include <ctime>

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    std::cout << "=== TESTING ITEM FACTORY ===\n";

    // Test creating specific item
    Item sword = ItemFactory::createItem(0);
    std::cout << "Created item 0:\n";
    sword.displayInfo();

    // Test creating random item
    Item randomItem = ItemFactory::createRandomItem();
    std::cout << "\nRandom item:\n";
    randomItem.displayInfo();

    // Test creating random weapon
    Item randomWeapon = ItemFactory::createRandomItemOfType(ItemType::WEAPON);
    std::cout << "\nRandom weapon:\n";
    randomWeapon.displayInfo();

    // Test creating random consumable
    Item randomConsumable = ItemFactory::createRandomItemOfType(ItemType::CONSUMABLE);
    std::cout << "\nRandom consumable:\n";
    randomConsumable.displayInfo();

    // Test getting all items count
    const auto& allItems = ItemFactory::getAllItems();
    std::cout << "\nTotal items available: " << allItems.size() << "\n";

    // Test getting weapons count
    auto weapons = ItemFactory::getItemsByType(ItemType::WEAPON);
    std::cout << "Weapons available: " << weapons.size() << "\n";

    return 0;
}