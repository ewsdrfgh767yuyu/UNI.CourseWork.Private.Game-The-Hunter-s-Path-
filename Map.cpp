#include "Map.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>
#include <array>
#include <map>

NodeType charToNodeType(char c)
{
    switch (c)
    {
    case 'I':
        return NodeType::START;
    case 'B':
        return NodeType::BATTLE;
    case 'S':
        return NodeType::EVENT;
    case 'P':
        return NodeType::TREASURE;
    case 'V':
        return NodeType::EXIT;
    case '.':
        return NodeType::EMPTY;
    case '?':
        return NodeType::UNKNOWN;
    case 'F':
        return NodeType::FOREST;
    case 'C':
        return NodeType::CAVE;
    case 'D':
        return NodeType::DEAD_CITY;
    case 'Z':
        return NodeType::CASTLE;
    default:
        return NodeType::UNKNOWN;
    }
}

#include "mapPrototypesData.h"

void Map::parsePrototype(const char *const *prototype)
{
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            char c = prototype[y][x];
            if (c == 'R')
            {
                // Randomly assign BATTLE, TREASURE, or EVENT
                int randType = rng() % 3;
                switch (randType)
                {
                case 0:
                    grid[y][x] = NodeType::BATTLE;
                    break;
                case 1:
                    grid[y][x] = NodeType::TREASURE;
                    break;
                case 2:
                    grid[y][x] = NodeType::EVENT;
                    break;
                }
            }
            else
            {
                grid[y][x] = charToNodeType(c);
            }
        }
    }
}

Map::Map() : grid(MAP_SIZE, std::vector<NodeType>(MAP_SIZE, NodeType::EMPTY)),
             visited(MAP_SIZE, std::vector<char>(MAP_SIZE, 0)),
             rng(std::random_device{}())
{
    playerPos = Position(0, 0);
}

void Map::generate()
{
    // Clear grid first
    for (auto &row : grid)
    {
        std::fill(row.begin(), row.end(), NodeType::UNKNOWN);
    }

    // Select a map type currently supported: FOREST, DEAD_CITY, CAVE, CASTLE
    int mapTypeIndex = rng() % 4;

    const char *const(*selectedPrototypes)[15] = nullptr;
    switch (mapTypeIndex)
    {
    case 0:
        selectedPrototypes = forestPrototypes;
        break;
    case 1:
        selectedPrototypes = deadCityPrototypes;
        break;
    case 2:
        selectedPrototypes = cavePrototypes;
        break;
    case 3:
        selectedPrototypes = castlePrototypes;
        break;
    }

    // Select a prototype variant randomly [0..3]
    int variantIndex = rng() % 4;

    // Parse the selected prototype into grid with randomization for 'R'
    parsePrototype((const char **)selectedPrototypes[variantIndex]);

    // Add points of interest based on map type
    std::vector<Position> emptyPositions;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::EMPTY)
            {
                emptyPositions.emplace_back(x, y);
            }
        }
    }

    // Determine POI type based on map type
    NodeType poiType;
    switch (mapTypeIndex)
    {
    case 0: // Forest
        poiType = NodeType::FOREST;
        break;
    case 1: // Dead City
        poiType = NodeType::DEAD_CITY;
        break;
    case 2: // Cave
        poiType = NodeType::CAVE;
        break;
    case 3: // Castle
        poiType = NodeType::CASTLE;
        break;
    }

    // Add 2-4 POI in random empty positions
    int numPOI = 2 + (rng() % 3); // 2-4 POI
    for (int i = 0; i < numPOI && !emptyPositions.empty(); ++i)
    {
        int idx = rng() % emptyPositions.size();
        Position pos = emptyPositions[idx];
        grid[pos.y][pos.x] = poiType;
        emptyPositions.erase(emptyPositions.begin() + idx);
    }

    resetVisited();

    // Place player at start position
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::START)
            {
                playerPos = Position(x, y);
                visited[y][x] = 1;
                return;
            }
        }
    }
}

