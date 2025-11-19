# TODO: Add Shared Inventory and Save/Load System

## Phase 1: Implement Shared Inventory

- [ ] Add `vector<Item> partyInventory` to CampaignSystem.h
- [ ] Modify `handleTreasureEvent` in CampaignSystem.cpp to add items to `partyInventory`
- [ ] Update `manageInventory` in CampaignSystem.cpp to show shared inventory and allow equipping on any hero
- [ ] Remove `m_inventory` from Player class in entity.h
- [ ] Adjust Player methods: `addItem`, `equipItem`, `unequipItem`, `displayInventory`, `useConsumable` to work with shared inventory
- [ ] Update `displayInventory` in Player to show from CampaignSystem
- [ ] Test inventory management in game

## Phase 2: Implement Save/Load System

- [ ] Add `saveGame(const string& filename)` and `loadGame(const string& filename)` to CampaignSystem.h
- [ ] Implement saveGame: serialize playerParty, currentLocation, gameMap, currentDifficulty, gameCompleted, visitedNodes, partyInventory
- [ ] Implement loadGame: deserialize and restore state
- [ ] Add "Load Game" option to main menu in main.cpp
- [ ] Add save option in map mode (press 's' to save)
- [ ] Test save/load functionality
