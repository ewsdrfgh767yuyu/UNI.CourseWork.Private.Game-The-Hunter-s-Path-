#include "HeroTemplates.h"
#include "entity.h"
#include <iostream>
#include <cassert>

void testExperience()
{
    Player *hero = HeroFactory::createHero(HeroClass::WARRIOR, "TestHero");

    std::cout << "Initial level: " << hero->getLevel() << std::endl;
    std::cout << "Initial EXP: " << hero->getReceivedExperience() << "/" << hero->getRequiredExperience() << std::endl;
    std::cout << "Initial HP: " << hero->getMaxHealthPoint() << ", Damage: " << hero->getDamage() << std::endl;

    // Gain experience to level up
    hero->setReceivedExperience(hero->getReceivedExperience() + 500);
    hero->upLevel();

    std::cout << "After gaining EXP: " << hero->getReceivedExperience() << "/" << hero->getRequiredExperience() << std::endl;
    std::cout << "New level: " << hero->getLevel() << std::endl;
    std::cout << "New HP: " << hero->getMaxHealthPoint() << ", Damage: " << hero->getDamage() << std::endl;

    assert(hero->getLevel() == 2);
    assert(hero->getRequiredExperience() == 275); // Check progression: 250 + 25
    assert(hero->getMaxHealthPoint() == 130);     // Base 120 + 10 level up
    assert(hero->getDamage() == 16);              // Base 15 + 1 level up

    delete hero;
}

int main()
{
    testExperience();
    std::cout << "Experience test passed!" << std::endl;
    return 0;
}