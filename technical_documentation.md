# Technical Documentation of the Game "The Hunter's Path"

## Project Overview

A game in the genre of Darkest Dungeon with turn-based battles, non-linear location navigation, and rogue-like elements.

## Architectural Solutions

### 1. Modular Structure
The project is divided into independent systems to ensure:
- **Testability** - each system can be tested separately
- **Extensibility** - easy addition of new mechanics
- **Maintainability** - clear responsibility separation

### 2. Entity Inheritance
Using a class hierarchy:
- **Entity** - basic functionality for all creatures
- **Player** - specialization for playable characters
- **Enemy** - specialization for enemies

### 3. Component Approach
Each system is responsible for its area:
- **BattleSystem** - battle logic
- **LocationSystem** - world navigation
- **EventSystem** - event handling
- **PartySystem** - party management

## Implementation Details

### Battle System (BattleSystem)

#### Initiative Mechanics
```cpp
// Calculate turn weight based on initiative
int turnWeight = entity->getInitiative() / 10;
// Unit with initiative 20: turnWeight = 2 (acts twice as often)
```

#### Positional System
```cpp
class BattlePosition {
    vector<Entity*> frontLine;    // Front line (positions 0-1)
    vector<Entity*> backLine;     // Back line (positions 2-3)
    vector<Entity*> corpses;      // Corpses on positions
};

// Check target availability
bool canAttackTarget(Entity* attacker, Entity* target) {
    int attackerRange = attacker->getAttackRange();
    int targetPosition = getPosition(target);

    // Check for corpses blocking the path
    return isTargetReachable(attackerRange, targetPosition);
}
```

### Location System

#### Navigation Graph
```cpp
struct LocationConnection {
    Location* from;
    Location* to;
    int travelCost;  // Travel cost (stamina/health)
};

// Non-linear connections between locations
vector<LocationConnection> connections = {
    {FOREST, CAVE, 10},
    {FOREST, DEAD_CITY, 15},
    {CAVE, CASTLE, 20},
    {CAVE, DEAD_CITY, 12},
    {DEAD_CITY, CASTLE, 18},
    {DEAD_CITY, FOREST, 15},
    {CASTLE, CAVE, 20},
    {CASTLE, FOREST, 25}
};
```

### Event System

#### Detailed Event Generation Mechanics

**Probabilistic System:**
```cpp
struct EventProbability {
    EventType type;
    float baseProbability;    // Base probability
    float levelMultiplier;    // Multiplier from party level
    int minLevel;            // Minimum level for event
    int maxLevel;            // Maximum level for event
};

class EventGenerator {
private:
    map<LocationType, vector<EventProbability>> locationProbabilities;
    RandomGenerator random;

    // Adjust probabilities based on party level
    vector<float> adjustProbabilities(LocationType location, int partyLevel) {
        vector<float> adjusted;
        for (auto& prob : locationProbabilities[location]) {
            if (partyLevel >= prob.minLevel && partyLevel <= prob.maxLevel) {
                float adjustedProb = prob.baseProbability *
                                    (1.0 + prob.levelMultiplier * (partyLevel - 1));
                adjusted.push_back(adjustedProb);
            } else {
                adjusted.push_back(0.0); // Event unavailable for this level
            }
        }
        return adjusted;
    }

public:
    Event* generateEvent(Location* location, int partyLevel) {
        auto probabilities = adjustProbabilities(location->getType(), partyLevel);
        EventType type = weightedRandomSelect(probabilities);
        return createEvent(type, location, partyLevel);
    }

    // Weighted random selection
    EventType weightedRandomSelect(const vector<float>& probabilities) {
        float total = accumulate(probabilities.begin(), probabilities.end(), 0.0f);
        float randomValue = random.nextFloat(0.0f, total);

        float cumulative = 0.0f;
        for (int i = 0; i < probabilities.size(); i++) {
            cumulative += probabilities[i];
            if (randomValue <= cumulative) {
                return static_cast<EventType>(i);
            }
        }
        return COMBAT; // Fallback
    }
};
```

