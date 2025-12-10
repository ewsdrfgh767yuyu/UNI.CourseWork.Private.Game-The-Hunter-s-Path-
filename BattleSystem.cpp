#include "BattleSystem.h"
#include "HeroTemplates.h"
#include <algorithm>
#include <numeric>
#include <chrono>

BattleSystem::BattleSystem()
    : currentTurnIndex(0), battleActive(false)
{
    // Инициализация генератора случайных чисел
    randomGenerator.seed(static_cast<unsigned int>(
        chrono::system_clock::now().time_since_epoch().count()));
}

void BattleSystem::startBattle(const vector<Entity *> &players, const vector<Entity *> &enemies)
{
    // Clearing previous battle
    playerPositions.clear();
    enemyPositions.clear();
    turnOrder.clear();
    currentTurnIndex = 0;
    battleActive = true;

    // Player placement (positions 0-3)
    for (int i = 0; i < players.size() && i < 4; ++i)
    {
        playerPositions.push_back(BattlePosition(players[i], static_cast<int>(i)));
    }

    // Enemy placement (positions 0-3)
    for (int i = 0; i < enemies.size() && i < 4; ++i)
    {
        enemyPositions.push_back(BattlePosition(enemies[i], static_cast<int>(i)));
    }

    // Stamina regeneration for all participants
    for (auto &pos : playerPositions)
    {
        if (pos.entity)
            pos.entity->regenerateStamina();
    }
    for (auto &pos : enemyPositions)
    {
        if (pos.entity)
            pos.entity->regenerateStamina();
    }

    // Turn order calculation
    calculateTurnOrder();

    cout << "=== BATTLE STARTED ===\n";
    printBattlefield();
}

void BattleSystem::endBattle()
{
    cout << "[DEBUG] BattleSystem::endBattle() called\n";

    // Display victory or defeat screen before clearing
    if (isPlayerVictory())
    {
        cout << "\nVICTORY!\n";
        cout << "\n";
    }
    else if (isPlayerDefeat())
    {
        cout << "\nDEFEAT!\n";
        cout << "\n";
    }

    battleActive = false;
    playerPositions.clear();
    enemyPositions.clear();
    turnOrder.clear();
    currentTurnIndex = 0;
    cout << "=== BATTLE ENDED ===\n";
    cout << "[DEBUG] BattleSystem::endBattle() completed\n";
}

void BattleSystem::calculateTurnOrder()
{
    turnOrder.clear();
    vector<TurnInfo> allEntities;

    // Добавляем всех живых игроков
    for (const auto &pos : playerPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
        {
            int turnWeight = pos.entity->getInitiative() / 10;
            for (int i = 0; i < turnWeight; ++i)
            {
                allEntities.push_back(TurnInfo(pos.entity, pos.entity->getInitiative() + i));
            }
        }
    }

    // Добавляем всех живых врагов
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
        {
            int turnWeight = pos.entity->getInitiative() / 10;
            for (int i = 0; i < turnWeight; ++i)
            {
                allEntities.push_back(TurnInfo(pos.entity, pos.entity->getInitiative() + i));
            }
        }
    }

    // Сортировка по приоритету (инициатива)
    sort(allEntities.begin(), allEntities.end(),
         [](const TurnInfo &a, const TurnInfo &b)
         {
             return a.priority > b.priority;
         });

    // Формируем очередь ходов
    for (const auto &info : allEntities)
    {
        turnOrder.push_back(info.entity);
    }
}

bool BattleSystem::canAttackTarget(Entity *attacker, Entity *target) const
{
    if (!attacker || !target)
        return false;

    int attackerPos = getEntityPosition(attacker);
    int targetPos = getEntityPosition(target);
    int attackRange = attacker->getAttackRange();

    if (attackerPos == -1 || targetPos == -1)
        return false;

    // Определяем, на какой стороне находится атакующий
    bool isAttackerPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == attacker)
        {
            isAttackerPlayer = true;
            break;
        }
    }

    // Определяем, на какой стороне находится цель
    bool isTargetPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == target)
        {
            isTargetPlayer = true;
            break;
        }
    }

    // Нельзя атаковать союзников
    if (isAttackerPlayer == isTargetPlayer)
        return false;

    // Проверяем, является ли цель доступной для атаки
    int distance = abs(attackerPos - targetPos);

    // Для range=0: только позиция напротив (0-0, 1-1, 2-2, 3-3)
    if (attackRange == 0)
    {
        return distance == 0;
    }

    // Для range=1: можно атаковать через одного союзника
    // Если атакующий на передней линии (0-1), может атаковать переднюю линию врага (0-1)
    // Если атакующий на задней линии (2-3), может атаковать заднюю линию врага (2-3) и переднюю (0-1)
    if (attackRange == 1)
    {
        if (isAttackerPlayer)
        {
            // Игрок на передней линии может атаковать только переднюю линию врага
            if (attackerPos <= 1)
            {
                return targetPos <= 1;
            }
            // Игрок на задней линии может атаковать обе линии врага
            else
            {
                return targetPos <= 1; // Через одного союзника
            }
        }
        else
        {
            // Враг на передней линии может атаковать только переднюю линию игрока
            if (attackerPos <= 1)
            {
                return targetPos <= 1;
            }
            // Враг на задней линии может атаковать обе линии игрока
            else
            {
                return targetPos <= 1; // Через одного союзника
            }
        }
    }

    // Для range>=2: все позиции
    return distance <= attackRange;
}

