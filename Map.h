#pragma once
#include <vector>
#include <string>
#include <utility>
#include <random>

enum class NodeType
{
    START = 'I',
    BATTLE = 'B',
    EVENT = 'S',
    TREASURE = 'P',
    EXIT = 'V',
    EMPTY = '.',
    UNKNOWN = '?',
    FOREST = 'F',
    CAVE = 'C',
    DEAD_CITY = 'D',
    CASTLE = 'Z'
};

struct Position
{
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position &other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position &other) const { return !(*this == other); }
    bool operator<(const Position &other) const
    {
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }
    Position operator+(const Position &other) const { return Position(x + other.x, y + other.y); }
    int manhattanDistance(const Position &other) const { return abs(x - other.x) + abs(y - other.y); }
};

class Map
{
private:
    static const int MAP_SIZE = 15;
    std::vector<std::vector<NodeType>> grid;
    Position playerPos;
    std::vector<std::vector<char>> visited;
    std::mt19937 rng;

    // Методы генерации
    void initializeGrid();
    void generateFixedMap();
    void generateCorridorMap();
    void createCorridorBranch(Position start, int length, int branchType);
    void placeSpecialNodes();
    void connectNodes();
    void ensureConnectivity();
    bool isValidPosition(int x, int y) const;
    bool isSpecialNode(NodeType type) const;
    std::vector<Position> getNeighbors(const Position &pos) const;
    void carvePath(const Position &start, const Position &end);
    bool hasPathToExit(const Position &start, NodeType exitType) const;

public:
    Map();
    void generate();
    void resetVisited();

    // Движение игрока
    bool movePlayer(char direction); // w/a/s/d
    Position getPlayerPosition() const { return playerPos; }
    NodeType getNodeAt(const Position &pos) const;
    NodeType getCurrentNode() const { return getNodeAt(playerPos); }

    // Видимость
    bool isVisible(const Position &pos) const;
    NodeType getVisibleNodeAt(const Position &pos) const;

    // Вывод
    void printMap() const;
    std::string getMapString() const;

    // Проверка
    bool isValidMove(char direction) const;
    std::vector<char> getValidMoves() const;
};