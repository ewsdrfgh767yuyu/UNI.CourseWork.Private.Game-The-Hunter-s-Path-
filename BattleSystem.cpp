#include "BattleSystem.h"
#include <algorithm>
#include <numeric>
#include <chrono>

BattleSystem::BattleSystem()
    : currentTurnIndex(0), battleActive(false) {
    // Инициализация генератора случайных чисел
    randomGenerator.seed(static_cast<unsigned int>(
        chrono::system_clock::now().time_since_epoch().count()));
}

void BattleSystem::startBattle(const vector<Entity*>& players, const vector<Entity*>& enemies) {
    // Очистка предыдущего боя
    playerPositions.clear();
    enemyPositions.clear();
    turnOrder.clear();
    currentTurnIndex = 0;
    battleActive = true;

    // Расстановка игроков (позиции 0-3)
    for (size_t i = 0; i < players.size() && i < 4; ++i) {
        playerPositions.push_back(BattlePosition(players[i], static_cast<int>(i), false));
    }

    // Расстановка врагов (позиции 0-3)
    for (size_t i = 0; i < enemies.size() && i < 4; ++i) {
        enemyPositions.push_back(BattlePosition(enemies[i], static_cast<int>(i), false));
    }

    // Регенерация стамины для всех участников
    for (auto& pos : playerPositions) {
        if (pos.entity) pos.entity->regenerateStamina();
    }
    for (auto& pos : enemyPositions) {
        if (pos.entity) pos.entity->regenerateStamina();
    }

    // Расчет очереди ходов
    calculateTurnOrder();

    cout << "=== БОЙ НАЧАЛСЯ ===\n";
    printBattlefield();
}

void BattleSystem::endBattle() {
    battleActive = false;
    playerPositions.clear();
    enemyPositions.clear();
    turnOrder.clear();
    currentTurnIndex = 0;
    cout << "=== БОЙ ЗАКОНЧЕН ===\n";
}

void BattleSystem::calculateTurnOrder() {
    turnOrder.clear();
    vector<TurnInfo> allEntities;

    // Добавляем всех живых игроков
    for (const auto& pos : playerPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse) {
            int turnWeight = pos.entity->getInitiative() / 10;
            for (int i = 0; i < turnWeight; ++i) {
                allEntities.push_back(TurnInfo(pos.entity, pos.entity->getInitiative() + i));
            }
        }
    }

    // Добавляем всех живых врагов
    for (const auto& pos : enemyPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse) {
            int turnWeight = pos.entity->getInitiative() / 10;
            for (int i = 0; i < turnWeight; ++i) {
                allEntities.push_back(TurnInfo(pos.entity, pos.entity->getInitiative() + i));
            }
        }
    }

    // Сортировка по приоритету (инициатива)
    sort(allEntities.begin(), allEntities.end(),
         [](const TurnInfo& a, const TurnInfo& b) {
             return a.priority > b.priority;
         });

    // Формируем очередь ходов
    for (const auto& info : allEntities) {
        turnOrder.push_back(info.entity);
    }
}

bool BattleSystem::canAttackTarget(Entity* attacker, Entity* target) const {
    if (!attacker || !target) return false;

    int attackerPos = getEntityPosition(attacker);
    int targetPos = getEntityPosition(target);
    int attackRange = attacker->getAttackRange();

    if (attackerPos == -1 || targetPos == -1) return false;

    // Определяем, на какой стороне находится атакующий
    bool isAttackerPlayer = false;
    for (const auto& pos : playerPositions) {
        if (pos.entity == attacker) {
            isAttackerPlayer = true;
            break;
        }
    }

    // Определяем, на какой стороне находится цель
    bool isTargetPlayer = false;
    for (const auto& pos : playerPositions) {
        if (pos.entity == target) {
            isTargetPlayer = true;
            break;
        }
    }

    // Нельзя атаковать союзников
    if (isAttackerPlayer == isTargetPlayer) return false;

    // Проверяем, является ли цель доступной для атаки
    int distance = abs(attackerPos - targetPos);

    // Для range=0: только позиция напротив (0-0, 1-1, 2-2, 3-3)
    if (attackRange == 0) {
        return distance == 0;
    }

    // Для range=1: можно атаковать через одного союзника
    // Если атакующий на передней линии (0-1), может атаковать переднюю линию врага (0-1)
    // Если атакующий на задней линии (2-3), может атаковать заднюю линию врага (2-3) и переднюю (0-1)
    if (attackRange == 1) {
        if (isAttackerPlayer) {
            // Игрок на передней линии может атаковать только переднюю линию врага
            if (attackerPos <= 1) {
                return targetPos <= 1;
            }
            // Игрок на задней линии может атаковать обе линии врага
            else {
                return targetPos <= 1; // Через одного союзника
            }
        } else {
            // Враг на передней линии может атаковать только переднюю линию игрока
            if (attackerPos <= 1) {
                return targetPos <= 1;
            }
            // Враг на задней линии может атаковать обе линии игрока
            else {
                return targetPos <= 1; // Через одного союзника
            }
        }
    }

    // Для range>=2: все позиции
    return distance <= attackRange;
}