bool BattleSystem::canAttackCorpse(Entity *attacker, int targetPosition) const
{
    if (!attacker)
        return false;

    int attackerPos = getEntityPosition(attacker);
    int attackRange = attacker->getAttackRange();

    if (attackerPos == -1)
        return false;

    // Определяем, на какой стороне находится атакующий
    bool isAttackerPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == attacker)
        {
            isAttackerPlayer = true;
            break;
        }
    }

    // Определяем, на какой стороне находится цель (трупы только на вражеской стороне)
    const vector<BattlePosition> &opponentPositions = isAttackerPlayer ? enemyPositions : playerPositions;

    // Проверяем, есть ли труп на этой позиции
    bool hasCorpse = false;
    for (const auto &pos : opponentPositions)
    {
        if (pos.position == targetPosition && !pos.entity && pos.corpseHP > 0)
        {
            hasCorpse = true;
            break;
        }
    }

    if (!hasCorpse)
        return false;

    // Проверяем расстояние
    int distance = abs(attackerPos - targetPosition);

    // Для range=0: только позиция напротив (0-0, 1-1, 2-2, 3-3)
    if (attackRange == 0)
    {
        return distance == 0;
    }

    // Для range=1: можно атаковать через одного союзника
    if (attackRange == 1)
    {
        if (isAttackerPlayer)
        {
            // Игрок на передней линии может атаковать только переднюю линию врага
            if (attackerPos <= 1)
            {
                return targetPosition <= 1;
            }
            // Игрок на задней линии может атаковать обе линии врага
            else
            {
                return targetPosition <= 1; // Через одного союзника
            }
        }
        else
        {
            // Враг на передней линии может атаковать только переднюю линию игрока
            if (attackerPos <= 1)
            {
                return targetPosition <= 1;
            }
            // Враг на задней линии может атаковать обе линии игрока
            else
            {
                return targetPosition <= 1; // Через одного союзника
            }
        }
    }

    // Для range>=2: все позиции
    return distance <= attackRange;
}

int BattleSystem::getEntityPosition(Entity *entity) const
{
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == entity)
            return pos.position;
    }
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity == entity)
            return pos.position;
    }
    return -1;
}

bool BattleSystem::isPositionBlocked(int position, const vector<BattlePosition> &positions) const
{
    // Позиции не блокируются, так как трупов нет
    return false;
}

bool BattleSystem::hasEmptyPositions(const vector<BattlePosition> &positions) const
{
    // Проверяем, есть ли пустые позиции (без живых сущностей и трупов)
    for (int i = 0; i < 4; ++i)
    {
        bool occupied = false;
        for (const auto &pos : positions)
        {
            if (pos.position == i)
            {
                occupied = true;
                break;
            }
        }
        if (!occupied)
        {
            return true; // Есть пустая позиция
        }
    }
    return false;
}

vector<pair<Entity *, int>> BattleSystem::getAvailableTargets(Entity *attacker, bool isPlayerAttacker) const
{
    vector<pair<Entity *, int>> targets;

    if (!attacker)
        return targets;

    const vector<BattlePosition> &opponentPositions = isPlayerAttacker ? enemyPositions : playerPositions;

    for (const auto &pos : opponentPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
        {
            if (canAttackTarget(attacker, pos.entity))
            {
                targets.push_back({pos.entity, pos.position});
            }
        }
    }

    return targets;
}

void BattleSystem::removeDeadEntities()
{
    // Remove dead players (only mark as nullptr, deletion in CampaignSystem)
    for (auto &pos : playerPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() <= 0)
        {
            cout << pos.entity->getName() << " fell in battle!\n";
            pos.entity = nullptr;
            shiftPositionsAfterDeath(playerPositions, pos.position);
        }
    }

    // Mark dead enemies as nullptr and create corpses (deletion in CampaignSystem)
    for (auto &pos : enemyPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() <= 0)
        {
            cout << pos.entity->getName() << " defeated!\n";
            pos.entity = nullptr;
            pos.corpseHP = 50; // Create corpse with 50 HP
            shiftPositionsAfterDeath(enemyPositions, pos.position);
        }
    }
}

void BattleSystem::regenerateStaminaForTurn()
{
    Entity *current = getCurrentTurnEntity();
    if (current)
    {
        current->regenerateStamina();
        current->updateEffects(); // Применить эффекты в начале хода

        // Проверить смерть от эффектов и окончание боя
        removeDeadEntities();
        if (isPlayerVictory() || isPlayerDefeat())
        {
            battleActive = false;
        }
    }
}

