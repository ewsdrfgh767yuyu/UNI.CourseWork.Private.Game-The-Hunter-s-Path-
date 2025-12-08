#include "entity.h"
#include <iostream>
#include <cassert>

void testItems()
{
    // Create a test item
    Item sword("Sword", "A sharp blade", ItemType::WEAPON, EquipmentSlot::MAIN_HAND, {{"damage", 10}, {"attack", 5}});

    std::cout << "Item created: " << sword.getName() << std::endl;
    std::cout << "Type: " << (sword.getType() == ItemType::WEAPON ? "Weapon" : "Other") << std::endl;
    std::cout << "Damage stat: " << sword.getStat("damage") << std::endl;

    assert(sword.getStat("damage") == 10);
    assert(sword.getStat("nonexistent") == 0);
}

int main()
{
    testItems();
    std::cout << "Items test passed!" << std::endl;
    return 0;
}