#pragma once
#include "entity.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <random>
#include <iostream>

using namespace std;

// Структура для хранения информации о позиции сущности в бою
struct BattlePosition
{
    Entity *entity;
    int position; // 0-3 (0 первая линия, 1 вторая, 2 третья)
    int corpseHP; // HP трупа, если entity == nullptr

    BattlePosition(Entity *e = nullptr, int pos = 0, int cHP = 0)
        : entity(e), position(pos), corpseHP(cHP) {}
};

// Структура для хранения информации о ходе
struct TurnInfo
{
    Entity *entity;
    int priority; // Приоритет для сортировки очереди

    TurnInfo(Entity *e = nullptr, int p = 0) : entity(e), priority(p) {}
};

class BattleSystem
{
private:
    vector<BattlePosition> playerPositions; // Позиции игроков (до 4)
    vector<BattlePosition> enemyPositions;  // Позиции врагов (до 4)
    vector<Entity *> turnOrder;             // Текущая очередь ходов
    size_t currentTurnIndex;                // Индекс текущего хода
    bool battleActive;                      // Флаг активного боя

    // Генератор случайных чисел
    mt19937 randomGenerator;

    // Вспомогательные методы
    bool canAttackTarget(Entity *attacker, Entity *target) const;
    bool canAttackCorpse(Entity *attacker, int targetPosition) const;
    int getEntityPosition(Entity *entity) const;
    bool isPositionBlocked(int position, const vector<BattlePosition> &positions) const;
    bool hasEmptyPositions(const vector<BattlePosition> &positions) const;
    vector<pair<Entity *, int>> getAvailableTargets(Entity *attacker, bool isPlayerAttacker) const;
    void regenerateStaminaForTurn();
    void applyAbilityEffect(Entity *attacker, Entity *target, int damage);
    void shiftPositionsAfterDeath(vector<BattlePosition> &positions, int deadPosition);

public:
    void removeDeadEntities(); // Made public for testing
    void calculateTurnOrder(); // Made public for testing

public:
    BattleSystem();

    // Основные методы управления боем
    void startBattle(const vector<Entity *> &players, const vector<Entity *> &enemies);
    void endBattle();
    bool isBattleActive() const { return battleActive; }

    // Методы для выполнения действий
    bool attack(Entity *attacker, Entity *target);
    bool movePosition(Entity *entity, int newPosition);
    bool useItem(Entity *user, int itemIndex);
    bool useAbility(Entity *user, AbilityType ability);
    bool skipHalfTurn(Entity *entity);

    // Методы для получения информации
    Entity *getCurrentTurnEntity() const;
    vector<pair<Entity *, int>> getAvailableTargetsForCurrent() const;
    vector<pair<Entity *, string>> getAllEntitiesWithStatus() const;
    string getBattleStatus() const;
    const vector<BattlePosition> &getPlayerPositions() const { return playerPositions; }
    const vector<BattlePosition> &getEnemyPositions() const { return enemyPositions; }
    const vector<Entity *> &getTurnOrder() const { return turnOrder; }
    void displayEntityDetails(Entity *entity) const;
    string getAttackDescription(Entity *attacker, Entity *target) const;

    // Методы для проверки условий
    bool isPlayerVictory() const;
    bool isPlayerDefeat() const;

    // Методы управления очередью ходов
    void nextTurn();

    // Отладочные методы
    void printBattlefield() const;
    void printTurnOrder() const;
};