bool BattleSystem::attack(Entity *attacker, Entity *target)
{
    if (!battleActive || !attacker || !target)
        return false;
    if (attacker->getCurrentHealthPoint() <= 0)
        return false;
    if (attacker->getCurrentStamina() <= 0)
        return false;

    if (!canAttackTarget(attacker, target))
        return false;

    // Расчет урона
    int damage = attacker->attack(target->getDefense());
    target->takeDamage(damage);

    // Применение эффектов способности
    applyAbilityEffect(attacker, target, damage);

    cout << getAttackDescription(attacker, target) << "\n";
    cout << "Deals " << damage << " damage!\n";

    // Проверка на смерть
    bool targetWasDead = target->getCurrentHealthPoint() <= 0;
    removeDeadEntities();

    // Пересчет очереди ходов после смерти
    if (targetWasDead)
    {
        calculateTurnOrder();
        // Корректируем currentTurnIndex, чтобы он не выходил за границы
        if (currentTurnIndex >= turnOrder.size())
        {
            currentTurnIndex = 0;
        }
    }

    // Трата стамины
    attacker->spendStamina();

    // Проверяем, закончен ли бой
    if (isPlayerVictory() || isPlayerDefeat())
    {
        battleActive = false;
    }

    return true;
}

bool BattleSystem::movePosition(Entity *entity, int newPosition)
{
    if (!battleActive || !entity || newPosition < 0 || newPosition > 3)
        return false;
    if (entity->getCurrentHealthPoint() <= 0)
        return false;
    if (entity->getCurrentStamina() <= 0)
        return false;

    // Определяем, игрок это или враг
    bool isPlayerEntity = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == entity)
        {
            isPlayerEntity = true;
            break;
        }
    }

    vector<BattlePosition> &sameSidePositions = isPlayerEntity ? playerPositions : enemyPositions;
    vector<BattlePosition> &opponentPositions = isPlayerEntity ? enemyPositions : playerPositions;

    // Проверяем, есть ли союзник на новой позиции
    bool hasAllyAtPosition = false;
    for (const auto &pos : sameSidePositions)
    {
        if (pos.position == newPosition && pos.entity && pos.entity != entity)
        {
            hasAllyAtPosition = true;
            break;
        }
    }

    // Проверяем, занята ли новая позиция кем-то другим (союзником или врагом)
    bool positionOccupied = false;
    for (const auto &pos : sameSidePositions)
    {
        if (pos.position == newPosition && pos.entity && pos.entity != entity)
        {
            positionOccupied = true;
            break;
        }
    }
    if (!positionOccupied)
    {
        for (const auto &pos : opponentPositions)
        {
            if (pos.position == newPosition && pos.entity)
            {
                positionOccupied = true;
                break;
            }
        }
    }

    if (positionOccupied)
    {
        if (hasAllyAtPosition)
        {
            // Swap places with ally
            vector<BattlePosition> &positions = isPlayerEntity ? playerPositions : enemyPositions;
            int currentPosition = -1;
            for (auto &pos : positions)
            {
                if (pos.entity == entity)
                {
                    currentPosition = pos.position;
                    break;
                }
            }

            if (currentPosition == -1)
                return false;

            // Find ally and swap positions
            for (auto &pos : positions)
            {
                if (pos.position == newPosition && pos.entity && pos.entity != entity)
                {
                    pos.position = currentPosition;
                    break;
                }
            }

            // Update current character's position
            for (auto &pos : positions)
            {
                if (pos.entity == entity)
                {
                    pos.position = newPosition;
                    entity->spendStamina();
                    cout << entity->getName() << " swaps places with ally to position " << newPosition << "\n";
                    return true;
                }
            }
        }
        else
        {
            // Position occupied by enemy - swap places without visible indication
            vector<BattlePosition> &opponentPositions = isPlayerEntity ? enemyPositions : playerPositions;
            int currentPosition = -1;
            for (auto &pos : sameSidePositions)
            {
                if (pos.entity == entity)
                {
                    currentPosition = pos.position;
                    break;
                }
            }

            if (currentPosition == -1)
                return false;

            // Find enemy and swap positions
            for (auto &pos : opponentPositions)
            {
                if (pos.position == newPosition && pos.entity)
                {
                    pos.position = currentPosition;
                    break;
                }
            }

            // Update current character's position
            for (auto &pos : sameSidePositions)
            {
                if (pos.entity == entity)
                {
                    pos.position = newPosition;
                    entity->spendStamina();
                    // No visible indication of the swap
                    return true;
                }
            }
        }
    }
    else
    {
        // Move to empty position
        vector<BattlePosition> &positions = isPlayerEntity ? playerPositions : enemyPositions;
        for (auto &pos : positions)
        {
            if (pos.entity == entity)
            {
                pos.position = newPosition;
                entity->spendStamina();
                cout << entity->getName() << " moves to position " << newPosition << "\n";
                return true;
            }
        }
    }

    return false;
}

bool BattleSystem::useItem(Entity *user, int itemIndex)
{
    // Заглушка для использования предметов
    // Реализация будет добавлена при интеграции с инвентарем
    return false;
}

