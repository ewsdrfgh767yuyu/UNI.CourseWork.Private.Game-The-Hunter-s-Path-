# TODO: Redesign Game for Full-Screen Mode

## Step 1: Modify Window Creation

- Change main.cpp to create full-screen window instead of fixed 800x1000 size
- Use sf::VideoMode::getDesktopMode() for full-screen resolution

## Step 2: Update GUI Classes for Relative Positioning

- [x] Modify GUI.h and GUI.cpp to support relative positioning (percentages of window size)
- [x] Add methods to Button and TextDisplay for setting positions/sizes relative to window
- [x] Update Menu class to handle relative coordinates
 т
## Step 3: Update main.cpp Positions and Sizes

- Replace all hardcoded positions and sizes with relative calculations
- Update button positions, text positions, and other UI elements to use window percentages
- Adjust font sizes to scale with window size

## Step 4: Test and Adjust Scaling

- Compile and run the game in full-screen mode
- Verify all buttons and text scale properly
- Make any necessary adjustments for readability and usability
