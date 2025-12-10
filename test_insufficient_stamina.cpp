#include "BattleSystem.h"
#include "HeroTemplates.h"
#include "EnemyTemplates.h"
#include <iostream>

int main()
{
    // Create a barbarian (has Shadow Step)
    Player *barbarian = HeroFactory::createHero(HeroClass::BARBARIAN, "TestBarb");

    // Create an enemy
    Enemy *goblin = new Goblin("TestGoblin");

    // Create battle system
    BattleSystem battle;

    // Start battle
    vector<Entity *> players = {barbarian};
    vector<Entity *> enemies = {goblin};
    battle.startBattle(players, enemies);

    // Use Shadow Step first time (cost 2, max 3, remains 1)
    cout << "Using Shadow Step first time:\n";
    bool success1 = battle.useAbility(barbarian, AbilityType::SHADOW_STEP);
    cout << "Success: " << success1 << "\n";
    cout << "Stamina after first use: " << barbarian->getCurrentStamina() << "/" << barbarian->getMaxStamina() << "\n";

    // Try to use again without regenerating (should fail)
    cout << "Trying Shadow Step second time without regen:\n";
    bool success2 = battle.useAbility(barbarian, AbilityType::SHADOW_STEP);
    cout << "Success: " << success2 << "\n";

    // Clean up
    delete barbarian;
    delete goblin;

    return 0;
}