void Map::generateCorridorMap()
{
    // Размещаем специальные узлы (старт, выходы, бои, события, сокровища)
    placeSpecialNodes();

    // Соединяем узлы коридорами
    connectNodes();

    // Убеждаемся в связности карты
    ensureConnectivity();

    // Добавляем дополнительные ветви коридоров для большей сложности (опционально)
    // Создаем несколько случайных ветвей от существующих коридоров
    std::vector<Position> corridorStarts;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::EMPTY)
            {
                corridorStarts.emplace_back(x, y);
            }
        }
    }

    // Выбираем случайные стартовые точки для ветвей (до 3 ветвей)
    int branchesToCreate = std::min(3, (int)corridorStarts.size());
    for (int i = 0; i < branchesToCreate; ++i)
    {
        int idx = rng() % corridorStarts.size();
        Position start = corridorStarts[idx];
        corridorStarts.erase(corridorStarts.begin() + idx);

        // Создаем ветвь длиной 3-6 клеток
        int length = 3 + (rng() % 4);
        createCorridorBranch(start, length, 0); // branchType = 0 для простоты
    }
}

void Map::initializeGrid()
{
    for (auto &row : grid)
    {
        std::fill(row.begin(), row.end(), NodeType::UNKNOWN);
    }
}

void Map::generateFixedMap()
{
    // Создаем фиксированную карту с локациями
    // Лес (F) - 2 выхода
    // Пещера (C) - 2 выхода
    // Город Мертвецов (D) - 2 выхода
    // Замок (Z) - 1 выход + босс

    // Очищаем карту
    for (auto &row : grid)
    {
        std::fill(row.begin(), row.end(), NodeType::UNKNOWN);
    }

    // Размещаем локации
    // Лес в левом верхнем углу
    grid[2][2] = NodeType::FOREST;
    grid[2][3] = NodeType::EMPTY;
    grid[2][4] = NodeType::EMPTY;
    grid[3][2] = NodeType::EMPTY;
    grid[3][3] = NodeType::EMPTY;
    grid[3][4] = NodeType::EMPTY;
    grid[4][2] = NodeType::EMPTY;
    grid[4][3] = NodeType::EMPTY;
    grid[4][4] = NodeType::EMPTY;

    // Пещера в правом верхнем углу
    grid[2][10] = NodeType::CAVE;
    grid[2][11] = NodeType::EMPTY;
    grid[2][12] = NodeType::EMPTY;
    grid[3][10] = NodeType::EMPTY;
    grid[3][11] = NodeType::EMPTY;
    grid[3][12] = NodeType::EMPTY;
    grid[4][10] = NodeType::EMPTY;
    grid[4][11] = NodeType::EMPTY;
    grid[4][12] = NodeType::EMPTY;

    // Город Мертвецов в левом нижнем углу
    grid[10][2] = NodeType::DEAD_CITY;
    grid[10][3] = NodeType::EMPTY;
    grid[10][4] = NodeType::EMPTY;
    grid[11][2] = NodeType::EMPTY;
    grid[11][3] = NodeType::EMPTY;
    grid[11][4] = NodeType::EMPTY;
    grid[12][2] = NodeType::EMPTY;
    grid[12][3] = NodeType::EMPTY;
    grid[12][4] = NodeType::EMPTY;

    // Замок в центре
    grid[7][7] = NodeType::CASTLE;
    grid[7][8] = NodeType::BATTLE; // Бой с боссом в замке

    // Создаем пути между локациями
    // Путь от Леса к Пещере
    carvePath(Position(4, 3), Position(10, 3));

    // Путь от Леса к Городу Мертвецов
    carvePath(Position(3, 4), Position(3, 10));

    // Путь от Пещеры к Замку
    carvePath(Position(11, 4), Position(7, 7));

    // Путь от Города Мертвецов к Замку
    carvePath(Position(4, 11), Position(7, 7));

    // Размещаем старт в Лесу
    grid[3][3] = NodeType::START;

    // Размещаем обязательные бои, заграждающие пути к событиям и выходам
    // Бой перед событием в лесу
    grid[3][5] = NodeType::BATTLE;
    // Бой перед сокровищем в лесу
    grid[4][5] = NodeType::BATTLE;
    // Бой перед выходом из леса
    grid[5][4] = NodeType::BATTLE;

    // Бой перед событием в пещерах
    grid[3][13] = NodeType::BATTLE;
    // Бой перед сокровищем в пещерах
    grid[4][13] = NodeType::BATTLE;
    // Бой перед выходом из пещер
    grid[5][12] = NodeType::BATTLE;

    // Бой перед событием в городе мертвых
    grid[11][5] = NodeType::BATTLE;
    // Бой перед сокровищем в городе мертвых
    grid[12][5] = NodeType::BATTLE;
    // Бой перед выходом из города мертвых
    grid[11][6] = NodeType::BATTLE;

    // Бой перед финальным выходом
    grid[8][7] = NodeType::BATTLE;
    grid[6][7] = NodeType::BATTLE;

    // Размещаем события и сокровища
    // Событие в лесу
    grid[3][6] = NodeType::EVENT;
    // Сокровище в лесу
    grid[4][6] = NodeType::TREASURE;

    // Событие в пещерах
    grid[3][14] = NodeType::EVENT;
    // Сокровище в пещерах
    grid[4][14] = NodeType::TREASURE;

    // Событие в городе мертвых
    grid[11][8] = NodeType::EVENT;
    // Сокровище в городе мертвых
    grid[12][7] = NodeType::TREASURE;

    // Финальное событие перед замком
    grid[7][6] = NodeType::EVENT;
    // Финальное сокровище
    grid[7][8] = NodeType::TREASURE;

    // Размещаем выходы из локаций
    // Выходы из леса (2 выхода)
    grid[6][4] = NodeType::EXIT;
    grid[4][6] = NodeType::EXIT;

    // Выходы из пещер (2 выхода)
    grid[6][12] = NodeType::EXIT;
    grid[5][13] = NodeType::EXIT;

    // Выходы из города мертвых (2 выхода)
    grid[13][4] = NodeType::EXIT;
    grid[11][7] = NodeType::EXIT;

    // Выход из замка убран, так как замок - финальная локация

    resetVisited();

    // Place player at start position
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::START)
            {
                playerPos = Position(x, y);
                visited[y][x] = 1;
                return;
            }
        }
    }
}

