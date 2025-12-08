#include "HeroFactory.cpp"
#include "entity.h"
#include <iostream>
#include <cassert>

void testExperience()
{
    Player *hero = HeroFactory::createHero(HeroClass::WARRIOR, "TestHero");

    std::cout << "Initial level: " << hero->getLevel() << std::endl;
    std::cout << "Initial EXP: " << hero->getReceivedExperience() << "/" << hero->getRequiredExperience() << std::endl;

    // Gain experience
    hero->setReceivedExperience(hero->getReceivedExperience() + 500);
    hero->upLevel();

    std::cout << "After gaining EXP: " << hero->getReceivedExperience() << "/" << hero->getRequiredExperience() << std::endl;
    std::cout << "New level: " << hero->getLevel() << std::endl;

    assert(hero->getLevel() >= 1);

    delete hero;
}

int main()
{
    testExperience();
    std::cout << "Experience test passed!" << std::endl;
    return 0;
}