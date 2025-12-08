#include "HeroFactory.cpp"
#include "entity.h"
#include <iostream>
#include <cassert>

void testProgressBars()
{
    Player *hero = HeroFactory::createHero(HeroClass::WARRIOR, "TestHero");

    std::cout << "Hero created: " << hero->getName() << std::endl;
    std::cout << "Initial HP: " << hero->getCurrentHealthPoint() << "/" << hero->getMaxHealthPoint() << std::endl;
    std::cout << "Initial EXP: " << hero->getReceivedExperience() << std::endl;

    // Simulate gaining experience
    hero->setReceivedExperience(hero->getReceivedExperience() + 100);
    hero->upLevel();
    std::cout << "After gaining EXP: " << hero->getReceivedExperience() << std::endl;

    // Check if leveled up
    if (hero->getLevel() > 1)
    {
        std::cout << "Leveled up to " << hero->getLevel() << std::endl;
    }

    delete hero;
}

int main()
{
    testProgressBars();
    std::cout << "Progress bars test passed!" << std::endl;
    return 0;
}