Entity *BattleSystem::getCurrentTurnEntity() const
{
    if (turnOrder.empty() || currentTurnIndex >= turnOrder.size())
        return nullptr;
    return turnOrder[currentTurnIndex];
}

vector<pair<Entity *, int>> BattleSystem::getAvailableTargetsForCurrent() const
{
    Entity *current = getCurrentTurnEntity();
    if (!current)
        return {};

    // Определяем, игрок это или враг
    bool isPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == current)
        {
            isPlayer = true;
            break;
        }
    }

    return getAvailableTargets(current, isPlayer);
}

vector<pair<Entity *, string>> BattleSystem::getAllEntitiesWithStatus() const
{
    vector<pair<Entity *, string>> entities;

    // Добавляем игроков
    for (const auto &pos : playerPositions)
    {
        if (pos.entity)
        {
            string status = "ЖИВ";
            entities.push_back({pos.entity, status});
        }
    }

    // Добавляем врагов
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity)
        {
            string status = "ЖИВ";
            entities.push_back({pos.entity, status});
        }
    }

    return entities;
}

string BattleSystem::getBattleStatus() const
{
    if (!battleActive)
        return "Battle not active";

    string status = "Players:\n";
    for (const auto &pos : playerPositions)
    {
        if (pos.entity)
        {
            Player *player = static_cast<Player *>(pos.entity);
            status += "  " + player->getName() + " (Lv." + to_string(player->getLevel()) + ")\n";
            status += "  " + player->getHealthBarString() + "\n";
            status += "  " + player->getExperienceBarString() + "\n";
            // Add effects
            const vector<Effect> &effects = player->getActiveEffects();
            if (!effects.empty())
            {
                status += "  Effects: ";
                for (size_t i = 0; i < effects.size(); ++i)
                {
                    status += effects[i].name + " (" + to_string(effects[i].duration) + ")";
                    if (i < effects.size() - 1)
                        status += ", ";
                }
                status += "\n";
            }
        }
    }

    status += "\nEnemies:\n";
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity)
        {
            status += "  " + pos.entity->getName() + "\n";
            status += "  " + pos.entity->getHealthBarString() + "\n";
            // Add effects
            const vector<Effect> &effects = pos.entity->getActiveEffects();
            if (!effects.empty())
            {
                status += "  Effects: ";
                for (size_t i = 0; i < effects.size(); ++i)
                {
                    status += effects[i].name + " (" + to_string(effects[i].duration) + ")";
                    if (i < effects.size() - 1)
                        status += ", ";
                }
                status += "\n";
            }
        }
    }

    return status;
}

string BattleSystem::getTurnOrderString() const
{
    if (!battleActive)
        return "";

    string status = "Turn order:\n";
    for (int i = 0; i < turnOrder.size(); ++i)
    {
        string marker = (i == currentTurnIndex) ? " -> " : "    ";
        status += marker + to_string(i + 1) + ". " + turnOrder[i]->getName() + "\n";
    }

    return status;
}

bool BattleSystem::isPlayerVictory() const
{
    // Проверяем, все ли враги мертвы
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
        {
            return false;
        }
    }
    return true;
}

bool BattleSystem::isPlayerDefeat() const
{
    // Проверяем, все ли игроки мертвы
    for (const auto &pos : playerPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
        {
            return false;
        }
    }
    return true;
}

void BattleSystem::printBattlefield() const
{
    cout << "\n=== BATTLEFIELD ===\n";

    // Display in column: friendly positions 4-3-2-1, enemy 1-2-3-4
    cout << "Friendly positions:\n";
    for (int i = 3; i >= 0; --i)
    {
        cout << (i + 1) << " friendly: ";
        string display = "[EMPTY]";
        for (const auto &pos : playerPositions)
        {
            if (pos.position == i)
            {
                if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
                {
                    display = "[" + pos.entity->getName() + "]";
                    break; // Priority to alive
                }
                else if (pos.corpseHP > 0)
                {
                    display = "[CORPSE]";
                }
            }
        }
        cout << display << "\n";
    }

    cout << "\nEnemy positions:\n";
    for (int i = 0; i < 4; ++i)
    {
        cout << (i + 1) << " enemy:     ";
        string display = "[EMPTY]";
        for (const auto &pos : enemyPositions)
        {
            if (pos.position == i)
            {
                if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
                {
                    display = "[" + pos.entity->getName() + "]";
                    break; // Priority to alive
                }
                else if (pos.corpseHP > 0)
                {
                    display = "[CORPSE]";
                }
            }
        }
        cout << display << "\n";
    }
    cout << "=================\n";
}

