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

// Event types in campaign mode
enum class EventType
{
    BATTLE,     // Battle
    TREASURE,   // Find
    TEXT_EVENT, // Text event with choice
    EXIT,       // Exit from location
    BOSS_BATTLE // Boss battle
};

// Structure for storing event information
struct CampaignEvent
{
    EventType type;
    std::string description;
    std::vector<std::string> choices; // Choice options for text events
    std::vector<EventType> outcomes;  // Choice outcomes
    Item *reward = nullptr;           // Reward for find
    int difficultyModifier = 0;       // Difficulty modifier for battle
};

// Structure for storing location information
struct Location
{
    LocationType type;
    std::string name;
    std::string description;
    std::vector<LocationType> connections; // Connected locations
    std::vector<CampaignEvent> events;     // Events in location
    bool isFinalBossLocation = false;      // Final boss flag
};

// Class for managing campaign mode
class CampaignSystem
{
private:
    std::vector<Player *> playerParty;          // Player party
    Location currentLocation;                   // Current location
    std::map<LocationType, Location> locations; // All locations
    Map gameMap;                                // Game map
    int currentDifficulty = 0;                  // Current difficulty
    bool gameCompleted = false;                 // Game completion flag
    std::map<Position, bool> visitedNodes;      // Visited nodes on map
    std::vector<Item> partyInventory;           // Shared party inventory
    Item *pendingTreasure = nullptr;            // Pending treasure for GUI
    CampaignEvent pendingEvent;                 // Pending event for GUI
    CampaignEvent pendingExit;                  // Pending exit for GUI
    bool pendingBattle = false;                 // Pending battle for GUI
    int pendingExperience = 0;                  // Pending experience for GUI
    BattleSystem *currentBattle = nullptr;      // Current battle system for GUI

    // Helper methods
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

    // Main methods
    void startCampaign();
    void initializeCampaign();
    void createPlayerParty();
    void createPlayerPartyFromPreset(int presetIndex);
    void runCampaignLoop();
    bool isGameCompleted() const { return gameCompleted; }
    void setGameCompleted(bool completed) { gameCompleted = completed; }

    // Methods for getting information
    const std::vector<Player *> &getPlayerParty() const { return playerParty; }
    const std::vector<Item> &getPartyInventory() const { return partyInventory; }
    std::vector<Item> &getPartyInventoryMutable() { return partyInventory; }
    const Location &getCurrentLocation() const { return currentLocation; }
    const Location &getLocation(LocationType type) const { return locations.at(type); }
    int getCurrentDifficulty() const { return currentDifficulty; }
    const Map &getGameMap() const { return gameMap; }
    Map &getGameMapMutable() { return gameMap; }
    const std::map<Position, bool> &getVisitedNodes() const { return visitedNodes; }
    void markNodeVisited(const Position &pos) { visitedNodes[pos] = true; }
    void handleNodeEventPublic(NodeType nodeType) { handleNodeEvent(nodeType); }

    // Methods for GUI events
    bool hasPendingTreasure() const { return pendingTreasure != nullptr; }
    bool hasPendingEvent() const { return !pendingEvent.description.empty(); }
    bool hasPendingExit() const { return !pendingExit.description.empty(); }
    bool hasPendingBattle() const { return pendingBattle; }
    Item *getPendingTreasure() { return pendingTreasure; }
    const CampaignEvent &getPendingEvent() const { return pendingEvent; }
    const CampaignEvent &getPendingExit() const { return pendingExit; }
    void clearPendingTreasure()
    {
        delete pendingTreasure;
        pendingTreasure = nullptr;
    }
    void takePendingTreasure()
    {
        if (pendingTreasure)
        {
            partyInventory.push_back(*pendingTreasure);
        }
        clearPendingTreasure();
    }
    void clearPendingEvent() { pendingEvent = CampaignEvent(); }
    void clearPendingExit() { pendingExit = CampaignEvent(); }
    void clearPendingBattle()
    {
        pendingBattle = false;
        if (currentBattle)
        {
            delete currentBattle;
            currentBattle = nullptr;
        }
    }
    void handleEventChoice(int choiceIndex);
    void handleExitChoice(int choiceIndex);
    BattleSystem *getCurrentBattle() { return currentBattle; }
    int getPendingExperience() const { return pendingExperience; }
    void setPendingExperience(int exp) { pendingExperience = exp; }
    void clearPendingExperience() { pendingExperience = 0; }
};