int BattleSystem::getEntityPosition(Entity* entity) const {
    for (const auto& pos : playerPositions) {
        if (pos.entity == entity) return pos.position;
    }
    for (const auto& pos : enemyPositions) {
        if (pos.entity == entity) return pos.position;
    }
    return -1;
}

bool BattleSystem::isPositionBlocked(int position, const vector<BattlePosition>& positions) const {
    for (const auto& pos : positions) {
        if (pos.position == position && pos.isCorpse) {
            return true; // Позиция заблокирована трупом
        }
    }
    return false;
}

bool BattleSystem::hasEmptyPositions(const vector<BattlePosition>& positions) const {
    // Проверяем, есть ли пустые позиции (без живых сущностей и трупов)
    for (int i = 0; i < 4; ++i) {
        bool occupied = false;
        for (const auto& pos : positions) {
            if (pos.position == i) {
                occupied = true;
                break;
            }
        }
        if (!occupied) {
            return true; // Есть пустая позиция
        }
    }
    return false;
}

vector<Entity*> BattleSystem::getAvailableTargets(Entity* attacker, bool isPlayerAttacker) const {
    vector<Entity*> targets;

    if (!attacker) return targets;

    const vector<BattlePosition>& opponentPositions = isPlayerAttacker ? enemyPositions : playerPositions;

    for (const auto& pos : opponentPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse) {
            if (canAttackTarget(attacker, pos.entity)) {
                targets.push_back(pos.entity);
            }
        }
    }

    return targets;
}

void BattleSystem::removeDeadEntities() {
    // Превращаем мертвых игроков в трупы
    for (auto& pos : playerPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() <= 0 && !pos.isCorpse) {
            pos.isCorpse = true;
            cout << pos.entity->getName() << " пал в бою!\n";
        }
    }

    // Превращаем мертвых врагов в трупы
    for (auto& pos : enemyPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() <= 0 && !pos.isCorpse) {
            pos.isCorpse = true;
            cout << pos.entity->getName() << " повержен!\n";
        }
    }
}

void BattleSystem::regenerateStaminaForTurn() {
    Entity* current = getCurrentTurnEntity();
    if (current) {
        current->regenerateStamina();
    }
}

bool BattleSystem::attack(Entity* attacker, Entity* target) {
    if (!battleActive || !attacker || !target) return false;
    if (attacker->getCurrentStamina() <= 0) return false;
    if (!canAttackTarget(attacker, target)) return false;

    // Расчет урона
    int damage = attacker->attack(target->getDefense());
    target->takeDamage(damage);

    // Трата стамины
    attacker->spendStamina();

    cout << getAttackDescription(attacker, target) << "\n";
    cout << "Наносит " << damage << " урона!\n";

    // Проверка на смерть
    removeDeadEntities();

    // Пересчет очереди ходов после смерти
    if (target->getCurrentHealthPoint() <= 0) {
        calculateTurnOrder();
    }

    // Проверяем, закончен ли бой
    if (isPlayerVictory() || isPlayerDefeat()) {
        battleActive = false;
    }

    return true;
}