void Map::createCorridorBranch(Position start, int length, int branchType)
{
    Position current = start;
    int direction = rng() % 4; // 0-up, 1-down, 2-left, 3-right

    for (int i = 0; i < length;)
    {
        int attempts = 0;
        bool placed = false;

        while (attempts < 10 && !placed) // Максимум 10 попыток на шаг
        {
            // Пытаемся двигаться в текущем направлении
            Position next = current;
            switch (direction)
            {
            case 0:
                next.y--;
                break; // up
            case 1:
                next.y++;
                break; // down
            case 2:
                next.x--;
                break; // left
            case 3:
                next.x++;
                break; // right
            }

            if (isValidPosition(next.x, next.y) && grid[next.y][next.x] == NodeType::UNKNOWN)
            {
                // Проверяем, что не слишком близко к другим коридорам
                bool canPlace = true;
                for (int dy = -1; dy <= 1; ++dy)
                {
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        if (dx == 0 && dy == 0)
                            continue;
                        Position check(next.x + dx, next.y + dy);
                        if (isValidPosition(check.x, check.y) && grid[check.y][check.x] == NodeType::EMPTY)
                        {
                            canPlace = false;
                            break;
                        }
                    }
                    if (!canPlace)
                        break;
                }

                if (canPlace)
                {
                    grid[next.y][next.x] = NodeType::EMPTY;
                    current = next;
                    placed = true;

                    // Случайно меняем направление
                    if (rng() % 3 == 0)
                    { // 33% шанс
                        direction = rng() % 4;
                    }
                }
            }

            if (!placed)
            {
                // Меняем направление при неудаче
                direction = rng() % 4;
                attempts++;
            }
        }

        if (placed)
        {
            i++; // Увеличиваем только если удалось разместить
        }
        else
        {
            // Если не удалось разместить после 10 попыток, прекращаем ветвь
            break;
        }
    }
}

