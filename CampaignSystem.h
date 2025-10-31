#pragma once
#include "entity.h"
#include "BattleSystem.h"
#include "EnemyTemplates.h"
#include "HeroTemplates.h"
#include "Map.h"
#include <vector>
#include <string>
#include <map>
#include <random>

// Типы событий в походовом режиме
enum class EventType
{
    BATTLE,     // Бой
    TREASURE,   // Находка
    TEXT_EVENT, // Текстовый ивент с выбором
    EXIT,       // Выход из локации
    BOSS_BATTLE // Бой с боссом
};

// Структура для хранения информации о событии
struct Event
{
    EventType type;
    std::string description;
    std::vector<std::string> choices; // Варианты выбора для текстовых ивентов
    std::vector<EventType> outcomes;  // Результаты выбора
    Item *reward = nullptr;           // Награда за находку
    int difficultyModifier = 0;       // Модификатор сложности для боя
};

// Структура для хранения информации о локации
struct Location
{
    LocationType type;
    std::string name;
    std::string description;
    std::vector<LocationType> connections; // Связанные локации
    std::vector<Event> events;             // События в локации
    bool isFinalBossLocation = false;      // Флаг финального босса
};

// Класс для управления походовым режимом
class CampaignSystem
{
private:
    std::vector<Player *> playerParty;          // Отряд игрока
    Location currentLocation;                   // Текущая локация
    std::map<LocationType, Location> locations; // Все локации
    Map gameMap;                                // Карта игры
    int currentDifficulty = 0;                  // Текущая сложность
    bool gameCompleted = false;                 // Флаг завершения игры
    std::map<Position, bool> visitedNodes;      // Посещенные узлы на карте

    // Вспомогательные методы
    void initializeLocations();
    Event generateRandomEvent();
    void handleBattleEvent(const Event &event);
    void handleTreasureEvent(const Event &event);
    void handleTextEvent(const Event &event);
    void handleExitEvent(const Event &event);
    void handleBossBattleEvent(const Event &event);
    void handleNodeEvent(NodeType nodeType);
    void displayLocationInfo();
    void displayAvailableConnections();
    bool moveToLocation(LocationType targetLocation);
    void cleanupParty();
    void runMapMode();
    char getPlayerMovementInput();
    void displayMap();
    void manageInventory(Player *player = nullptr);

public:
    CampaignSystem();
    ~CampaignSystem();

    // Основные методы
    void startCampaign();
    void createPlayerParty();
    void runCampaignLoop();
    bool isGameCompleted() const { return gameCompleted; }

    // Методы для получения информации
    const std::vector<Player *> &getPlayerParty() const { return playerParty; }
    const Location &getCurrentLocation() const { return currentLocation; }
    int getCurrentDifficulty() const { return currentDifficulty; }
    const Map &getGameMap() const { return gameMap; }
};