void BattleSystem::nextTurn()
{
    if (turnOrder.empty())
        return;

    currentTurnIndex++;

    // Если достигли конца очереди, пересчитываем очередь и начинаем с начала
    if (currentTurnIndex >= turnOrder.size())
    {
        calculateTurnOrder();
        currentTurnIndex = 0;
    }

    // Пропускаем мертвых персонажей
    while (currentTurnIndex < turnOrder.size() && turnOrder[currentTurnIndex] && turnOrder[currentTurnIndex]->getCurrentHealthPoint() <= 0)
    {
        cout << turnOrder[currentTurnIndex]->getName() << " is dead, skipping turn.\n";
        currentTurnIndex++;
        if (currentTurnIndex >= turnOrder.size())
        {
            calculateTurnOrder();
            currentTurnIndex = 0;
        }
    }

    // Регенерируем стамину для текущего персонажа, если он существует
    if (currentTurnIndex < turnOrder.size() && turnOrder[currentTurnIndex])
    {
        regenerateStaminaForTurn();
    }
}

void BattleSystem::displayEntityDetails(Entity *entity) const
{
    if (!entity)
        return;

    cout << "\n=== CHARACTERISTICS " << entity->getName() << " ===\n";
    cout << "Health: " << entity->getCurrentHealthPoint() << "/" << entity->getMaxHealthPoint() << "\n";
    cout << "Damage: " << entity->getDamage() << "\n";
    cout << "Defense: " << entity->getDefense() << "\n";
    cout << "Attack: " << entity->getAttack() << "\n";
    cout << "Initiative: " << entity->getInitiative() << "\n";
    cout << "Attack range: " << entity->getAttackRange() << "\n";
    cout << "Stamina: " << entity->getCurrentStamina() << "/" << entity->getMaxStamina() << "\n";

    // Check if entity is a player (check by presence in playerPositions)
    bool isPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == entity)
        {
            isPlayer = true;
            break;
        }
    }

    // Show active effects
    const vector<Effect> &effects = entity->getActiveEffects();
    if (!effects.empty())
    {
        cout << "\nActive effects:\n";
        for (const auto &effect : effects)
        {
            cout << "- " << effect.name << " (" << effect.duration << " turns";
            if (effect.value != 0)
            {
                string sign = (effect.value > 0) ? "+" : "";
                cout << ", " << sign << effect.value;
            }
            cout << ")\n";
        }
    }
    else
    {
        cout << "\nActive effects: None\n";
    }

    if (isPlayer)
    {
        // Show all available hero abilities
        Player *player = static_cast<Player *>(entity);
        const vector<AbilityType> &abilities = player->getAvailableAbilities();

        if (!abilities.empty())
        {
            cout << "\nHero abilities:\n";
            for (int i = 0; i < abilities.size(); ++i)
            {
                const AbilityInfo &info = HeroFactory::getAbilityInfo(abilities[i]);
                cout << i + 1 << ". " << info.name << ": " << info.description << "\n";
                cout << "   Effect: " << info.effect << "\n";
            }
        }
        else
        {
            cout << "\nAbilities: No available abilities\n";
        }
    }
    else
    {
        // For enemies show basic ability
        AbilityType ability = entity->getAbility();
        if (ability != AbilityType::NONE)
        {
            const AbilityInfo &info = HeroFactory::getAbilityInfo(ability);
            cout << "\nAbility: " << info.name << ": " << info.description << "\n";
            cout << "Effect: " << info.effect << "\n";
        }
        else
        {
            cout << "\nAbility: No special ability\n";
        }
    }
    cout << "=====================================\n";
}

string BattleSystem::getAttackDescription(Entity *attacker, Entity *target) const
{
    if (!attacker || !target)
        return "";

    string description = attacker->getName() + " ";

    // Attack descriptions depending on ability type
    AbilityType ability = attacker->getAbility();
    switch (ability)
    {
    case AbilityType::FIRE_DAMAGE:
        description += "spews a stream of flame at ";
        break;
    case AbilityType::ICE_DAMAGE:
        description += "freezes with icy breath ";
        break;
    case AbilityType::LIGHTNING:
        description += "strikes with lightning ";
        break;
    case AbilityType::POISON:
        description += "poisons with venom ";
        break;
    case AbilityType::LIFE_STEAL:
        description += "drains life force from ";
        break;
    case AbilityType::BERSERK:
        description += "rages and crashes into ";
        break;
    case AbilityType::CHARGE:
        description += "charges and slams into ";
        break;
    case AbilityType::MAGIC_MISSILE:
        description += "launches a magic arrow at ";
        break;
    case AbilityType::CHAIN_LIGHTNING:
        description += "calls chain lightning striking ";
        break;
    case AbilityType::FLAME_BURST:
        description += "explodes with fireball ";
        break;
    case AbilityType::SHADOW_STEP:
        description += "emerges from shadow and attacks ";
        break;
    case AbilityType::ARCANE_MISSILE:
        description += "shoots arcane arrow at ";
        break;
    default:
        description += "attacks ";
        break;
    }

    description += target->getName() + "!";
    return description;
}

void BattleSystem::printTurnOrder() const
{
    cout << "\nTurn order:\n";
    for (int i = 0; i < turnOrder.size(); ++i)
    {
        string marker = (i == currentTurnIndex) ? " -> " : "    ";
        cout << marker << i + 1 << ". " << turnOrder[i]->getName()
             << " (Range: " << turnOrder[i]->getAttackRange() << ")\n";
    }
}

