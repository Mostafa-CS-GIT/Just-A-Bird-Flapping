# 🦜 Flappy Bird — Game Mechanics Customization Guide

An interactive OpenGL Flappy Bird game built with **C++**, **GLFW**, and **GLEW**. This guide explains every customizable constant so you can tweak game mechanics, difficulty, and feel without touching any game logic.

## 🎮 About This Game

**Flappy Bird** is a classic arcade-style game where you control a bird navigating through pipes. Press **SPACE** to jump and avoid obstacles. This implementation features:

- 🖥️ Real-time OpenGL rendering with smooth 60 FPS gameplay
- 🎯 Dynamic collision detection
- 🎨 Customizable bird appearance and game mechanics
- ⚙️ Full parameter adjustment without code logic chan
## 📋 Game Constants

All customizable values are at the top of `FlappyBird.cpp` after the `#include` lines. After any change, rebuild with **Ctrl + Shift + B**.

---

## Where to find the constants

Open `FlappyBird.cpp`. The constants are grouped at the top, right after the `#include` lines:

```cpp
const int   WINDOW_WIDTH        = 800;
const int   WINDOW_HEIGHT       = 600;

const float BIRD_SIZE           = 0.05f;
const float GRAVITY             = 0.0005f;
const float JUMP_POWER          = 0.018f;
const float PIPE_WIDTH          = 0.12f;
const float PIPE_GAP            = 0.38f;
const float PIPE_SPEED          = 0.004f;
const float PIPE_SPAWN_DIST     = 0.80f;
```

**Coordinate System:** The game space runs from **-1.0 to +1.0** on both axes. All values are fractions of the screen — think of them as percentages. The actual window resolution is controlled by `WINDOW_WIDTH` and `WINDOW_HEIGHT`.

---

## 📐 Constants Reference

### `WINDOW_WIDTH` & `WINDOW_HEIGHT`
**Defaults:** `800` × `600`

The display resolution in pixels.

| Values | Effect |
|--------|--------|
| `800 × 600` | Default — comfortable window |
| `1024 × 768` | HD-ready — larger screen |
| `1280 × 720` | Wide aspect ratio |
| `640 × 480` | Windowed — smaller footprint |

> The game automatically scales to fit any resolution — no gameplay changes needed.

---

### `BIRD_SIZE`
**Default:** `0.05f`

The radius of the bird (circle) in screen units.

| Value | Effect |
|-------|--------|
| `0.03f` | Tiny bird — easier to squeeze through gaps |
| `0.05f` | **Default** — balanced |
| `0.08f` | Large bird — much harder, tighter gaps |
| `0.12f` | Huge bird — nearly impossible |

> Collision detection automatically scales with this value.

---

### `GRAVITY`
**Default:** `0.0005f`

How fast the bird accelerates downward each frame. Higher = falls faster and heavier.

| Value | Effect |
|-------|--------|
| `0.0002f` | Floaty, drifts down slowly — very easy |
| `0.0005f` | **Default** — balanced and responsive |
| `0.0008f` | Heavy, quick fall — challenging |
| `0.0012f` | Extreme weight — nearly impossible |

> This is applied **60 times per second** (vsync), so even `0.0001f` changes feel significant. Start with small adjustments.

---

### `JUMP_POWER`
**Default:** `0.018f`

The upward velocity applied when you press SPACE. Higher = bigger jump.

| Value | Effect |
|-------|--------|
| `0.010f` | Tiny hop — hard to gain altitude |
| `0.018f` | **Default** — matches gravity well |
| `0.025f` | Strong jump — easy to overshoot |
| `0.035f` | Rocket launch — flies off-screen fast |

> **Balance Tip:** Keep `JUMP_POWER` at **30–40× GRAVITY** for natural-feeling physics. If you increase gravity, increase jump power proportionally.

---

### `PIPE_GAP`
**Default:** `0.38f`

The vertical space between top and bottom pipes. Higher = wider gap = easier.

| Value | Effect |
|-------|--------|
| `0.20f` | Tiny gap — expert/nightmare mode |
| `0.30f` | Narrow — very challenging |
| `0.38f` | **Default** — comfortable difficulty |
| `0.50f` | Wide — beginner-friendly |
| `0.70f` | Very easy — almost impossible to lose |

> **Safety Rule:** Gap must always be **> `BIRD_SIZE * 2`** or the game is physically impossible. Absolute minimum is around `0.15f`.

---

### `PIPE_WIDTH`
**Default:** `0.12f`

The horizontal thickness of each pipe. Wider pipes leave less time to react.

| Value | Effect |
|-------|--------|
| `0.07f` | Thin — more forgiving |
| `0.12f` | **Default** |
| `0.18f` | Fat — tight timing windows |
| `0.25f` | Very thick — reaction test |

---

### `PIPE_SPEED`
**Default:** `0.004f`

How many screen units each pipe moves left per frame. Higher = faster = harder.

| Value | Effect |
|-------|--------|
| `0.002f` | Slow — relaxed, arcade-style |
| `0.004f` | **Default** |
| `0.006f` | Fast — requires quick reactions |
| `0.010f` | Very fast — extreme difficulty |

> Increasing `PIPE_SPEED` makes pipes leave the screen faster. If you speed this up, consider raising `PIPE_SPAWN_DIST` to maintain comfortable pipe spacing.

---

### `PIPE_SPAWN_DIST`
**Default:** `0.80f`

The horizontal distance between consecutive pipe spawns. Lower = more frequent pipes.

| Value | Effect |
|-------|--------|
| `0.50f` | Dense — pipes spawn very close, hectic |
| `0.80f` | **Default** — comfortable spacing |
| `1.20f` | Sparse — wide gaps between pipes |
| `1.60f` | Very sparse — lots of breathing room |

