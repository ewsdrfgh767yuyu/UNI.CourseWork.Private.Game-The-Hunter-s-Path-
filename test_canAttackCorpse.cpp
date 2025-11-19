#include "BattleSystem.h"
#include "HeroTemplates.h"
#include "EnemyTemplates.h"
#include <iostream>
#include <cassert>

using namespace std;

void testCanAttackCorpse()
{
    cout << "Testing canAttackCorpse method..." << endl;

    BattleSystem battleSystem;

    // Create test entities
    Player *player = HeroFactory::createHero(HeroClass::WARRIOR, "Test Warrior");
    Enemy *enemy = EnemyFactory::createEnemyByName("Goblin", 0);

    // Start battle with player and enemy
    vector<Entity *> players = {player};
    vector<Entity *> enemies = {enemy};
    battleSystem.startBattle(players, enemies);

    // Simulate enemy death to create corpse
    enemy->takeDamage(1000); // Ensure death
    // Manually create corpse since removeDeadEntities is private
    auto &enemyPositions = const_cast<vector<BattlePosition> &>(battleSystem.getEnemyPositions());
    for (auto &pos : enemyPositions)
    {
        if (pos.entity == enemy)
        {
            pos.isCorpse = true;
            pos.corpseHP = 50;
            pos.entity = nullptr;
            break;
        }
    }

    // Test 1: Player attacking enemy corpse at position 0 (should be valid for range 0)
    player->setAttackRange(0);
    bool canAttack = battleSystem.canAttackCorpse(player, 0);
    cout << "Test 1 - Player range 0 attacking corpse at pos 0: " << (canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == true);

    // Test 2: Player attacking enemy corpse at position 1 (should be invalid for range 0)
    canAttack = battleSystem.canAttackCorpse(player, 1);
    cout << "Test 2 - Player range 0 attacking corpse at pos 1: " << (!canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == false);

    // Test 3: Player with range 1 attacking corpse at pos 1 (front line, should be valid)
    player->setAttackRange(1);
    // Manually create corpse at pos 1
    enemyPositions.push_back(BattlePosition(nullptr, 1, true, 50));
    canAttack = battleSystem.canAttackCorpse(player, 1);
    cout << "Test 3 - Player range 1 attacking corpse at pos 1: " << (canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == true);

    // Test 4: Player with range 1 attacking corpse at pos 2 (back line, should be valid)
    enemyPositions.push_back(BattlePosition(nullptr, 2, true, 50));
    canAttack = battleSystem.canAttackCorpse(player, 2);
    cout << "Test 4 - Player range 1 attacking corpse at pos 2: " << (canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == true);

    // Test 5: Player attacking own side corpse (should be invalid)
    // First, create player corpse
    player->takeDamage(1000);
    // Manually create player corpse
    auto &playerPositions = const_cast<vector<BattlePosition> &>(battleSystem.getPlayerPositions());
    for (auto &pos : playerPositions)
    {
        if (pos.entity == player)
        {
            pos.isCorpse = true;
            pos.corpseHP = 50;
            pos.entity = nullptr;
            break;
        }
    }
    canAttack = battleSystem.canAttackCorpse(enemy, 0); // Enemy attacking player corpse
    cout << "Test 5 - Enemy attacking player corpse: " << (!canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == false);

    // Test 6: Invalid position
    canAttack = battleSystem.canAttackCorpse(player, 5);
    cout << "Test 6 - Invalid position 5: " << (!canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == false);

    // Test 7: Null attacker
    canAttack = battleSystem.canAttackCorpse(nullptr, 0);
    cout << "Test 7 - Null attacker: " << (!canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == false);

    // Test 8: No corpse at position
    // Reset battle
    battleSystem.endBattle();
    Player *player2 = HeroFactory::createHero(HeroClass::WARRIOR, "Test Warrior 2");
    Enemy *enemy2 = EnemyFactory::createEnemyByName("Goblin", 0);
    players = {player2};
    enemies = {enemy2};
    battleSystem.startBattle(players, enemies);
    canAttack = battleSystem.canAttackCorpse(player2, 0);
    cout << "Test 8 - No corpse at position: " << (!canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == false);

    // Test 9: Corpse with 0 HP
    enemy2->takeDamage(1000);
    // Manually create corpse with 0 HP
    auto &enemyPositions2 = const_cast<vector<BattlePosition> &>(battleSystem.getEnemyPositions());
    for (auto &pos : enemyPositions2)
    {
        if (pos.entity == enemy2)
        {
            pos.isCorpse = true;
            pos.corpseHP = 0;
            pos.entity = nullptr;
            break;
        }
    }
    canAttack = battleSystem.canAttackCorpse(player2, 0);
    cout << "Test 9 - Corpse with 0 HP: " << (!canAttack ? "PASS" : "FAIL") << endl;
    assert(canAttack == false);

    cout << "All tests passed!" << endl;

    // Clean up
    delete player;
    delete enemy;
    delete player2;
    delete enemy2;
}

int main()
{
    testCanAttackCorpse();
    return 0;
}
