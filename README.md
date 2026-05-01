# 🐦 Flappy Bird - Enhanced Edition

A modern implementation of the classic Flappy Bird game using **OpenGL** and **GLFW**. This version features dynamic bird color selection and a comprehensive score tracking system.

## 🎮 Game Features

### Core Gameplay
- **Classic Flappy Bird mechanics**: Navigate the bird through pipes to score points
- **Physics-based movement**: Realistic gravity and velocity simulation
- **Dynamic difficulty**: Pipes spawn at varying heights for increased challenge
- **Smooth collision detection**: Precise hit detection with pipes and boundaries

### Unique Features

#### 🎨 Bird Color Selection
Choose from 5 vibrant bird colors:
- **1** - Red Bird
- **2** - Blue Bird
- **3** - Green Bird
- **4** - Yellow Bird
- **5** - Purple Bird

Change colors anytime during gameplay!

#### 📊 Score Tracking System
- **Real-time score display** in console
- **High score persistence** throughout session
- **Score increments** for each successfully passed pipe
- **Game state indicators** (MENU, PLAYING, GAME_OVER)

## 🎮 Controls

| Key | Action |
|-----|--------|
| **SPACE** | Jump (while playing) / Start Game (in menu) / Restart (game over) |
| **1-5** | Change bird color (Red, Blue, Green, Yellow, Purple) |
| **ESC** | Exit game |

## 🛠️ Technical Details

### Dependencies
- **GLFW3**: Window management and input handling
- **GLEW**: OpenGL extension loading
- **OpenGL**: Graphics rendering

### System Requirements
- Windows (MSYS2/MinGW compatible)
- OpenGL 2.1 or higher
- G++ compiler with C++11 support

### Game Constants
```cpp
Window Size: 800x600
Bird Size: 0.04 units
Pipe Width: 0.15 units
Pipe Gap: 0.3 units
Gravity: 0.0008 units/frame
Jump Power: 0.025 units
Pipe Speed: 0.005 units/frame
```

## 🚀 Build Instructions

### Prerequisites
Ensure MSYS2 is installed with the following packages:
```bash
pacman -S mingw-w64-ucrt64-glfw
pacman -S mingw-w64-ucrt64-glew
pacman -S mingw-w64-ucrt64-opengl
```

### Compilation
```bash
cd projects/
g++.exe -fdiagnostics-color=always -g FlappyBird.CPP -o FlappyBird.exe \
  -I D:\APPS\msys64\ucrt64\include \
  -L D:\APPS\msys64\ucrt64\lib \
  -lglfw3 -lglew32 -lopengl32
```

### Running the Game
```bash
./FlappyBird.exe
```

## 📈 Game Design

### Scoring System
- **+1 point** for each pipe successfully passed
- **High score** tracking across game sessions
- Score displayed in real-time on console

### Difficulty Progression
- Pipes spawn at random heights within the visible game area
- Pipe gap remains constant (0.3 units)
- Spawn rate is fixed to maintain consistent difficulty

### Collision Detection
The bird collides with:
- Top and bottom boundaries of the game world
- Pipe obstacles (both upper and lower sections)
- Game immediately transitions to GAME_OVER state

## 🎨 Game States

1. **MENU**: Initial state, press SPACE to start
2. **PLAYING**: Active gameplay, press SPACE to jump, 1-5 to change colors
3. **GAME_OVER**: Collision detected, press SPACE to return to menu

## 📝 Code Structure

### Main Functions
- `drawCircle()`: Renders circular shapes (bird and obstacles)
- `drawRect()`: Renders rectangular shapes (pipes and ground)
- `drawBird()`: Draws the colored bird character
- `drawPipes()`: Renders all active pipe obstacles
- `updateBird()`: Updates bird physics and position
- `updatePipes()`: Manages pipe movement and spawning
- `checkCollision()`: Detects collisions between bird and obstacles
- `keyCallback()`: Handles user input events
- `render()`: Main rendering function

### Data Structures
```cpp
struct Bird {
    float x, y;              // Position
    float velocity;          // Vertical velocity
    BirdColor color;         // Visual appearance
};

struct Pipe {
    float x;                 // Horizontal position
    float gapY;              // Gap center Y position
};

enum BirdColor { RED, BLUE, GREEN, YELLOW, PURPLE };
enum GameState { MENU, PLAYING, GAME_OVER };
```

## 🔧 Configuration

### Adjustable Parameters
All game constants are defined at the top of `FlappyBird.CPP`:
- Modify `GRAVITY` for different physics feel
- Change `JUMP_POWER` for jump height
- Adjust `PIPE_SPEED` for difficulty
- Modify `PIPE_SPAWN_INTERVAL` for pipe frequency

## 🐛 Known Limitations
- Score display renders to console rather than in-game (can be enhanced with text rendering library)
- Simple 2D circle-based bird model
- No sound effects or background music
- Single player mode only

## 🚀 Future Enhancements
- On-screen score rendering
- Sound effects and background music
- Multiple difficulty levels
- Bird animations and rotation based on velocity
- Power-ups (shield, slow motion, etc.)
- Leaderboard system
- Mobile touch controls

## 📄 License
Free to use and modify for educational and personal projects.

## 👨‍💻 Developer
Created as an enhanced OpenGL game project featuring:
- Advanced color system for bird selection
- Comprehensive score tracking
- Clean C++ code structure with OpenGL rendering

---

**Enjoy the game! Challenge yourself to beat your high score!** 🎯