**Probability Configuration by Locations:**
```cpp
// Initialize event probabilities
void initializeProbabilities() {
    locationProbabilities[FOREST] = {
        {COMBAT, 0.6, 0.1, 1, 10},     // 60% base, +10% per level
        {TREASURE, 0.2, 0.05, 1, 10},  // 20% base, +5% per level
        {RECRUIT, 0.1, 0.02, 2, 8},    // Only from level 2
        {TRAP, 0.05, 0.01, 1, 10},
        {REST, 0.05, 0.0, 1, 10}       // Probability doesn't grow with level
    };

    locationProbabilities[CAVE] = {
        {COMBAT, 0.7, 0.15, 1, 10},    // Caves are more dangerous
        {TREASURE, 0.15, 0.08, 1, 10}, // But rewards are better
        {TRAP, 0.1, 0.05, 1, 10},
        {REST, 0.05, 0.0, 1, 10}
    };

    locationProbabilities[CASTLE] = {
        {BOSS, 1.0, 0.0, 3, 10}        // Only boss from level 3
    };
}
```

**Dynamic Event Content Generation:**
```cpp
Event* createEvent(EventType type, Location* location, int partyLevel) {
    Event* event = new Event();
    event->type = type;
    event->difficultyLevel = calculateDifficulty(partyLevel, location);

    switch (type) {
    case COMBAT:
        event->involvedEntities = generateEnemies(event->difficultyLevel, location);
        event->rewards = generateCombatRewards(event->difficultyLevel);
        break;

    case TREASURE:
        event->rewards = generateTreasure(event->difficultyLevel);
        event->choices = {"Take the treasure", "Leave it", "Examine surroundings"};
        break;

    case RECRUIT:
        event->involvedEntities = {generateRecruitableHero(partyLevel)};
        event->choices = {"Accept into party", "Refuse", "Exchange items"};
        break;
    }

    return event;
}
```

**Choice Consequence System:**
- Each event choice has consequences
- Influence on character relationships
- Change in future event probabilities
- Temporary effects (buffs/debuffs)

## Interface Solutions

### Console Interface (Current Stage)
- Text representation of game state
- Command input for control
- Step-by-step battle display

### Planned Graphical Interface
- SDL2 or similar library
- Isometric or 2D representation
- Battle and movement animations

## Algorithms and Data Structures

### 1. Battle Turn Queue
```cpp
vector<pair<Entity*, int>> calculateTurnOrder() {
    vector<pair<Entity*, int>> order;

    for (Entity* entity : allEntities) {
        int weight = entity->getInitiative() / 10;
        for (int i = 0; i < weight; i++) {
            order.push_back({entity, calculatePriority(entity)});
        }
    }

    sort(order.begin(), order.end(),
          [](auto a, auto b) { return a.second > b.second; });
    return order;
}
```

### 2. Path Finding Between Locations
```cpp
vector<Location*> findPath(Location* start, Location* end) {
    // A* or BFS algorithm to find shortest path
    // Considers travel cost and location accessibility
}
```

### 3. Experience Distribution
```cpp
void Party::distributeExperience(int totalExp) {
    int expPerMember = totalExp / members.size();
    for (Player* member : members) {
        if (!member->isDead()) {
            member->addExperience(expPerMember);
        }
    }
}
```

## Limitations and Requirements

### Technical Limitations
- Maximum party size: 4 characters
- Maximum enemies in battle: 6
- Number of locations: 4 main + connections
- Inventory depth: limited only by memory

### Performance Requirements
- Fast random event generation
- Efficient turn order calculation
- Minimal delays in battle display

## Development Recommendations

### 1. Implementation Priorities
1. **Core systems** - BattleSystem, PartySystem
2. **Navigation** - LocationSystem, EventSystem
3. **Content** - enemies, items, events
4. **Interface** - console → graphical

### 2. Testing
- Unit tests for each system
- Integration tests for system interactions
- Balance tests for battle and event generation

### 3. Balancing
- Gradual enemy stat adjustment
- Initiative system balance
- Even event distribution across locations

## Future Expansions

### Possible Improvements
1. **Skill system** - unique character abilities
2. **Status effects** - poison, weakening, etc.
3. **Weather conditions** - influence on battle and movement
4. **Dialogue system** - interaction with NPCs
5. **Achievement system** - progress tracking

### Meta-progress (Optional)
- Permanent improvements between runs
- Unlocking new characters
- Improving base stats

This architecture provides a solid foundation for developing a full-fledged game in the style of Darkest Dungeon with the possibility of gradual functionality expansion.