void Map::placeSpecialNodes()
{
    // Размещаем старт в случайном месте на краю карты
    int startX, startY;
    int edge = rng() % 4; // 0 - top, 1 - right, 2 - bottom, 3 - left
    switch (edge)
    {
    case 0:
        startX = 1 + rng() % (MAP_SIZE - 2); // Избегаем углов
        startY = 0;
        break; // top
    case 1:
        startX = MAP_SIZE - 1;
        startY = 1 + rng() % (MAP_SIZE - 2); // Избегаем углов
        break;                               // right
    case 2:
        startX = 1 + rng() % (MAP_SIZE - 2); // Избегаем углов
        startY = MAP_SIZE - 1;
        break; // bottom
    case 3:
        startX = 0;
        startY = 1 + rng() % (MAP_SIZE - 2); // Избегаем углов
        break;                               // left
    }
    grid[startY][startX] = NodeType::START;

    // Список уже размещенных узлов
    std::vector<Position> placedNodes = {{startX, startY}};

    // Размещаем выходы (ровно 2 выхода) на противоположных сторонах от старта
    for (int i = 0; i < 2; ++i)
    {
        int x, y;
        bool validPosition = false;
        int attempts = 0;

        do
        {
            // Первый выход на противоположной стороне по X
            if (i == 0)
            {
                if (startX < MAP_SIZE / 2)
                {
                    x = MAP_SIZE - 1 - (rng() % 3); // Правая сторона, не в углу
                }
                else
                {
                    x = rng() % 3; // Левая сторона, не в углу
                }
                y = 1 + rng() % (MAP_SIZE - 2); // Центральная позиция по Y
            }
            // Второй выход на противоположной стороне по Y
            else
            {
                if (startY < MAP_SIZE / 2)
                {
                    y = MAP_SIZE - 1 - (rng() % 3); // Нижняя сторона, не в углу
                }
                else
                {
                    y = rng() % 3; // Верхняя сторона, не в углу
                }
                x = 1 + rng() % (MAP_SIZE - 2); // Центральная позиция по X
            }

            // Проверяем минимальное расстояние от старта и других узлов
            Position newPos(x, y);
            validPosition = (grid[y][x] == NodeType::UNKNOWN);

            // Проверяем расстояние от всех уже размещенных узлов
            for (const auto &placed : placedNodes)
            {
                if (placed.manhattanDistance(newPos) < 6) // Увеличили минимальное расстояние
                {
                    validPosition = false;
                    break;
                }
            }

            // Дополнительная проверка: выход должен быть достаточно далеко от старта
            if (validPosition)
            {
                validPosition = (Position(startX, startY).manhattanDistance(newPos) >= 10);
            }

            attempts++;
        } while (!validPosition && attempts < 100);

        if (validPosition)
        {
            grid[y][x] = NodeType::EXIT;
            placedNodes.emplace_back(x, y);
        }
    }

    // Размещаем бои (4-6 боев) с равномерным распределением
    int battleCount = 4 + (rng() % 3); // 4-6 боев
    for (int i = 0; i < battleCount; ++i)
    {
        int x, y;
        bool validPosition = false;
        int attempts = 0;

        do
        {
            // Размещаем бои в разных квадрантах карты для лучшего распределения
            int quadrant = rng() % 4;
            switch (quadrant)
            {
            case 0: // Левый верхний
                x = 2 + rng() % (MAP_SIZE / 2 - 2);
                y = 2 + rng() % (MAP_SIZE / 2 - 2);
                break;
            case 1: // Правый верхний
                x = MAP_SIZE / 2 + rng() % (MAP_SIZE / 2 - 2);
                y = 2 + rng() % (MAP_SIZE / 2 - 2);
                break;
            case 2: // Левый нижний
                x = 2 + rng() % (MAP_SIZE / 2 - 2);
                y = MAP_SIZE / 2 + rng() % (MAP_SIZE / 2 - 2);
                break;
            case 3: // Правый нижний
                x = MAP_SIZE / 2 + rng() % (MAP_SIZE / 2 - 2);
                y = MAP_SIZE / 2 + rng() % (MAP_SIZE / 2 - 2);
                break;
            }

            Position newPos(x, y);
            validPosition = (grid[y][x] == NodeType::UNKNOWN);

            // Проверяем расстояние от всех уже размещенных узлов
            for (const auto &placed : placedNodes)
            {
                if (placed.manhattanDistance(newPos) < 4)
                {
                    validPosition = false;
                    break;
                }
            }

            attempts++;
        } while (!validPosition && attempts < 100);

        if (validPosition)
        {
            grid[y][x] = NodeType::BATTLE;
            placedNodes.emplace_back(x, y);
        }
    }

    // Размещаем события и сокровища (4-6 штук) в оставшихся квадрантах
    int eventCount = 4 + (rng() % 3); // 4-6 событий/сокровищ
    for (int i = 0; i < eventCount; ++i)
    {
        int x, y;
        bool validPosition = false;
        int attempts = 0;

        do
        {
            // Размещаем в случайном месте, но избегаем краев
            x = 2 + rng() % (MAP_SIZE - 4);
            y = 2 + rng() % (MAP_SIZE - 4);

            Position newPos(x, y);
            validPosition = (grid[y][x] == NodeType::UNKNOWN);

            // Проверяем расстояние от всех уже размещенных узлов
            for (const auto &placed : placedNodes)
            {
                if (placed.manhattanDistance(newPos) < 3)
                {
                    validPosition = false;
                    break;
                }
            }

            attempts++;
        } while (!validPosition && attempts < 100);

        if (validPosition)
        {
            // 50% шанс на событие, 50% на сокровище
            grid[y][x] = (rng() % 2 == 0) ? NodeType::EVENT : NodeType::TREASURE;
            placedNodes.emplace_back(x, y);
        }
    }
}