void BattleSystem::applyAbilityEffect(Entity *attacker, Entity *target, int damage)
{
    AbilityType ability = attacker->getAbility();
    switch (ability)
    {
    case AbilityType::LIFE_STEAL:
    {
        int healAmount = static_cast<int>(damage * 0.5); // 50% of dealt damage
        if (healAmount > 0)
        {
            attacker->heal(healAmount);
            cout << attacker->getName() << " restores " << healAmount << " HP thanks to vampirism!\n";
        }
        break;
    }
    case AbilityType::BERSERK:
    {
        // Temporary damage increase (already accounted in description, but can add buff)
        break;
    }
    case AbilityType::POISON:
    {
        // Poison: damage over time (simplified version)
        int poisonDamage = 5;
        target->takeDamage(poisonDamage);
        cout << target->getName() << " takes " << poisonDamage << " damage from poison!\n";
        break;
    }
    case AbilityType::FIRE_DAMAGE:
    {
        // Fire damage: additional fire damage
        int fireDamage = static_cast<int>(damage * 0.3); // 30% of main damage
        if (fireDamage > 0)
        {
            target->takeDamage(fireDamage);
            cout << target->getName() << " takes " << fireDamage << " additional fire damage!\n";
        }
        break;
    }
    case AbilityType::ICE_DAMAGE:
    {
        // Ice damage: additional damage and slowing
        int iceDamage = static_cast<int>(damage * 0.25); // 25% of main damage
        if (iceDamage > 0)
        {
            target->takeDamage(iceDamage);
            target->setInitiative(max(1, target->getInitiative() - 1));
            cout << target->getName() << " takes " << iceDamage << " ice damage and is slowed!\n";
        }
        break;
    }
    case AbilityType::LIGHTNING:
    {
        // Lightning: chance of chain reaction
        if (rand() % 100 < 20) // 20% chance
        {
            // Find another enemy and deal half damage
            bool isAttackerPlayer = false;
            for (const auto &pos : playerPositions)
            {
                if (pos.entity == attacker)
                {
                    isAttackerPlayer = true;
                    break;
                }
            }
            const vector<BattlePosition> &opponents = isAttackerPlayer ? enemyPositions : playerPositions;
            for (const auto &pos : opponents)
            {
                if (pos.entity && pos.entity != target && pos.entity->getCurrentHealthPoint() > 0)
                {
                    int chainDamage = damage / 2;
                    pos.entity->takeDamage(chainDamage);
                    cout << "Lightning jumps to " << pos.entity->getName() << " for " << chainDamage << " damage!\n";
                    break;
                }
            }
        }
        break;
    }
    // Other abilities can be added similarly
    default:
        break;
    }
}

