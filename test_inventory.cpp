#include "HeroFactory.cpp"
#include "entity.h"
#include <iostream>
#include <cassert>

void testInventory()
{
    Player *hero = HeroFactory::createHero(HeroClass::WARRIOR, "TestHero");

    Item potion("Health Potion", "Restores HP", ItemType::CONSUMABLE, EquipmentSlot::NONE, {{"health_restore", 50}});

    hero->addItem(potion);
    std::cout << "Added item to inventory. Size: " << hero->getInventory().size() << std::endl;

    assert(hero->getInventory().size() == 1);

    // Use consumable
    hero->useConsumable(0);
    std::cout << "Used consumable. Inventory size: " << hero->getInventory().size() << std::endl;

    assert(hero->getInventory().empty());

    delete hero;
}

int main()
{
    testInventory();
    std::cout << "Inventory test passed!" << std::endl;
    return 0;
}
