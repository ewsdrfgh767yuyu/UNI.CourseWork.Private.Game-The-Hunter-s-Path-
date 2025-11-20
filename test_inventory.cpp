#include "entity.h"
#include <iostream>

int main()
{
    Player hero("TestHero");
    Item sword("Sword", "A sharp sword", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, {{"damage", 5}});
    Item potion("Health Potion", "Restores health", ItemType::CONSUMABLE, EquipmentSlot::NONE, {{"health_restore", 20}});

    // Add items
    hero.addItem(sword);
    hero.addItem(potion);

    std::cout << "Inventory after adding:\n";
    hero.displayInventory();

    // Remove item (remove the potion at index 1)
    hero.removeItem(1);

    std::cout << "Inventory after removing potion:\n";
    hero.displayInventory();

    // Try to remove invalid index
    try
    {
        hero.removeItem(10);
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}