void Map::connectNodes()
{
    std::vector<Position> specialNodes;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (isSpecialNode(grid[y][x]))
            {
                specialNodes.emplace_back(x, y);
            }
        }
    }

    if (specialNodes.size() < 2)
        return; // Нужно как минимум 2 узла для соединения

    // Находим стартовую позицию
    Position start;
    for (const auto &pos : specialNodes)
    {
        if (grid[pos.y][pos.x] == NodeType::START)
        {
            start = pos;
            break;
        }
    }

    // Создаем минимальное остовное дерево для гарантии связности
    // Используем простой алгоритм: соединяем каждый узел с ближайшим уже подключенным узлом

    std::vector<Position> connectedNodes;
    connectedNodes.push_back(start);

    // Удаляем старт из списка специальных узлов для соединения
    auto it = std::find(specialNodes.begin(), specialNodes.end(), start);
    if (it != specialNodes.end())
    {
        specialNodes.erase(it);
    }

    // Соединяем все оставшиеся узлы с уже подключенной сетью
    while (!specialNodes.empty())
    {
        Position closestNode;
        Position closestConnected;
        int minDistance = MAP_SIZE * MAP_SIZE;

        // Находим ближайшую пару (неподключенный узел - подключенный узел)
        for (const auto &node : specialNodes)
        {
            for (const auto &connected : connectedNodes)
            {
                int distance = node.manhattanDistance(connected);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closestNode = node;
                    closestConnected = connected;
                }
            }
        }

        // Соединяем найденные узлы
        if (minDistance < MAP_SIZE * MAP_SIZE)
        {
            carvePath(closestNode, closestConnected);
            connectedNodes.push_back(closestNode);

            // Удаляем подключенный узел из списка неподключенных
            it = std::find(specialNodes.begin(), specialNodes.end(), closestNode);
            if (it != specialNodes.end())
            {
                specialNodes.erase(it);
            }
        }
        else
        {
            // Если не нашли подходящую пару, прекращаем
            break;
        }
    }

    // Добавляем несколько случайных соединений для создания альтернативных путей
    int extraConnections = std::min(3, (int)connectedNodes.size() / 2);
    for (int i = 0; i < extraConnections; ++i)
    {
        if (connectedNodes.size() >= 2)
        {
            int idx1 = rng() % connectedNodes.size();
            int idx2 = rng() % connectedNodes.size();

            // Убеждаемся, что это разные узлы
            while (idx2 == idx1 && connectedNodes.size() > 1)
            {
                idx2 = rng() % connectedNodes.size();
            }

            Position node1 = connectedNodes[idx1];
            Position node2 = connectedNodes[idx2];
            int distance = node1.manhattanDistance(node2);

            // Соединяем только если расстояние разумное (не слишком большое и не слишком маленькое)
            if (distance >= 4 && distance <= 10)
            {
                carvePath(node1, node2);
            }
        }
    }
}

void Map::carvePath(const Position &start, const Position &end)
{
    Position current = start;

    // Создаем путь от старта до конца
    while (current != end)
    {
        // Выбираем направление к цели
        Position direction;
        if (current.x < end.x)
            direction.x = 1;
        else if (current.x > end.x)
            direction.x = -1;
        else
            direction.x = 0;

        if (current.y < end.y)
            direction.y = 1;
        else if (current.y > end.y)
            direction.y = -1;
        else
            direction.y = 0;

        // Случайно выбираем приоритет направления (горизонтальное или вертикальное)
        if ((direction.x != 0 && direction.y != 0) && (rng() % 2 == 0))
        {
            // Двигаемся по горизонтали
            current.x += direction.x;
        }
        else if (direction.y != 0)
        {
            // Двигаемся по вертикали
            current.y += direction.y;
        }
        else if (direction.x != 0)
        {
            // Двигаемся по горизонтали
            current.x += direction.x;
        }

        // Устанавливаем клетку как пустую (путь)
        if (isValidPosition(current.x, current.y))
        {
            grid[current.y][current.x] = NodeType::EMPTY;
        }
        else
        {
            // Если вышли за границы, прекращаем создание пути
            break;
        }
    }

    // Убеждаемся, что конечная точка тоже установлена как EMPTY
    if (isValidPosition(end.x, end.y))
    {
        grid[end.y][end.x] = NodeType::EMPTY;
    }
}

