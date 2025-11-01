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
    // Очистка предыдущего боя
    playerPositions.clear();
    enemyPositions.clear();
    turnOrder.clear();
    currentTurnIndex = 0;
    battleActive = true;

    // Расстановка игроков (позиции 0-3)
    for (size_t i = 0; i < players.size() && i < 4; ++i)
    {
        playerPositions.push_back(BattlePosition(players[i], static_cast<int>(i), false));
    }

    // Расстановка врагов (позиции 0-3)
    for (size_t i = 0; i < enemies.size() && i < 4; ++i)
    {
        enemyPositions.push_back(BattlePosition(enemies[i], static_cast<int>(i), false));
    }

    // Регенерация стамины для всех участников
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

    // Расчет очереди ходов
    calculateTurnOrder();

    cout << "=== БОЙ НАЧАЛСЯ ===\n";
    printBattlefield();
}

void BattleSystem::endBattle()
{
    cout << "[DEBUG] BattleSystem::endBattle() called\n";
    battleActive = false;
    playerPositions.clear();
    enemyPositions.clear();
    turnOrder.clear();
    currentTurnIndex = 0;
    cout << "=== БОЙ ЗАКОНЧЕН ===\n";
    cout << "[DEBUG] BattleSystem::endBattle() completed\n";
}

