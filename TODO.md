# TODO - Map Random Prototype Selection and Event Randomization

## Objective

Implement multiple map prototypes for each map type (forest, city, castle, cave) and randomize map selection and event types on 'R' positions.

## Tasks

1. Add user-provided map prototypes (4 variants per map type) as static const 15x15 char arrays in Map.cpp.

2. Add enum or constants for map types (FOREST, CITY, CASTLE, CAVE) if not present.

3. Update Map::generate() to:
   - randomly select a map type
   - randomly select one of the 4 prototypes of that type
   - load prototype with new helper function

4. Implement helper function:
   - parsePrototype(const char[15][15])
   - convert prototype chars to NodeType grid
   - For 'R' cells, randomly assign NodeType::BATTLE, NodeType::TREASURE, or NodeType::EVENT

5. Enforce castle rules:
   - exactly one boss (D)
   - at least one battle (B) node leading to boss
   - no exits (V)

6. Retain player start (I) location from prototype.

7. Adjust connectNodes or ensureConnectivity if needed after loading prototype

8. Test map generation randomization and event assignment.

## Files to edit

- Map.cpp
- Map.h

## Next steps

- Implement prototypes data arrays and parsing
- Modify generate() logic
- Add random event assignment on 'R'
- Handle castle-specific rules