void Map::ensureConnectivity()
{
    // Находим стартовую позицию
    Position start;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::START)
            {
                start = Position(x, y);
                break;
            }
        }
    }

    // Проверяем связность от старта ко всем специальным узлам
    std::vector<Position> specialNodes;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (isSpecialNode(grid[y][x]) && grid[y][x] != NodeType::START)
            {
                specialNodes.emplace_back(x, y);
            }
        }
    }

    // Убеждаемся, что каждый специальный узел достижим от старта
    for (const auto &node : specialNodes)
    {
        if (!hasPathToExit(start, grid[node.y][node.x]))
        {
            // Если нет пути, создаем его
            carvePath(start, node);
        }
    }

    // Проверяем общую связность карты (все EMPTY клетки должны быть связаны)
    std::vector<std::vector<char>> reachable(MAP_SIZE, std::vector<char>(MAP_SIZE, 0));
    std::queue<Position> q;
    q.push(start);
    reachable[start.y][start.x] = 1;

    while (!q.empty())
    {
        Position current = q.front();
        q.pop();

        // Проверяем всех соседей
        std::vector<Position> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        for (const auto &dir : directions)
        {
            Position neighbor(current.x + dir.x, current.y + dir.y);
            if (isValidPosition(neighbor.x, neighbor.y) &&
                !reachable[neighbor.y][neighbor.x] &&
                grid[neighbor.y][neighbor.x] != NodeType::UNKNOWN)
            {
                reachable[neighbor.y][neighbor.x] = 1;
                q.push(neighbor);
            }
        }
    }

    // Подключаем все недостижимые EMPTY клетки
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] != NodeType::UNKNOWN && !reachable[y][x])
            {
                Position isolated(x, y);

                // Находим ближайшую достижимую клетку
                Position closest;
                int minDist = MAP_SIZE * MAP_SIZE; // Большое начальное расстояние

                for (int yy = 0; yy < MAP_SIZE; ++yy)
                {
                    for (int xx = 0; xx < MAP_SIZE; ++xx)
                    {
                        if (reachable[yy][xx])
                        {
                            int dist = isolated.manhattanDistance(Position(xx, yy));
                            if (dist < minDist)
                            {
                                minDist = dist;
                                closest = Position(xx, yy);
                            }
                        }
                    }
                }

                // Соединяем изолированную клетку с ближайшей достижимой
                if (minDist < MAP_SIZE * MAP_SIZE)
                {
                    carvePath(isolated, closest);
                }
            }
        }
    }
}

bool Map::hasPathToExit(const Position &start, NodeType exitType) const
{
    std::vector<std::vector<char>> visited(MAP_SIZE, std::vector<char>(MAP_SIZE, 0));
    std::queue<Position> q;
    q.push(start);
    visited[start.y][start.x] = 1;

    while (!q.empty())
    {
        Position current = q.front();
        q.pop();

        if (grid[current.y][current.x] == exitType)
        {
            return true;
        }

        for (const auto &neighbor : getNeighbors(current))
        {
            if (!visited[neighbor.y][neighbor.x] && grid[neighbor.y][neighbor.x] != NodeType::UNKNOWN)
            {
                visited[neighbor.y][neighbor.x] = 1;
                q.push(neighbor);
            }
        }
    }
    return false;
}

bool Map::isValidPosition(int x, int y) const
{
    return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
}

bool Map::isSpecialNode(NodeType type) const
{
    return type == NodeType::START || type == NodeType::BATTLE ||
           type == NodeType::EVENT || type == NodeType::TREASURE || type == NodeType::EXIT;
}

