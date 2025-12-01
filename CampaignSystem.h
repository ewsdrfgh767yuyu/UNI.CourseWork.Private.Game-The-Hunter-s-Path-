#pragma once
#include "entity.h"
#include "BattleSystem.h"
#include "EnemyTemplates.h"
#include "HeroTemplates.h"
#include "Map.h"
#include "utils.h"
#include <vector>
#include <string>
#include <map>
#include <random>
#include <iostream>
#include <limits>

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
struct CampaignEvent
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
    std::vector<CampaignEvent> events;             // События в локации
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
    std::vector<Item> partyInventory;           // Общий инвентарь отряда

    // Вспомогательные методы
    void initializeLocations();
    CampaignEvent generateRandomEvent();
    void handleBattleEvent(const CampaignEvent &event);
    void handleTreasureEvent(const CampaignEvent &event);
    void handleTextEvent(const CampaignEvent &event);
    void handleExitEvent(const CampaignEvent &event);
    void handleBossBattleEvent(const CampaignEvent &event);
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
    void initializeCampaign();
    void createPlayerParty();
    void createPlayerPartyFromPreset(int presetIndex);
    void runCampaignLoop();
    bool isGameCompleted() const { return gameCompleted; }

    // Методы для получения информации
    const std::vector<Player *> &getPlayerParty() const { return playerParty; }
    const std::vector<Item> &getPartyInventory() const { return partyInventory; }
    const Location &getCurrentLocation() const { return currentLocation; }
    int getCurrentDifficulty() const { return currentDifficulty; }
    const Map &getGameMap() const { return gameMap; }
    Map &getGameMapMutable() { return gameMap; }
    const std::map<Position, bool> &getVisitedNodes() const { return visitedNodes; }
    void markNodeVisited(const Position &pos) { visitedNodes[pos] = true; }
    void handleNodeEventPublic(NodeType nodeType) { handleNodeEvent(nodeType); }
};