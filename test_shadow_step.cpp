#include "BattleSystem.h"
#include "HeroTemplates.h"
#include "EnemyTemplates.h"
#include <iostream>

int main() {
    // Create a barbarian (has Shadow Step)
    Player* barbarian = HeroFactory::createHero(HeroClass::BARBARIAN, "TestBarb");

    // Create an enemy
    Enemy* goblin = new Goblin("TestGoblin");

    // Create battle system
    BattleSystem battle;

    // Start battle
    vector<Entity*> players = {barbarian};
    vector<Entity*> enemies = {goblin};
    battle.startBattle(players, enemies);

    // Try to use Shadow Step multiple times
    cout << "Using Shadow Step first time:\n";
    bool success1 = battle.useAbility(barbarian, AbilityType::SHADOW_STEP);
    cout << "Success: " << success1 << "\n";

    // Regenerate stamina for second use
    barbarian->regenerateStamina();

    cout << "Using Shadow Step second time:\n";
    bool success2 = battle.useAbility(barbarian, AbilityType::SHADOW_STEP);
    cout << "Success: " << success2 << "\n";

    // Clean up
    delete barbarian;
    delete goblin;

    return 0;
}