std::vector<Position> Map::getNeighbors(const Position &pos) const
{
    std::vector<Position> neighbors;
    std::vector<Position> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // up, down, left, right
    for (const auto &dir : directions)
    {
        Position neighbor = pos + dir;
        if (isValidPosition(neighbor.x, neighbor.y))
        {
            neighbors.push_back(neighbor);
        }
    }

    // Ограничиваем до 6 соседей максимум
    if (neighbors.size() > 6)
    {
        // Простая перетасовка: меняем местами элементы
        for (int i = 0; i < neighbors.size(); ++i)
        {
            int j = (rand() % (neighbors.size() - i)) + i;
            std::swap(neighbors[i], neighbors[j]);
        }
        neighbors.resize(6);
    }

    return neighbors;
}

void Map::resetVisited()
{
    for (auto &row : visited)
    {
        std::fill(row.begin(), row.end(), 0);
    }
}

bool Map::movePlayer(char direction)
{
    Position newPos = playerPos;
    switch (direction)
    {
    case 'w':
        newPos.y--;
        break;
    case 's':
        newPos.y++;
        break;
    case 'a':
        newPos.x--;
        break;
    case 'd':
        newPos.x++;
        break;
    default:
        return false;
    }

    if (isValidPosition(newPos.x, newPos.y) && grid[newPos.y][newPos.x] != NodeType::UNKNOWN)
    {
        playerPos = newPos;
        visited[newPos.y][newPos.x] = 1;
        return true;
    }
    return false;
}

NodeType Map::getNodeAt(const Position &pos) const
{
    if (!isValidPosition(pos.x, pos.y))
        return NodeType::UNKNOWN;
    return grid[pos.y][pos.x];
}

bool Map::isVisible(const Position &pos) const
{
    return playerPos.manhattanDistance(pos) <= 5;
}

NodeType Map::getVisibleNodeAt(const Position &pos) const
{
    if (!isValidPosition(pos.x, pos.y))
        return NodeType::UNKNOWN;
    if (isVisible(pos))
    {
        return visited[pos.y][pos.x] ? grid[pos.y][pos.x] : NodeType::UNKNOWN;
    }
    return NodeType::UNKNOWN;
}

void Map::printMap() const
{
    std::cout << "Карта:\n";
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            char symbol = ' ';
            Position pos(x, y);

            if (pos == playerPos)
            {
                std::cout << "@";
            }
            else if (!isVisible(pos))
            {
                std::cout << " ";
            }
            else
            {
                NodeType nodeType = grid[y][x];
                if (nodeType == NodeType::UNKNOWN)
                {
                    symbol = ' ';
                }
                else if (nodeType == NodeType::EMPTY)
                {
                    symbol = '.';
                }
                else if (visited[y][x])
                {
                    // Исследованная клетка - показываем настоящий символ
                    switch (nodeType)
                    {
                    case NodeType::START:
                        symbol = 'I';
                        break;
                    case NodeType::BATTLE:
                        symbol = 'B';
                        break;
                    case NodeType::EVENT:
                        symbol = 'S';
                        break;
                    case NodeType::TREASURE:
                        symbol = 'P';
                        break;
                    case NodeType::EXIT:
                        symbol = 'V';
                        break;
                    case NodeType::FOREST:
                        symbol = 'F';
                        break;
                    case NodeType::CAVE:
                        symbol = 'C';
                        break;
                    case NodeType::DEAD_CITY:
                        symbol = 'D';
                        break;
                    case NodeType::CASTLE:
                        symbol = 'Z';
                        break;
                    default:
                        symbol = '.';
                        break;
                    }
                }
                else
                {
                    // Неисследованная клетка - показываем ? для событий
                    if (nodeType == NodeType::EVENT || nodeType == NodeType::TREASURE || nodeType == NodeType::BATTLE || nodeType == NodeType::EXIT)
                    {
                        symbol = '?';
                    }
                    else
                    {
                        symbol = '.';
                    }
                }
                std::cout << symbol;
            }

            // Добавляем пробел между столбцами
            if (x < MAP_SIZE - 1)
            {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

std::string Map::getMapString() const
{
    std::string result;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            char symbol = ' ';
            Position pos(x, y);

            if (pos == playerPos)
            {
                result += "@";
            }
            else if (!isVisible(pos))
            {
                result += " ";
            }
            else
            {
                NodeType nodeType = grid[y][x];
                if (nodeType == NodeType::UNKNOWN)
                {
                    symbol = ' ';
                }
                else if (nodeType == NodeType::EMPTY)
                {
                    symbol = '.';
                }
                else if (visited[y][x])
                {
                    // Исследованная клетка - показываем настоящий символ
                    switch (nodeType)
                    {
                    case NodeType::START:
                        symbol = 'I';
                        break;
                    case NodeType::BATTLE:
                        symbol = 'B';
                        break;
                    case NodeType::EVENT:
                        symbol = 'S';
                        break;
                    case NodeType::TREASURE:
                        symbol = 'P';
                        break;
                    case NodeType::EXIT:
                        symbol = 'V';
                        break;
                    case NodeType::FOREST:
                        symbol = 'F';
                        break;
                    case NodeType::CAVE:
                        symbol = 'C';
                        break;
                    case NodeType::DEAD_CITY:
                        symbol = 'D';
                        break;
                    case NodeType::CASTLE:
                        symbol = 'Z';
                        break;
                    default:
                        symbol = '.';
                        break;
                    }
                }
                else
                {
                    // Неисследованная клетка - показываем ? для событий
                    if (nodeType == NodeType::EVENT || nodeType == NodeType::TREASURE || nodeType == NodeType::BATTLE || nodeType == NodeType::EXIT)
                    {
                        symbol = '?';
                    }
                    else
                    {
                        symbol = '.';
                    }
                }
                result += symbol;
            }

            // Добавляем пробел между столбцами
            if (x < MAP_SIZE - 1)
            {
                result += " ";
            }
        }
        result += "\n";
    }
    return result;
}