bool BattleSystem::useAbility(Entity *user, AbilityType ability)
{
    if (!battleActive || !user)
        return false;
    if (user->getCurrentHealthPoint() <= 0)
        return false;

    // Get ability info
    const AbilityInfo &info = HeroFactory::getAbilityInfo(ability);

    // Check stamina
    if (user->getCurrentStamina() < info.staminaCost)
    {
        cout << "Недостаточно стамины для использования способности " << info.name << "! Требуется " << info.staminaCost << ", имеется " << user->getCurrentStamina() << ".\n";
        return false;
    }

    // Check if ability is available for user
    bool isPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == user)
        {
            isPlayer = true;
            break;
        }
    }

    if (isPlayer)
    {
        // For players check available abilities
        Player *player = static_cast<Player *>(user);
        const vector<AbilityType> &available = player->getAvailableAbilities();
        if (find(available.begin(), available.end(), ability) == available.end())
        {
            cout << "Ability unavailable!\n";
            return false;
        }
    }
    else
    {
        // For enemies check basic ability
        if (user->getAbility() != ability)
        {
            cout << "Ability unavailable!\n";
            return false;
        }
    }

    // Apply ability effect
    switch (ability)
    {
    case AbilityType::BERSERK:
    {
        // Increase damage and decrease defense
        user->addEffect(Effect(EffectType::BUFF_DAMAGE, 5, 3, "Berserk"));
        user->addEffect(Effect(EffectType::DEBUFF_DEFENSE, 2, 3, "Berserk"));
        cout << user->getName() << " enters berserk state! Damage +5, defense -2 for 3 turns.\n";
        break;
    }
    case AbilityType::HEALING_WAVE:
    {
        // Heal all allies
        vector<BattlePosition> &allies = isPlayer ? playerPositions : enemyPositions;
        for (auto &pos : allies)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->heal(50);
                cout << pos.entity->getName() << " healed for 50 HP!\n";
            }
        }
        break;
    }
    case AbilityType::TELEPORT:
    {
        // Teleport to random position (simplified version)
        int newPos = rand() % 4;
        movePosition(user, newPos);
        break;
    }
    case AbilityType::FEAR:
    {
        // Fear: reduce initiative and damage of enemies
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->setInitiative(max(1, pos.entity->getInitiative() - 2));
                pos.entity->setDamage(max(1, pos.entity->getDamage() - 3));
                cout << pos.entity->getName() << " frightened! Initiative -2, damage -3.\n";
            }
        }
        break;
    }
    case AbilityType::FIRE_DAMAGE:
    {
        // Огненный урон: наносим урон всем врагам в радиусе
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                int fireDamage = 15;
                pos.entity->takeDamage(fireDamage);
                cout << pos.entity->getName() << " получает " << fireDamage << " огненного урона!\n";
            }
        }
        break;
    }
    case AbilityType::ICE_DAMAGE:
    {
        // Ледяной урон: замораживаем и наносим урон
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                int iceDamage = 12;
                pos.entity->takeDamage(iceDamage);
                pos.entity->setInitiative(max(1, pos.entity->getInitiative() - 3));
                cout << pos.entity->getName() << " получает " << iceDamage << " ледяного урона и замедлен!\n";
            }
        }
        break;
    }
    case AbilityType::LIGHTNING:
    {
        // Молния: наносим урон одному врагу
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                int lightningDamage = 25;
                pos.entity->takeDamage(lightningDamage);
                cout << pos.entity->getName() << " поражен молнией за " << lightningDamage << " урона!\n";
                break; // Только один враг
            }
        }
        break;
    }
    case AbilityType::POISON:
    {
        // Яд: отравляем всех врагов (8 урона в начале каждого их хода на 3 хода)
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->addEffect(Effect(EffectType::POISON_DAMAGE, 8, 3, "Яд"));
                cout << pos.entity->getName() << " отравлен!\n";
            }
        }
        break;
    }
    case AbilityType::LIFE_STEAL:
    {
        // Вампиризм: лечим себя и наносим урон врагу
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                int stealDamage = 20;
                pos.entity->takeDamage(stealDamage);
                user->heal(stealDamage / 2);
                cout << user->getName() << " крадет " << stealDamage << " HP у " << pos.entity->getName() << "!\n";
                break; // Только один враг
            }
        }
        break;
    }
    case AbilityType::HEAL:
    {
        // Лечение: лечим себя
        int healAmount = 40;
        user->heal(healAmount);
        cout << user->getName() << " лечит себя на " << healAmount << " HP!\n";
        break;
    }
    case AbilityType::REGENERATION:
    {
        // Регенерация: лечим себя
        int healAmount = 30;
        user->heal(healAmount);
        cout << user->getName() << " регенерирует " << healAmount << " HP!\n";
        break;
    }
    case AbilityType::FLYING:
    {
        // Полет: перемещаемся на любую позицию
        int newPos = rand() % 4;
        movePosition(user, newPos);
        cout << user->getName() << " взлетает и перемещается!\n";
        break;
    }
    case AbilityType::INVISIBLE:
    {
        // Невидимость: пропускаем ход, но становимся невидимым (упрощенная версия)
        cout << user->getName() << " становится невидимым!\n";
        break;
    }
    case AbilityType::CHARGE:
    {
        // Рывок: перемещение к цели и атака с бонусом
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                // Перемещаемся к цели (упрощенная версия - телепортация)
                int targetPos = pos.position;
                movePosition(user, targetPos);

                // Атака с бонусом урона
                int chargeDamage = static_cast<int>(user->getDamage() * 1.5); // +50% урон
                pos.entity->takeDamage(chargeDamage);

                // Шанс оглушения (снижение инициативы)
                if (rand() % 100 < 30) // 30% шанс
                {
                    pos.entity->setInitiative(max(1, pos.entity->getInitiative() - 2));
                    cout << pos.entity->getName() << " оглушен!\n";
                }

                cout << user->getName() << " совершает рывок и наносит " << chargeDamage << " урона!\n";
                break; // Только одна цель
            }
        }
        break;
    }
    case AbilityType::SHIELD_WALL:
    {
        // Стена щитов: блокирует урон на 2 хода
        // Упрощенная версия: временное увеличение защиты
        user->setDefense(user->getDefense() + 5);
        cout << user->getName() << " создает стену щитов! Защита +5 на 2 хода.\n";
        // TODO: Реализовать таймер для снятия баффа через 2 хода
        break;
    }
    case AbilityType::BATTLE_CRY:
    {
        // Боевой клич: бафф союзников +2 атаки и защиты, страх врагов -2 атаки и инициативы
        vector<BattlePosition> &allies = isPlayer ? playerPositions : enemyPositions;
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;

        // Бафф союзников
        for (auto &pos : allies)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->addEffect(Effect(EffectType::BUFF_DAMAGE, 2, 2, "Боевой клич"));
                pos.entity->addEffect(Effect(EffectType::BUFF_DEFENSE, 2, 2, "Боевой клич"));
                cout << pos.entity->getName() << " воодушевлен боевым кличем!\n";
            }
        }

        // Страх врагов
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->addEffect(Effect(EffectType::DEBUFF_DAMAGE, 2, 2, "Страх"));
                pos.entity->addEffect(Effect(EffectType::DEBUFF_INITIATIVE, 1, 2, "Страх"));
                cout << pos.entity->getName() << " напуган боевым кличем!\n";
            }
        }
        break;
    }
    case AbilityType::COMMAND:
    {
        // Команда: бафф союзников +2 инициатива, +1 урон
        vector<BattlePosition> &allies = isPlayer ? playerPositions : enemyPositions;
        for (auto &pos : allies)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->setInitiative(pos.entity->getInitiative() + 2);
                pos.entity->setDamage(pos.entity->getDamage() + 1);
                cout << pos.entity->getName() << " получает приказ! Инициатива +2, урон +1.\n";
            }
        }
        break;
    }
    case AbilityType::FROST_ARMOR:
    {
        // Ледяная броня: защита +5, замедление врагов
        user->setDefense(user->getDefense() + 5);
        cout << user->getName() << " покрывается ледяной броней! Защита +5.\n";

        // Замедление врагов
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->setInitiative(max(1, pos.entity->getInitiative() - 2));
                cout << pos.entity->getName() << " замедлен ледяной броней!\n";
            }
        }
        break;
    }
    case AbilityType::STEALTH:
    {
        // Скрытность: невидимость + критический удар x2
        cout << user->getName() << " скрывается в тенях!\n";
        // Упрощенная версия: следующая атака будет критической
        // TODO: Реализовать флаг stealth для следующей атаки
        break;
    }
    case AbilityType::SHADOW_STEP:
    {
        // Теневой шаг: телепортация + гарантированный удар
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                // Телепортация к цели
                int targetPos = pos.position;
                movePosition(user, targetPos);

                // Гарантированный удар (игнорируем защиту)
                int shadowDamage = user->getDamage() * 2; // x2 урон
                pos.entity->takeDamage(shadowDamage);
                cout << user->getName() << " выныривает из тени и наносит " << shadowDamage << " урона!\n";

                // Проверка на смерть и окончание боя
                removeDeadEntities();
                if (isPlayerVictory() || isPlayerDefeat())
                {
                    battleActive = false;
                }
                break;
            }
        }
        break;
    }
    case AbilityType::ARCANE_MISSILE:
    {
        // Магический снаряд: урон 15-25, игнорирует защиту
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                int arcaneDamage = 15 + (rand() % 11); // 15-25
                pos.entity->takeDamage(arcaneDamage);
                cout << user->getName() << " запускает магический снаряд за " << arcaneDamage << " урона!\n";
                break; // Одна цель
            }
        }
        break;
    }
    case AbilityType::CHAIN_LIGHTNING:
    {
        // Цепная молния: урон 20, цепная реакция на 3 цели
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        int chainCount = 0;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && chainCount < 3)
            {
                pos.entity->takeDamage(20);
                cout << pos.entity->getName() << " поражен цепной молнией за 20 урона!\n";
                chainCount++;
            }
        }
        break;
    }
    case AbilityType::FLAME_BURST:
    {
        // Взрыв пламени: урон 25 по области 3x3
        // Упрощенная версия: урон всем врагам
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0)
            {
                pos.entity->takeDamage(25);
                cout << pos.entity->getName() << " получает 25 урона от взрыва пламени!\n";
            }
        }
        break;
    }
    case AbilityType::BLOOD_RITUAL:
    {
        // Кровавый ритуал: жертва 30 HP за урон +50% на 3 хода
        if (user->getCurrentHealthPoint() > 30)
        {
            user->takeDamage(30);
            user->setDamage(static_cast<int>(user->getDamage() * 1.5));
            cout << user->getName() << " проводит кровавый ритуал! Жертвует 30 HP, урон +50%.\n";
            // TODO: Реализовать таймер для снятия баффа через 3 хода
        }
        else
        {
            cout << "Недостаточно здоровья для ритуала!\n";
            return false;
        }
        break;
    }
    default:
        cout << "Способность не реализована.\n";
        return false;
    }

    // Трата стамины
    user->spendStamina(info.staminaCost);

    return true;
}

void BattleSystem::shiftPositionsAfterDeath(vector<BattlePosition> &positions, int deadPosition)
{
    // Сдвигаем всех персонажей на позициях выше deadPosition на одну позицию ближе к бою
    for (int i = deadPosition + 1; i < 4; ++i)
    {
        // Найти позицию i
        auto it = find_if(positions.begin(), positions.end(),
                          [i](const BattlePosition &pos)
                          { return pos.position == i && pos.entity != nullptr; });
        if (it != positions.end())
        {
            it->position = i - 1;
        }
    }
}

bool BattleSystem::skipHalfTurn(Entity *entity)
{
    if (!battleActive || !entity)
        return false;

    // Уменьшаем стамину наполовину (округляем вниз)
    int staminaCost = max(1, entity->getCurrentStamina() / 2);
    entity->setCurrentStamina(entity->getCurrentStamina() - staminaCost);

    cout << entity->getName() << " пропускает половину хода! Потеряно " << staminaCost << " стамины.\n";

    return true;
}
