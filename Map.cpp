#include "Map.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>

Map::Map() : grid(MAP_SIZE, std::vector<NodeType>(MAP_SIZE, NodeType::EMPTY)),
             visited(MAP_SIZE, std::vector<bool>(MAP_SIZE, false)),
             rng(std::random_device{}())
{
    playerPos = Position(0, 0);
}

void Map::generate()
{
    initializeGrid();
    generateFixedMap();
    resetVisited();
    // Помещаем игрока на старт
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::START)
            {
                playerPos = Position(x, y);
                visited[y][x] = true;
                return;
            }
        }
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

    // Размещаем случайные бои, события и сокровища на путях
    std::vector<Position> pathPositions;
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (grid[y][x] == NodeType::EMPTY)
            {
                pathPositions.emplace_back(x, y);
            }
        }
    }

    // Размещаем бои (минимум 3)
    int battlesPlaced = 0;
    while (battlesPlaced < 3 && !pathPositions.empty())
    {
        int idx = rng() % pathPositions.size();
        Position pos = pathPositions[idx];
        grid[pos.y][pos.x] = NodeType::BATTLE;
        pathPositions.erase(pathPositions.begin() + idx);
        battlesPlaced++;
    }

    // Размещаем события (минимум 2)
    int eventsPlaced = 0;
    while (eventsPlaced < 2 && !pathPositions.empty())
    {
        int idx = rng() % pathPositions.size();
        Position pos = pathPositions[idx];
        grid[pos.y][pos.x] = NodeType::EVENT;
        pathPositions.erase(pathPositions.begin() + idx);
        eventsPlaced++;
    }

    // Размещаем сокровища (минимум 2)
    int treasuresPlaced = 0;
    while (treasuresPlaced < 2 && !pathPositions.empty())
    {
        int idx = rng() % pathPositions.size();
        Position pos = pathPositions[idx];
        grid[pos.y][pos.x] = NodeType::TREASURE;
        pathPositions.erase(pathPositions.begin() + idx);
        treasuresPlaced++;
    }

    // Дополнительные случайные события
    for (const auto &pos : pathPositions)
    {
        if (rng() % 5 == 0)
        { // 20% шанс
            if (rng() % 3 == 0)
            {
                grid[pos.y][pos.x] = NodeType::BATTLE;
            }
            else if (rng() % 2 == 0)
            {
                grid[pos.y][pos.x] = NodeType::EVENT;
            }
            else
            {
                grid[pos.y][pos.x] = NodeType::TREASURE;
            }
        }
    }
}

void Map::createCorridorBranch(Position start, int length, int branchType)
{
    Position current = start;
    int direction = rng() % 4; // 0-up, 1-down, 2-left, 3-right

    for (int i = 0; i < length; ++i)
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

                // Случайно меняем направление
                if (rng() % 3 == 0)
                { // 33% шанс
                    direction = rng() % 4;
                }
            }
            else
            {
                // Меняем направление при блокировке
                direction = rng() % 4;
                i--; // повторяем шаг
            }
        }
        else
        {
            // Меняем направление при выходе за границы или занятости
            direction = rng() % 4;
            i--; // повторяем шаг
        }
    }
}

