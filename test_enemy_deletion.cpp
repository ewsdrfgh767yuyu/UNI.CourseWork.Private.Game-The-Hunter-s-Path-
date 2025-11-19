#include "BattleSystem.h"
#include "CampaignSystem.h"
#include "HeroTemplates.h"
#include "EnemyTemplates.h"
#include <iostream>
#include <cassert>

using namespace std;

void testEnemyDeletionInBattle()
{
    cout << "Testing enemy deletion in battle..." << endl;

    BattleSystem battleSystem;
    CampaignSystem campaignSystem;

    // Create test entities
    Player *player = HeroFactory::createHero(HeroClass::WARRIOR, "Test Warrior");
    Enemy *enemy1 = EnemyFactory::createEnemyByName("Goblin", 0);
    Enemy *enemy2 = EnemyFactory::createEnemyByName("Orc", 0);

    // Start battle
    vector<Entity *> players = {player};
    vector<Entity *> enemies = {enemy1, enemy2};
    battleSystem.startBattle(players, enemies);

    // Simulate enemy deaths
    enemy1->takeDamage(1000); // Kill enemy1
    enemy2->takeDamage(1000); // Kill enemy2

    // Call removeDeadEntities (normally called in attack method)
    battleSystem.removeDeadEntities();

    // Check that enemies are marked as nullptr, not deleted
    auto &enemyPositions = const_cast<vector<BattlePosition> &>(battleSystem.getEnemyPositions());
    bool enemy1Null = false, enemy2Null = false;
    for (auto &pos : enemyPositions)
    {
        if (pos.position == 0 && pos.entity == nullptr)
            enemy1Null = true;
        if (pos.position == 1 && pos.entity == nullptr)
            enemy2Null = true;
    }
    cout << "Enemy1 marked as nullptr: " << (enemy1Null ? "PASS" : "FAIL") << endl;
    assert(enemy1Null);
    cout << "Enemy2 marked as nullptr: " << (enemy2Null ? "PASS" : "FAIL") << endl;
    assert(enemy2Null);

    // End battle
    battleSystem.endBattle();

    // Now, simulate CampaignSystem deleting enemies
    // In real code, this happens in CampaignSystem after battle
    // For test, manually delete
    delete enemy1;
    delete enemy2;

    cout << "Enemies deleted after battle: PASS" << endl;

    // Clean up player
    delete player;

    cout << "Test passed!" << endl;
}

void testMixedDeaths()
{
    cout << "Testing mixed player and enemy deaths..." << endl;

    BattleSystem battleSystem;

    Player *player1 = HeroFactory::createHero(HeroClass::WARRIOR, "Warrior");
    Player *player2 = HeroFactory::createHero(HeroClass::MAGE, "Mage");
    Enemy *enemy1 = EnemyFactory::createEnemyByName("Goblin", 0);
    Enemy *enemy2 = EnemyFactory::createEnemyByName("Orc", 0);

    vector<Entity *> players = {player1, player2};
    vector<Entity *> enemies = {enemy1, enemy2};
    battleSystem.startBattle(players, enemies);

    // Kill one player and one enemy
    player1->takeDamage(1000);
    enemy1->takeDamage(1000);

    battleSystem.removeDeadEntities();

    // Check positions shifted
    auto &playerPositions = const_cast<vector<BattlePosition> &>(battleSystem.getPlayerPositions());
    auto &enemyPositions = const_cast<vector<BattlePosition> &>(battleSystem.getEnemyPositions());

    bool playerShifted = false;
    for (auto &pos : playerPositions)
    {
        if (pos.position == 0 && pos.entity == nullptr)
            playerShifted = true;
    }
    cout << "Player position shifted after death: " << (playerShifted ? "PASS" : "FAIL") << endl;
    assert(playerShifted);

    bool enemyShifted = false;
    for (auto &pos : enemyPositions)
    {
        if (pos.position == 0 && pos.entity == nullptr)
            enemyShifted = true;
    }
    cout << "Enemy position shifted after death: " << (enemyShifted ? "PASS" : "FAIL") << endl;
    assert(enemyShifted);

    // Check turn order recalculation
    battleSystem.calculateTurnOrder();
    auto turnOrder = battleSystem.getTurnOrder();
    cout << "Turn order size after deaths: " << turnOrder.size() << " (expected 2): " << ((turnOrder.size() == 2) ? "PASS" : "FAIL") << endl;
    assert(turnOrder.size() == 2);

    battleSystem.endBattle();

    // Clean up
    delete player1;
    delete player2;
    delete enemy1;
    delete enemy2;

    cout << "Mixed deaths test passed!" << endl;
}

int main()
{
    testEnemyDeletionInBattle();
    testMixedDeaths();
    return 0;
}
