#include "HeroFactory.cpp"
#include "entity.h"
#include <iostream>
#include <cassert>

void testAbilities()
{
    Player *hero = HeroFactory::createHero(HeroClass::WARRIOR, "TestHero");

    std::cout << "Hero abilities: " << std::endl;
    for (auto ability : hero->getAvailableAbilities())
    {
        std::cout << " - " << static_cast<int>(ability) << std::endl;
    }

    // Test ability level
    hero->setAbilityLevel(AbilityType::BERSERK, 2);
    assert(hero->getAbilityLevel(AbilityType::BERSERK) == 2);

    delete hero;
}

int main()
{
    testAbilities();
    std::cout << "Abilities test passed!" << std::endl;
    return 0;
}