bool Map::isValidMove(char direction) const
{
    Position newPos = playerPos;
    switch (direction)
    {
    case 'w':
        newPos.y--;
        break;
    case 's':
        newPos.y++;
        break;
    case 'a':
        newPos.x--;
        break;
    case 'd':
        newPos.x++;
        break;
    default:
        return false;
    }
    return isValidPosition(newPos.x, newPos.y) && grid[newPos.y][newPos.x] != NodeType::UNKNOWN;
}

bool Map::checkAndFixMapIntegrity()
{
    // Find start position
    Position start(-1, -1);
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::START)
            {
                start = Position(x, y);
                break;
            }
        }
        if (start.x != -1)
            break;
    }
    if (start.x == -1)
        return false; // No start found, cannot verify

    // Collect all special nodes except the start
    std::vector<Position> specialNodes;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            NodeType type = grid[y][x];
            if (isSpecialNode(type) && type != NodeType::START)
            {
                specialNodes.emplace_back(x, y);
            }
        }
    }

    // Check reachability of each special node, carve path if unreachable
    for (const auto &node : specialNodes)
    {
        if (!hasPathToExit(start, grid[node.y][node.x]))
        {
            carvePath(start, node);
        }
    }

    // Check connectivity of all non-UNKNOWN cells starting from start
    std::vector<std::vector<char>> reachable(MAP_SIZE, std::vector<char>(MAP_SIZE, 0));
    std::queue<Position> q;
    q.push(start);
    reachable[start.y][start.x] = 1;

    while (!q.empty())
    {
        Position current = q.front();
        q.pop();

        std::vector<Position> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        for (const auto &dir : directions)
        {
            Position neighbor(current.x + dir.x, current.y + dir.y);
            if (isValidPosition(neighbor.x, neighbor.y) &&
                !reachable[neighbor.y][neighbor.x] &&
                grid[neighbor.y][neighbor.x] != NodeType::UNKNOWN)
            {
                reachable[neighbor.y][neighbor.x] = 1;
                q.push(neighbor);
            }
        }
    }

    // For any unreachable cell, carve path to closest reachable cell
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] != NodeType::UNKNOWN && !reachable[y][x])
            {
                Position isolated(x, y);
                Position closest;
                int minDist = MAP_SIZE * MAP_SIZE;

                for (int yy = 0; yy < MAP_SIZE; ++yy)
                {
                    for (int xx = 0; xx < MAP_SIZE; ++xx)
                    {
                        if (reachable[yy][xx])
                        {
                            int dist = isolated.manhattanDistance(Position(xx, yy));
                            if (dist < minDist)
                            {
                                minDist = dist;
                                closest = Position(xx, yy);
                            }
                        }
                    }
                }

                if (minDist < MAP_SIZE * MAP_SIZE)
                {
                    carvePath(isolated, closest);
                }
            }
        }
    }

    return true;
}

std::vector<char> Map::getValidMoves() const
{
    std::vector<char> moves;
    std::string possibleMoves = "wasd";
    for (char move : possibleMoves)
    {
        if (isValidMove(move))
        {
            moves.push_back(move);
        }
    }
    return moves;
}