> **Safety Rule:** Never set below `PIPE_WIDTH` or pipes will overlap. Keep above `0.30f` to avoid impossible density.

---

## 🎯 Difficulty Presets

You can copy any of these preset blocks directly over the constants at the top of `FlappyBird.cpp`.

### 🟢 Easy Mode
Perfect for learning the controls and casual play.

```cpp
const float BIRD_SIZE           = 0.05f;
const float GRAVITY             = 0.0003f;
const float JUMP_POWER          = 0.014f;
const float PIPE_WIDTH          = 0.10f;
const float PIPE_GAP            = 0.52f;
const float PIPE_SPEED          = 0.003f;
const float PIPE_SPAWN_DIST     = 1.00f;
```

### 🟡 Default (Balanced)
The original configuration — good starting point.

```cpp
const float BIRD_SIZE           = 0.05f;
const float GRAVITY             = 0.0005f;
const float JUMP_POWER          = 0.018f;
const float PIPE_WIDTH          = 0.12f;
const float PIPE_GAP            = 0.38f;
const float PIPE_SPEED          = 0.004f;
const float PIPE_SPAWN_DIST     = 0.80f;
```

### 🔴 Hard Mode
For experienced players seeking a real challenge.

```cpp
const float BIRD_SIZE           = 0.05f;
const float GRAVITY             = 0.0007f;
const float JUMP_POWER          = 0.020f;
const float PIPE_WIDTH          = 0.16f;
const float PIPE_GAP            = 0.28f;
const float PIPE_SPEED          = 0.006f;
const float PIPE_SPAWN_DIST     = 0.65f;
```

### ⚫ Chaos Mode
For the truly brave — extreme difficulty.

```cpp
const float BIRD_SIZE           = 0.07f;
const float GRAVITY             = 0.0010f;
const float JUMP_POWER          = 0.026f;
const float PIPE_WIDTH          = 0.20f;
const float PIPE_GAP            = 0.24f;
const float PIPE_SPEED          = 0.009f;
const float PIPE_SPAWN_DIST     = 0.55f;
```

---

## 🎨 Customization Examples

### Make the game slower & driftier
```cpp
const float GRAVITY      = 0.0002f;
const float JUMP_POWER   = 0.012f;
const float PIPE_SPEED   = 0.002f;
```
Result: Floaty, relaxed gameplay — great for learning.

### Tighten the gaps for a skill test
```cpp
const float PIPE_GAP     = 0.25f;
const float BIRD_SIZE    = 0.06f;
```
Result: Requires precision and quick reflexes.

### Speed run challenge
```cpp
const float PIPE_SPEED   = 0.008f;
const float PIPE_SPAWN_DIST = 0.60f;
```
Result: Non-stop action — can you keep up?

---

## 🎮 Controls

| Input | Action |
|-------|--------|
| **SPACE** | Jump / Start game |
| **ESC** | Quit game |

---

## 🔧 How to Make Changes

1. Open `projects/FlappyBird.cpp` in your editor
2. Find the constants section at the top (lines 8–20)
3. Edit any value — e.g., change `GRAVITY` from `0.0005f` to `0.0003f`
4. Save the file
5. Build and run with **Ctrl + Shift + B** (VS Code) or:
   ```bash
   g++ -o FlappyBird.exe FlappyBird.CPP \
     -I D:\APPS\msys64\ucrt64\include \
     -L D:\APPS\msys64\ucrt64\lib \
     -lglfw3 -lglew32 -lopengl32
   ./FlappyBird.exe
   ```

That's it! No other code needs to change. The game logic automatically uses your new values.

---

## 📚 Advanced Tips

### Finding Your Perfect Difficulty
- Start with a **preset** above
- Make **one change at a time** and test
- Increase gravity for heavier feel? **Increase jump power too**
- Want tighter gameplay? **Decrease PIPE_GAP and increase PIPE_SPEED together**

### Physics Balance
The game's feel depends on the relationship between constants:
- `JUMP_POWER ÷ GRAVITY` should stay **30–40×** for natural physics
- `PIPE_SPEED` relative to `PIPE_SPAWN_DIST` controls how "crowded" the pipes feel
- `BIRD_SIZE` vs `PIPE_GAP` determines difficulty ceiling

### Testing New Settings
To quickly test, try editing just these three:
```cpp
const float BIRD_SIZE      = 0.05f;    // ← try 0.03 or 0.08
const float GRAVITY        = 0.0005f;  // ← try 0.0003 or 0.0007
const float PIPE_GAP       = 0.38f;    // ← try 0.30 or 0.50
```

---

## 🚀 Project Structure

```
OPEN-GL-TEST/
├── projects/
│   └── FlappyBird.CPP          # Main game source
├── README.md                   # This file
└── .git/                       # Version control
```

---

## 🛠️ Requirements

- **C++ Compiler** (g++ or MSVC)
- **GLFW3** (windowing library)
- **GLEW** (OpenGL extension loader)
- **OpenGL 2.0+**

---

## 💡 Have Fun!

The beauty of Flappy Bird is its simplicity—but these constants let you explore vast design space. Try wild combinations, find your favorite difficulty, or create your own "signature" game feel. Share your discoveries! 🦜

---

## Bird colours (if applicable)

> **Note:** In the current version, bird appearance is simplified. To add colors back, uncomment the color selection code in `render()` and rebuild.

---

---

## Quick rebuild reminder

Every time you edit `FlappyBird.cpp`:

1. Save the file (`Ctrl + S`)
2. Press `Ctrl + Shift + B` in VS Code — this compiles and launches the new `.exe` automatically
3. Or double-click `build.bat` in File Explorer for the same result