void Map::placeSpecialNodes()
{
    // Размещаем старт в случайном месте (теперь на краю карты для лучшей видимости)
    int startX, startY;
    int edge = rng() % 4; // 0 - top, 1 - right, 2 - bottom, 3 - left
    switch (edge)
    {
    case 0:
        startX = rng() % MAP_SIZE;
        startY = 0;
        break; // top
    case 1:
        startX = MAP_SIZE - 1;
        startY = rng() % MAP_SIZE;
        break; // right
    case 2:
        startX = rng() % MAP_SIZE;
        startY = MAP_SIZE - 1;
        break; // bottom
    case 3:
        startX = 0;
        startY = rng() % MAP_SIZE;
        break; // left
    }
    grid[startY][startX] = NodeType::START;

    // Размещаем выходы (ровно 2 выхода) с учетом расстояния
    for (int i = 0; i < 2; ++i)
    {
        int x, y;
        bool validPosition = false;
        int attempts = 0;
        do
        {
            // Размещаем выходы на противоположных сторонах от старта
            if (i == 0)
            {
                // Первый выход - противоположная сторона
                if (startX == 0)
                    x = MAP_SIZE - 1;
                else if (startX == MAP_SIZE - 1)
                    x = 0;
                else
                    x = (rng() % 2 == 0) ? 0 : MAP_SIZE - 1;
                y = rng() % MAP_SIZE;
            }
            else
            {
                // Второй выход - другая сторона
                if (startY == 0)
                    y = MAP_SIZE - 1;
                else if (startY == MAP_SIZE - 1)
                    y = 0;
                else
                    y = (rng() % 2 == 0) ? 0 : MAP_SIZE - 1;
                x = rng() % MAP_SIZE;
            }

            // Проверяем минимальное расстояние от старта
            Position startPos(startX, startY);
            Position exitPos(x, y);
            validPosition = (grid[y][x] == NodeType::EMPTY && startPos.manhattanDistance(exitPos) >= 8);
            attempts++;
        } while (!validPosition && attempts < 50);

        if (validPosition)
        {
            grid[y][x] = NodeType::EXIT;
        }
    }

    // Размещаем бои (минимум 2) с учетом расстояния между ними
    int battleCount = 2 + (rng() % 3); // 2-4 боя
    std::vector<Position> placedNodes = {{startX, startY}};
    for (int i = 0; i < battleCount; ++i)
    {
        int x, y;
        bool validPosition = false;
        int attempts = 0;
        do
        {
            x = rng() % MAP_SIZE;
            y = rng() % MAP_SIZE;
            Position newPos(x, y);

            // Проверяем расстояние от всех уже размещенных узлов
            validPosition = (grid[y][x] == NodeType::EMPTY);
            for (const auto &placed : placedNodes)
            {
                if (placed.manhattanDistance(newPos) < 4)
                { // минимум 4 клетки расстояния
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

    // Размещаем события (минимум 1, может быть заменено на сокровище)
    int eventCount = 1 + (rng() % 3); // 1-3 события/сокровища
    for (int i = 0; i < eventCount; ++i)
    {
        int x, y;
        bool validPosition = false;
        int attempts = 0;
        do
        {
            x = rng() % MAP_SIZE;
            y = rng() % MAP_SIZE;
            Position newPos(x, y);

            // Проверяем расстояние от всех уже размещенных узлов
            validPosition = (grid[y][x] == NodeType::EMPTY);
            for (const auto &placed : placedNodes)
            {
                if (placed.manhattanDistance(newPos) < 4)
                { // минимум 4 клетки расстояния
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

    // Создаем пути только между ближайшими узлами, чтобы избежать плотной сетки
    // Сначала соединяем старт с ближайшими узлами
    Position start;
    for (const auto &pos : specialNodes)
    {
        if (grid[pos.y][pos.x] == NodeType::START)
        {
            start = pos;
            break;
        }
    }

    // Находим ближайшие узлы к старту и соединяем их
    std::vector<std::pair<int, Position>> distances;
    for (const auto &pos : specialNodes)
    {
        if (pos != start)
        {
            int dist = start.manhattanDistance(pos);
            distances.emplace_back(dist, pos);
        }
    }

    // Сортируем по расстоянию и соединяем с 2-3 ближайшими
    std::sort(distances.begin(), distances.end());
    int connections = std::min(3, (int)distances.size());
    for (int i = 0; i < connections; ++i)
    {
        carvePath(start, distances[i].second);
    }

    // Теперь соединяем остальные узлы между собой, но только если они достаточно близки
    for (size_t i = 0; i < specialNodes.size(); ++i)
    {
        for (size_t j = i + 1; j < specialNodes.size(); ++j)
        {
            Position a = specialNodes[i];
            Position b = specialNodes[j];
            int distance = a.manhattanDistance(b);

            // Соединяем только если расстояние не слишком большое и не слишком маленькое
            if (distance >= 6 && distance <= 12 && (rng() % 3 == 0))
            { // 33% шанс соединения
                carvePath(a, b);
            }
        }
    }
}

void Map::carvePath(const Position &start, const Position &end)
{
    Position current = start;
    while (current != end)
    {
        // Выбираем направление к цели
        Position direction;
        if (current.x < end.x)
            direction.x = 1;
        else if (current.x > end.x)
            direction.x = -1;
        if (current.y < end.y)
            direction.y = 1;
        else if (current.y > end.y)
            direction.y = -1;

        // Случайно выбираем приоритет направления
        if (rng() % 2 == 0 && direction.x != 0)
        {
            current.x += direction.x;
        }
        else if (direction.y != 0)
        {
            current.y += direction.y;
        }
        else if (direction.x != 0)
        {
            current.x += direction.x;
        }

        if (isValidPosition(current.x, current.y) && grid[current.y][current.x] != NodeType::UNKNOWN)
        {
            grid[current.y][current.x] = NodeType::EMPTY; // Устанавливаем как пустую клетку пути
        }
    }
}

void Map::ensureConnectivity()
{
    // Проверяем, что все специальные узлы связаны
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

    // Проверяем связность от старта
    Position start;
    for (const auto &pos : specialNodes)
    {
        if (grid[pos.y][pos.x] == NodeType::START)
        {
            start = pos;
            break;
        }
    }

    // Убеждаемся, что каждый выход имеет ровно один путь от старта
    std::vector<Position> exits;
    for (const auto &pos : specialNodes)
    {
        if (grid[pos.y][pos.x] == NodeType::EXIT)
        {
            exits.push_back(pos);
        }
    }

    for (const auto &exit : exits)
    {
        if (!hasPathToExit(start, NodeType::EXIT))
        {
            // Если нет пути, создаем его
            carvePath(start, exit);
        }
    }

    // Проверяем, что нет изолированных участков
    std::vector<std::vector<bool>> reachable(MAP_SIZE, std::vector<bool>(MAP_SIZE, false));
    std::queue<Position> q;
    q.push(start);
    reachable[start.y][start.x] = true;

    while (!q.empty())
    {
        Position current = q.front();
        q.pop();

        for (const auto &neighbor : getNeighbors(current))
        {
            if (!reachable[neighbor.y][neighbor.x] && grid[neighbor.y][neighbor.x] != NodeType::UNKNOWN)
            {
                reachable[neighbor.y][neighbor.x] = true;
                q.push(neighbor);
            }
        }
    }

    // Подключаем недостижимые области
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            if (!reachable[y][x] && grid[y][x] != NodeType::UNKNOWN)
            {
                // Находим ближайшую достижимую клетку и соединяем
                Position current(x, y);
                Position closest = start;
                int minDist = INT_MAX;
                for (int yy = 0; yy < MAP_SIZE; ++yy)
                {
                    for (int xx = 0; xx < MAP_SIZE; ++xx)
                    {
                        if (reachable[yy][xx])
                        {
                            int dist = current.manhattanDistance(Position(xx, yy));
                            if (dist < minDist)
                            {
                                minDist = dist;
                                closest = Position(xx, yy);
                            }
                        }
                    }
                }
                carvePath(current, closest);
            }
        }
    }
}

bool Map::hasPathToExit(const Position &start, NodeType exitType) const
{
    std::vector<std::vector<bool>> visited(MAP_SIZE, std::vector<bool>(MAP_SIZE, false));
    std::queue<Position> q;
    q.push(start);
    visited[start.y][start.x] = true;

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
                visited[neighbor.y][neighbor.x] = true;
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
        for (size_t i = 0; i < neighbors.size(); ++i)
        {
            size_t j = (rand() % (neighbors.size() - i)) + i;
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
        std::fill(row.begin(), row.end(), false);
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
        visited[newPos.y][newPos.x] = true;
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
            switch (grid[y][x])
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
            case NodeType::EMPTY:
                symbol = '.';
                break;
            case NodeType::UNKNOWN:
                symbol = ' ';
                break;
            }

            Position pos(x, y);
            if (pos == playerPos)
            {
                std::cout << "@";
            }
            else
            {
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
            switch (grid[y][x])
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
            case NodeType::EMPTY:
                symbol = '.';
                break;
            case NodeType::UNKNOWN:
                symbol = ' ';
                break;
            }

            Position pos(x, y);
            if (pos == playerPos)
            {
                result += "@";
            }
            else
            {
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