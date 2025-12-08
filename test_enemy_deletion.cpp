#include "EnemyFactory.cpp"
#include "entity.h"
#include <vector>
#include <iostream>
#include <cassert>

void testEnemyDeletion()
{
    std::vector<Enemy *> enemies;

    // Создаем несколько врагов
    for (int i = 0; i < 5; ++i)
    {
        Enemy *enemy = EnemyFactory::createRandomEnemy(LocationType::FOREST, 0);
        enemies.push_back(enemy);
    }

    std::cout << "Created " << enemies.size() << " enemies." << std::endl;

    // Удаляем врагов
    for (auto enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();

    std::cout << "All enemies deleted. Remaining: " << enemies.size() << std::endl;
    assert(enemies.empty());
}

int main()
{
    testEnemyDeletion();
    std::cout << "Test passed!" << std::endl;
    return 0;
}