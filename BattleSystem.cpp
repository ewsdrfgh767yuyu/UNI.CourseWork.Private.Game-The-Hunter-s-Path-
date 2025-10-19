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

    // Проверяем, является ли цель доступной для атаки
    int distance = abs(attackerPos - targetPos);

    // Для range=0: только позиция напротив (0-0, 1-1, 2-2, 3-3)
    if (attackRange == 0) {
        return distance == 0;
    }

    // Для range=1: позиция напротив и соседние
    if (attackRange == 1) {
        return distance <= 1;
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

    cout << attacker->getName() << " атакует " << target->getName()
         << " и наносит " << damage << " урона!\n";

    // Проверка на смерть
    removeDeadEntities();

    // Пересчет очереди ходов после смерти
    if (target->getCurrentHealthPoint() <= 0) {
        calculateTurnOrder();
    }

    return true;
}

bool BattleSystem::movePosition(Entity* entity, int newPosition) {
    if (!battleActive || !entity || newPosition < 0 || newPosition > 3) return false;
    if (entity->getCurrentStamina() <= 0) return false;

    // Проверка, заблокирована ли новая позиция
    bool isPlayerEntity = false;
    for (const auto& pos : playerPositions) {
        if (pos.entity == entity) {
            isPlayerEntity = true;
            break;
        }
    }

    const vector<BattlePosition>& sameSidePositions = isPlayerEntity ? playerPositions : enemyPositions;
    if (isPositionBlocked(newPosition, sameSidePositions)) {
        cout << "Позиция заблокирована трупом!\n";
        return false;
    }

    // Поиск и обновление позиции
    vector<BattlePosition>& positions = isPlayerEntity ? playerPositions : enemyPositions;
    for (auto& pos : positions) {
        if (pos.entity == entity) {
            pos.position = newPosition;
            entity->spendStamina();
            cout << entity->getName() << " перемещается на позицию " << newPosition << "\n";
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

    // Первая линия игроков
    cout << "Игроки (передняя): ";
    for (int i = 0; i < 2; ++i) {
        bool found = false;
        for (const auto& pos : playerPositions) {
            if (pos.position == i) {
                if (pos.isCorpse) {
                    cout << "[ТРУП] ";
                } else {
                    cout << "[" << pos.entity->getName() << "] ";
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "[ПУСТО] ";
    }
    cout << "\n";

    // Вторая линия игроков
    cout << "Игроки (задняя):  ";
    for (int i = 2; i < 4; ++i) {
        bool found = false;
        for (const auto& pos : playerPositions) {
            if (pos.position == i) {
                if (pos.isCorpse) {
                    cout << "[ТРУП] ";
                } else {
                    cout << "[" << pos.entity->getName() << "] ";
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "[ПУСТО] ";
    }
    cout << "\n";

    // Разделитель
    cout << "-----------------\n";

    // Вторая линия врагов
    cout << "Враги (задняя):   ";
    for (int i = 2; i < 4; ++i) {
        bool found = false;
        for (const auto& pos : enemyPositions) {
            if (pos.position == i) {
                if (pos.isCorpse) {
                    cout << "[ТРУП] ";
                } else {
                    cout << "[" << pos.entity->getName() << "] ";
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "[ПУСТО] ";
    }
    cout << "\n";

    // Первая линия врагов
    cout << "Враги (передняя): ";
    for (int i = 0; i < 2; ++i) {
        bool found = false;
        for (const auto& pos : enemyPositions) {
            if (pos.position == i) {
                if (pos.isCorpse) {
                    cout << "[ТРУП] ";
                } else {
                    cout << "[" << pos.entity->getName() << "] ";
                }
                found = true;
                break;
            }
        }
        if (!found) cout << "[ПУСТО] ";
    }
    cout << "\n=================\n";
}

void BattleSystem::printTurnOrder() const {
    cout << "\nОчередь ходов:\n";
    for (size_t i = 0; i < turnOrder.size(); ++i) {
        string marker = (i == currentTurnIndex) ? " -> " : "    ";
        cout << marker << i + 1 << ". " << turnOrder[i]->getName() << "\n";
    }
}