bool BattleSystem::movePosition(Entity* entity, int newPosition) {
    if (!battleActive || !entity || newPosition < 0 || newPosition > 3) return false;
    if (entity->getCurrentStamina() <= 0) return false;

    // Определяем, игрок это или враг
    bool isPlayerEntity = false;
    for (const auto& pos : playerPositions) {
        if (pos.entity == entity) {
            isPlayerEntity = true;
            break;
        }
    }

    const vector<BattlePosition>& sameSidePositions = isPlayerEntity ? playerPositions : enemyPositions;
    const vector<BattlePosition>& opponentPositions = isPlayerEntity ? enemyPositions : playerPositions;

    // Проверяем, есть ли пустые позиции на своей стороне
    if (hasEmptyPositions(sameSidePositions)) {
        // Если есть пустые позиции, перемещение запрещено - только смена местами
        cout << "Перемещение запрещено! Можно только менять местами персонажей.\n";
        return false;
    }

    // Проверяем, заблокирована ли новая позиция
    if (isPositionBlocked(newPosition, sameSidePositions)) {
        cout << "Позиция заблокирована трупом!\n";
        return false;
    }

    // Проверяем, есть ли союзник на новой позиции
    bool hasAllyAtPosition = false;
    for (const auto& pos : sameSidePositions) {
        if (pos.position == newPosition && pos.entity && pos.entity != entity && !pos.isCorpse) {
            hasAllyAtPosition = true;
            break;
        }
    }

    if (!hasAllyAtPosition) {
        cout << "Можно перемещаться только на позицию союзника для смены местами!\n";
        return false;
    }

    // Меняем местами с союзником
    vector<BattlePosition>& positions = isPlayerEntity ? playerPositions : enemyPositions;
    int currentPosition = -1;
    for (auto& pos : positions) {
        if (pos.entity == entity) {
            currentPosition = pos.position;
            break;
        }
    }

    if (currentPosition == -1) return false;

    // Находим союзника и меняем позиции
    for (auto& pos : positions) {
        if (pos.position == newPosition && pos.entity && pos.entity != entity && !pos.isCorpse) {
            pos.position = currentPosition;
            break;
        }
    }

    // Обновляем позицию текущего персонажа
    for (auto& pos : positions) {
        if (pos.entity == entity) {
            pos.position = newPosition;
            entity->spendStamina();
            cout << entity->getName() << " меняется местами с союзником на позицию " << newPosition << "\n";
            return true;
        }
    }

    return false;
}

bool BattleSystem::useItem(Entity* user, int itemIndex) {
    // Заглушка для использования предметов
    // Реализация будет добавлена при интеграции с инвентарем
    return false;
}

Entity* BattleSystem::getCurrentTurnEntity() const {
    if (turnOrder.empty() || currentTurnIndex >= turnOrder.size()) return nullptr;
    return turnOrder[currentTurnIndex];
}

vector<Entity*> BattleSystem::getAvailableTargetsForCurrent() const {
    Entity* current = getCurrentTurnEntity();
    if (!current) return {};

    // Определяем, игрок это или враг
    bool isPlayer = false;
    for (const auto& pos : playerPositions) {
        if (pos.entity == current) {
            isPlayer = true;
            break;
        }
    }

    return getAvailableTargets(current, isPlayer);
}

vector<pair<Entity*, string>> BattleSystem::getAllEntitiesWithStatus() const {
    vector<pair<Entity*, string>> entities;

    // Добавляем игроков
    for (const auto& pos : playerPositions) {
        if (pos.entity) {
            string status = pos.isCorpse ? "ТРУП" : "ЖИВ";
            entities.push_back({pos.entity, status});
        }
    }

    // Добавляем врагов
    for (const auto& pos : enemyPositions) {
        if (pos.entity) {
            string status = pos.isCorpse ? "ТРУП" : "ЖИВ";
            entities.push_back({pos.entity, status});
        }
    }

    return entities;
}

string BattleSystem::getBattleStatus() const {
    if (!battleActive) return "Бой не активен";

    string status = "Игроки: ";
    for (const auto& pos : playerPositions) {
        if (pos.entity) {
            status += pos.entity->getName() + "(" +
                     to_string(pos.entity->getCurrentHealthPoint()) + " HP) ";
        }
    }

    status += "\nВраги: ";
    for (const auto& pos : enemyPositions) {
        if (pos.entity) {
            status += pos.entity->getName() + "(" +
                     to_string(pos.entity->getCurrentHealthPoint()) + " HP) ";
        }
    }

    return status;
}

bool BattleSystem::isPlayerVictory() const {
    // Проверяем, все ли враги мертвы
    for (const auto& pos : enemyPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse) {
            return false;
        }
    }
    return true;
}

bool BattleSystem::isPlayerDefeat() const {
    // Проверяем, все ли игроки мертвы
    for (const auto& pos : playerPositions) {
        if (pos.entity && pos.entity->getCurrentHealthPoint() > 0 && !pos.isCorpse) {
            return false;
        }
    }
    return true;
}