void BattleSystem::calculateTurnOrder()
{
    turnOrder.clear();
    vector<TurnInfo> allEntities;

    // Добавляем всех живых игроков
    for (const auto &pos : playerPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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

bool BattleSystem::canAttackCorpse(Entity *attacker, int position) const
{
    if (!attacker || position < 0 || position > 3)
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

    // Определяем, на какой стороне находится труп
    const vector<BattlePosition> &opponentPositions = isAttackerPlayer ? enemyPositions : playerPositions;
    bool isCorpseAtPosition = false;
    for (const auto &pos : opponentPositions)
    {
        if (pos.position == position && pos.isCorpse && pos.corpseHP > 0)
        {
            isCorpseAtPosition = true;
            break;
        }
    }

    if (!isCorpseAtPosition)
        return false;

    // Проверяем расстояние
    int distance = abs(attackerPos - position);

    // Для range=0: только позиция напротив
    if (attackRange == 0)
    {
        return distance == 0;
    }

    // Для range=1: аналогично живым целям
    if (attackRange == 1)
    {
        if (isAttackerPlayer)
        {
            if (attackerPos <= 1)
            {
                return true; // Front line can attack both lines
            }
            else
            {
                return true;
            }
        }
        else
        {
            if (attackerPos <= 1)
            {
                return true; // Front line can attack both lines
            }
            else
            {
                return true;
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
    for (const auto &pos : positions)
    {
        if (pos.position == position && pos.isCorpse)
        {
            return true; // Позиция заблокирована трупом
        }
    }
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

vector<Entity *> BattleSystem::getAvailableTargets(Entity *attacker, bool isPlayerAttacker) const
{
    vector<Entity *> targets;

    if (!attacker)
        return targets;

    const vector<BattlePosition> &opponentPositions = isPlayerAttacker ? enemyPositions : playerPositions;

    for (const auto &pos : opponentPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
        {
            if (canAttackTarget(attacker, pos.entity))
            {
                targets.push_back(pos.entity);
            }
        }
        // Include corpses with HP > 0 as targets
        else if (pos.isCorpse && pos.corpseHP > 0)
        {
            // Create a dummy entity to represent the corpse
            Entity *corpseDummy = new Entity("Труп", pos.corpseHP, 0, 0, 0, 0, 0, 0, 0, AbilityType::NONE, 0.0);
            targets.push_back(corpseDummy);
        }
    }

    return targets;
}

void BattleSystem::removeDeadEntities()
{
    // Превращаем мертвых игроков в трупы
    for (auto &pos : playerPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() <= 0 && !pos.isCorpse)
        {
            pos.isCorpse = true;
            pos.corpseHP = 50; // Устанавливаем HP для трупа
            cout << pos.entity->getName() << " пал в бою!\n";
        }
    }

    // Превращаем мертвых врагов в трупы
    for (auto &pos : enemyPositions)
    {
        if (pos.entity && pos.entity->getCurrentHealthPoint() <= 0 && !pos.isCorpse)
        {
            pos.isCorpse = true;
            pos.corpseHP = 50; // Устанавливаем HP для трупа
            cout << pos.entity->getName() << " повержен!\n";
        }
    }
}

void BattleSystem::regenerateStaminaForTurn()
{
    Entity *current = getCurrentTurnEntity();
    if (current)
    {
        current->regenerateStamina();
    }
}

bool BattleSystem::attack(Entity *attacker, Entity *target)
{
    if (!battleActive || !attacker || !target)
        return false;
    if (attacker->getCurrentStamina() <= 0)
        return false;

    // Check if target is a corpse dummy
    bool isCorpseAttack = (target->getName() == "Труп");

    if (isCorpseAttack)
    {
        // Find the corpse position
        bool isPlayerAttacker = false;
        for (const auto &pos : playerPositions)
        {
            if (pos.entity == attacker)
            {
                isPlayerAttacker = true;
                break;
            }
        }

        vector<BattlePosition> &opponentPositions = isPlayerAttacker ? enemyPositions : playerPositions;
        int corpsePosition = -1;
        for (const auto &pos : opponentPositions)
        {
            if (pos.isCorpse && pos.corpseHP == target->getCurrentHealthPoint())
            {
                corpsePosition = pos.position;
                break;
            }
        }

        if (corpsePosition == -1 || !canAttackCorpse(attacker, corpsePosition))
            return false;

        // Calculate damage
        int damage = attacker->attack(0); // Corpses have no defense
        if (damage > 0)
        {
            // Apply damage to corpse
            for (BattlePosition &pos : opponentPositions)
            {
                if (pos.position == corpsePosition && pos.isCorpse)
                {
                    pos.corpseHP -= damage;
                    if (pos.corpseHP <= 0)
                    {
                        delete pos.entity;
                        pos.corpseHP = 0;
                        pos.isCorpse = false;
                        pos.entity = nullptr;
                        cout << "Труп на позиции " << corpsePosition << " уничтожен!\n";
                    }
                    break;
                }
            }

            cout << attacker->getName() << " атакует труп на позиции " << corpsePosition << "!\n";
            cout << "Наносит " << damage << " урона трупу!\n";
        }
    }
    else
    {
        if (!canAttackTarget(attacker, target))
            return false;

        // Расчет урона
        int damage = attacker->attack(target->getDefense());
        target->takeDamage(damage);

        // Применение эффектов способности
        applyAbilityEffect(attacker, target, damage);

        cout << getAttackDescription(attacker, target) << "\n";
        cout << "Наносит " << damage << " урона!\n";

        // Проверка на смерть
        removeDeadEntities();

        // Пересчет очереди ходов после смерти
        if (target->getCurrentHealthPoint() <= 0)
        {
            calculateTurnOrder();
            // Корректируем currentTurnIndex, чтобы он не выходил за границы
            if (currentTurnIndex >= turnOrder.size())
            {
                currentTurnIndex = 0;
            }
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

    const vector<BattlePosition> &sameSidePositions = isPlayerEntity ? playerPositions : enemyPositions;
    const vector<BattlePosition> &opponentPositions = isPlayerEntity ? enemyPositions : playerPositions;

    // Проверяем, заблокирована ли новая позиция
    if (isPositionBlocked(newPosition, sameSidePositions))
    {
        cout << "Позиция заблокирована трупом!\n";
        return false;
    }

    // Проверяем, есть ли союзник на новой позиции
    bool hasAllyAtPosition = false;
    for (const auto &pos : sameSidePositions)
    {
        if (pos.position == newPosition && pos.entity && pos.entity != entity && !pos.isCorpse)
        {
            hasAllyAtPosition = true;
            break;
        }
    }

    if (hasAllyAtPosition)
    {
        // Меняем местами с союзником
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

        // Находим союзника и меняем позиции
        for (auto &pos : positions)
        {
            if (pos.position == newPosition && pos.entity && pos.entity != entity && !pos.isCorpse)
            {
                pos.position = currentPosition;
                break;
            }
        }

        // Обновляем позицию текущего персонажа
        for (auto &pos : positions)
        {
            if (pos.entity == entity)
            {
                pos.position = newPosition;
                entity->spendStamina();
                cout << entity->getName() << " меняется местами с союзником на позицию " << newPosition << "\n";
                return true;
            }
        }
    }
    else
    {
        // Перемещаемся на пустую позицию (всегда разрешено)
        vector<BattlePosition> &positions = isPlayerEntity ? playerPositions : enemyPositions;
        for (auto &pos : positions)
        {
            if (pos.entity == entity)
            {
                pos.position = newPosition;
                entity->spendStamina();
                cout << entity->getName() << " перемещается на позицию " << newPosition << "\n";
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

vector<Entity *> BattleSystem::getAvailableTargetsForCurrent() const
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
            string status = pos.isCorpse ? "ТРУП" : "ЖИВ";
            entities.push_back({pos.entity, status});
        }
    }

    // Добавляем врагов
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity)
        {
            string status = pos.isCorpse ? "ТРУП" : "ЖИВ";
            entities.push_back({pos.entity, status});
        }
    }

    return entities;
}

string BattleSystem::getBattleStatus() const
{
    if (!battleActive)
        return "Бой не активен";

    string status = "Игроки: ";
    for (const auto &pos : playerPositions)
    {
        if (pos.entity)
        {
            status += pos.entity->getName() + "(" +
                      to_string(pos.entity->getCurrentHealthPoint()) + " HP) ";
        }
    }

    status += "\nВраги: ";
    for (const auto &pos : enemyPositions)
    {
        if (pos.entity)
        {
            status += pos.entity->getName() + "(" +
                      to_string(pos.entity->getCurrentHealthPoint()) + " HP) ";
        }
    }

    // Добавляем очередь ходов
    status += "\n\nОчередь ходов:\n";
    for (size_t i = 0; i < turnOrder.size(); ++i)
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
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
        {
            return false;
        }
    }
    return true;
}

void BattleSystem::printBattlefield() const
{
    cout << "\n=== ПОЛЕ БОЯ ===\n";

    // Отображение в столбик: дружественные позиции 4-3-2-1, вражеские 1-2-3-4
    cout << "Дружественные позиции:\n";
    for (int i = 3; i >= 0; --i)
    {
        cout << (i + 1) << " дружественная: ";
        bool found = false;
        for (const auto &pos : playerPositions)
        {
            if (pos.position == i)
            {
                if (pos.isCorpse)
                {
                    cout << "[ТРУП]";
                }
                else if (pos.entity)
                {
                    cout << "[" << pos.entity->getName() << "]";
                }
                else
                {
                    cout << "[ПУСТО]";
                }
                found = true;
                break;
            }
        }
        if (!found)
            cout << "[ПУСТО]";
        cout << "\n";
    }

    cout << "\nВражеские позиции:\n";
    for (int i = 0; i < 4; ++i)
    {
        cout << (i + 1) << " вражеская:     ";
        bool found = false;
        for (const auto &pos : enemyPositions)
        {
            if (pos.position == i)
            {
                if (pos.isCorpse)
                {
                    cout << "[ТРУП]";
                }
                else if (pos.entity)
                {
                    cout << "[" << pos.entity->getName() << "]";
                }
                else
                {
                    cout << "[ПУСТО]";
                }
                found = true;
                break;
            }
        }
        if (!found)
            cout << "[ПУСТО]";
        cout << "\n";
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

    cout << "\n=== ХАРАКТЕРИСТИКИ " << entity->getName() << " ===\n";
    cout << "Здоровье: " << entity->getCurrentHealthPoint() << "/" << entity->getMaxHealthPoint() << "\n";
    cout << "Урон: " << entity->getDamage() << "\n";
    cout << "Защита: " << entity->getDefense() << "\n";
    cout << "Атака: " << entity->getAttack() << "\n";
    cout << "Инициатива: " << entity->getInitiative() << "\n";
    cout << "Дальность атаки: " << entity->getAttackRange() << "\n";
    cout << "Выносливость: " << entity->getCurrentStamina() << "/" << entity->getMaxStamina() << "\n";

    // Проверяем, является ли сущность игроком (проверяем по наличию в playerPositions)
    bool isPlayer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == entity)
        {
            isPlayer = true;
            break;
        }
    }

    if (isPlayer)
    {
        // Показываем все доступные способности героя
        Player *player = static_cast<Player *>(entity);
        const vector<AbilityType> &abilities = player->getAvailableAbilities();

        if (!abilities.empty())
        {
            cout << "\nСпособности героя:\n";
            for (size_t i = 0; i < abilities.size(); ++i)
            {
                const AbilityInfo &info = HeroFactory::getAbilityInfo(abilities[i]);
                cout << i + 1 << ". " << info.name << ": " << info.description << "\n";
                cout << "   Эффект: " << info.effect << "\n";
            }
        }
        else
        {
            cout << "\nСпособности: Нет доступных способностей\n";
        }
    }
    else
    {
        // Для врагов показываем базовую способность
        AbilityType ability = entity->getAbility();
        if (ability != AbilityType::NONE)
        {
            const AbilityInfo &info = HeroFactory::getAbilityInfo(ability);
            cout << "\nСпособность: " << info.name << ": " << info.description << "\n";
            cout << "Эффект: " << info.effect << "\n";
        }
        else
        {
            cout << "\nСпособность: Нет специальной способности\n";
        }
    }
    cout << "=====================================\n";
}

string BattleSystem::getAttackDescription(Entity *attacker, Entity *target) const
{
    if (!attacker || !target)
        return "";

    string description = attacker->getName() + " ";

    // Описания атак в зависимости от типа способности
    AbilityType ability = attacker->getAbility();
    switch (ability)
    {
    case AbilityType::FIRE_DAMAGE:
        description += "извергает поток пламени на ";
        break;
    case AbilityType::ICE_DAMAGE:
        description += "замораживает ледяным дыханием ";
        break;
    case AbilityType::LIGHTNING:
        description += "поражает молнией ";
        break;
    case AbilityType::POISON:
        description += "отравляет ядом ";
        break;
    case AbilityType::LIFE_STEAL:
        description += "высасывает жизненную силу из ";
        break;
    case AbilityType::BERSERK:
        description += "в ярости обрушивается на ";
        break;
    case AbilityType::CHARGE:
        description += "с разбега врезается в ";
        break;
    case AbilityType::MAGIC_MISSILE:
        description += "запускает магическую стрелу в ";
        break;
    case AbilityType::CHAIN_LIGHTNING:
        description += "вызывает цепную молнию, ударяющую по ";
        break;
    case AbilityType::FLAME_BURST:
        description += "взрывает огненным шаром ";
        break;
    case AbilityType::SHADOW_STEP:
        description += "выныривает из тени и атакует ";
        break;
    case AbilityType::ARCANE_MISSILE:
        description += "пускает чародейскую стрелу в ";
        break;
    default:
        description += "атакует ";
        break;
    }

    description += target->getName() + "!";
    return description;
}

void BattleSystem::printTurnOrder() const
{
    cout << "\nОчередь ходов:\n";
    for (size_t i = 0; i < turnOrder.size(); ++i)
    {
        string marker = (i == currentTurnIndex) ? " -> " : "    ";
        cout << marker << i + 1 << ". " << turnOrder[i]->getName()
             << " (Дальность: " << turnOrder[i]->getAttackRange() << ")\n";
    }
}

void BattleSystem::applyAbilityEffect(Entity *attacker, Entity *target, int damage)
{
    AbilityType ability = attacker->getAbility();
    switch (ability)
    {
    case AbilityType::LIFE_STEAL:
    {
        int healAmount = static_cast<int>(damage * 0.5); // 50% от нанесенного урона
        if (healAmount > 0)
        {
            attacker->heal(healAmount);
            cout << attacker->getName() << " восстанавливает " << healAmount << " HP благодаря вампиризму!\n";
        }
        break;
    }
    case AbilityType::BERSERK:
    {
        // Временное увеличение урона (уже учтено в описании, но можно добавить бафф)
        break;
    }
    case AbilityType::POISON:
    {
        // Отравление: урон со временем (упрощенная версия)
        int poisonDamage = 5;
        target->takeDamage(poisonDamage);
        cout << target->getName() << " получает " << poisonDamage << " урона от яда!\n";
        break;
    }
    // Другие способности можно добавить по аналогии
    default:
        break;
    }
}

bool BattleSystem::useAbility(Entity *user, AbilityType ability)
{
    if (!battleActive || !user)
        return false;
    if (user->getCurrentStamina() <= 0)
        return false;

    // Проверяем, доступна ли способность для пользователя
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
        // Для игроков проверяем доступные способности
        Player *player = static_cast<Player *>(user);
        const vector<AbilityType> &available = player->getAvailableAbilities();
        if (find(available.begin(), available.end(), ability) == available.end())
        {
            cout << "Способность недоступна!\n";
            return false;
        }
    }
    else
    {
        // Для врагов проверяем базовую способность
        if (user->getAbility() != ability)
        {
            cout << "Способность недоступна!\n";
            return false;
        }
    }

    // Применяем эффект способности
    switch (ability)
    {
    case AbilityType::BERSERK:
    {
        // Увеличиваем урон и уменьшаем защиту
        user->setDamage(user->getDamage() + 5);
        user->setDefense(max(0, user->getDefense() - 2));
        cout << user->getName() << " входит в состояние берсерка! Урон +5, защита -2.\n";
        break;
    }
    case AbilityType::HEALING_WAVE:
    {
        // Исцеляем всех союзников
        vector<BattlePosition> &allies = isPlayer ? playerPositions : enemyPositions;
        for (auto &pos : allies)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
            {
                pos.entity->heal(50);
                cout << pos.entity->getName() << " исцелен на 50 HP!\n";
            }
        }
        break;
    }
    case AbilityType::TELEPORT:
    {
        // Телепортация на случайную позицию (упрощенная версия)
        int newPos = rand() % 4;
        movePosition(user, newPos);
        break;
    }
    case AbilityType::FEAR:
    {
        // Страх: снижаем инициативу и урон врагов
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
            {
                pos.entity->setInitiative(max(1, pos.entity->getInitiative() - 2));
                pos.entity->setDamage(max(1, pos.entity->getDamage() - 3));
                cout << pos.entity->getName() << " напуган! Инициатива -2, урон -3.\n";
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
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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
        // Яд: отравляем всех врагов
        vector<BattlePosition> &opponents = isPlayer ? enemyPositions : playerPositions;
        for (auto &pos : opponents)
        {
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
            {
                int poisonDamage = 8;
                pos.entity->takeDamage(poisonDamage);
                cout << pos.entity->getName() << " отравлен и получает " << poisonDamage << " урона!\n";
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
            if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse)
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
    default:
        cout << "Способность не реализована.\n";
        return false;
    }

    // Трата стамины
    user->spendStamina();

    return true;
}

bool BattleSystem::destroyCorpse(Entity *destroyer, int position)
{
    if (!battleActive || !destroyer || position < 0 || position > 3)
        return false;
    if (destroyer->getCurrentStamina() <= 0)
        return false;

    // Проверяем, может ли разрушитель атаковать труп на этой позиции
    if (!canAttackCorpse(destroyer, position))
        return false;

    // Определяем, на какой стороне находится разрушитель
    bool isPlayerDestroyer = false;
    for (const auto &pos : playerPositions)
    {
        if (pos.entity == destroyer)
        {
            isPlayerDestroyer = true;
            break;
        }
    }

    // Проверяем, есть ли труп на указанной позиции
    vector<BattlePosition> &positions = isPlayerDestroyer ? enemyPositions : playerPositions;
    bool corpseFound = false;
    for (auto &pos : positions)
    {
        if (pos.position == position && pos.isCorpse)
        {
            // Удаляем труп
            delete pos.entity;
            pos.isCorpse = false;
            pos.corpseHP = 0;
            pos.entity = nullptr;
            corpseFound = true;
            cout << destroyer->getName() << " уничтожает труп на позиции " << position << "!\n";
            break;
        }
    }

    if (!corpseFound)
    {
        cout << "На позиции " << position << " нет трупа!\n";
        return false;
    }

    // Трата стамины
    destroyer->spendStamina();

    return true;
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
