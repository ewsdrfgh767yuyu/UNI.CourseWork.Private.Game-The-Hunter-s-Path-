# TODO List for Fixing Linker Error

## Tasks to Complete

- [x] Implement `generateCorridorMap()` method in `Map.cpp` to generate a procedural corridor-based map by calling existing helper methods (`placeSpecialNodes()`, `connectNodes()`, `ensureConnectivity()`, and optionally `createCorridorBranch()` for added complexity).
- [x] Build the project using MSBuild to verify that the unresolved external symbol error for `generateCorridorMap()` is resolved.
- [x] Test the map generation functionality to ensure it produces a valid, connected map without runtime errors.