void BattleSystem::printBattlefield() const {
    cout << "\n=== ПОЛЕ БОЯ ===\n";

    // Отображение в столбик: дружественные позиции 4-3-2-1, вражеские 1-2-3-4
    cout << "Дружественные позиции:\n";
    for (int i = 3; i >= 0; --i) {
        cout << (i + 1) << " дружественная: ";
        bool found = false;
        for (const auto& pos : playerPositions) {
            if (pos.position == i) {
                if (pos.isCorpse) {
                    cout << "[ТРУП]";
                } else {
                    cout << "[" << pos.entity->getName() << "]";
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "[ПУСТО]";
        cout << "\n";
    }

    cout << "\nВражеские позиции:\n";
    for (int i = 0; i < 4; ++i) {
        cout << (i + 1) << " вражеская:     ";
        bool found = false;
        for (const auto& pos : enemyPositions) {
            if (pos.position == i) {
                if (pos.isCorpse) {
                    cout << "[ТРУП]";
                } else {
                    cout << "[" << pos.entity->getName() << "]";
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "[ПУСТО]";
        cout << "\n";
    }
    cout << "=================\n";
}

void BattleSystem::nextTurn() {
    if (turnOrder.empty()) return;
    
    currentTurnIndex++;
    
    // Если достигли конца очереди, пересчитываем очередь и начинаем с начала
    if (currentTurnIndex >= turnOrder.size()) {
        calculateTurnOrder();
        currentTurnIndex = 0;
    }
    
    // Регенерируем стамину для текущего персонажа
    regenerateStaminaForTurn();
}

void BattleSystem::displayEntityDetails(Entity* entity) const {
    if (!entity) return;

    cout << "\n=== ХАРАКТЕРИСТИКИ " << entity->getName() << " ===\n";
    cout << "Здоровье: " << entity->getCurrentHealthPoint() << "/" << entity->getMaxHealthPoint() << "\n";
    cout << "Урон: " << entity->getDamage() << "\n";
    cout << "Защита: " << entity->getDefense() << "\n";
    cout << "Атака: " << entity->getAttack() << "\n";
    cout << "Инициатива: " << entity->getInitiative() << "\n";
    cout << "Дальность атаки: " << entity->getAttackRange() << "\n";
    cout << "Выносливость: " << entity->getCurrentStamina() << "/" << entity->getMaxStamina() << "\n";

    AbilityType ability = entity->getAbility();
    if (ability != AbilityType::NONE) {
        // Для простоты используем базовую информацию о способности
        cout << "Способность: ";
        switch (ability) {
        case AbilityType::FLYING: cout << "Полёт"; break;
        case AbilityType::POISON: cout << "Яд"; break;
        case AbilityType::FIRE_DAMAGE: cout << "Огненный урон"; break;
        case AbilityType::ICE_DAMAGE: cout << "Ледяной урон"; break;
        case AbilityType::LIGHTNING: cout << "Молния"; break;
        case AbilityType::HEAL: cout << "Лечение"; break;
        case AbilityType::TELEPORT: cout << "Телепортация"; break;
        case AbilityType::INVISIBLE: cout << "Невидимость"; break;
        case AbilityType::LIFE_STEAL: cout << "Похищение жизни"; break;
        case AbilityType::REGENERATION: cout << "Регенерация"; break;
        case AbilityType::FEAR: cout << "Страх"; break;
        case AbilityType::BERSERK: cout << "Берсерк"; break;
        case AbilityType::CHARGE: cout << "Рывок"; break;
        case AbilityType::SHIELD_WALL: cout << "Стена щитов"; break;
        case AbilityType::BATTLE_CRY: cout << "Боевой клич"; break;
        case AbilityType::MAGIC_MISSILE: cout << "Магическая стрела"; break;
        case AbilityType::CHAIN_LIGHTNING: cout << "Цепная молния"; break;
        case AbilityType::FLAME_BURST: cout << "Огненный взрыв"; break;
        case AbilityType::BLOOD_RITUAL: cout << "Кровавый ритуал"; break;
        case AbilityType::HEALING_WAVE: cout << "Волна исцеления"; break;
        case AbilityType::COMMAND: cout << "Команда"; break;
        case AbilityType::FROST_ARMOR: cout << "Ледяная броня"; break;
        case AbilityType::STEALTH: cout << "Скрытность"; break;
        case AbilityType::SHADOW_STEP: cout << "Теневой шаг"; break;
        case AbilityType::ARCANE_MISSILE: cout << "Чародейская стрела"; break;
        default: cout << "Неизвестная способность"; break;
        }
        cout << "\n";
    }
    cout << "=====================================\n";
}

string BattleSystem::getAttackDescription(Entity* attacker, Entity* target) const {
    if (!attacker || !target) return "";

    string description = attacker->getName() + " ";

    // Описания атак в зависимости от типа способности
    AbilityType ability = attacker->getAbility();
    switch (ability) {
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

void BattleSystem::printTurnOrder() const {
    cout << "\nОчередь ходов:\n";
    for (size_t i = 0; i < turnOrder.size(); ++i) {
        string marker = (i == currentTurnIndex) ? " -> " : "    ";
        cout << marker << i + 1 << ". " << turnOrder[i]->getName()
             << " (Дальность: " << turnOrder[i]->getAttackRange() << ")\n";
    }
}