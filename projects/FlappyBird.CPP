#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

// ─────────────────────────────────────────────────────────────
//  Constants
// ─────────────────────────────────────────────────────────────
const int   WINDOW_WIDTH        = 800;
const int   WINDOW_HEIGHT       = 600;

const float BIRD_SIZE           = 0.05f;
const float GRAVITY             = 0.0005f;   // FIX #1 – original 0.0008 was too strong
const float JUMP_POWER          = 0.018f;    // FIX #2 – original 0.03 sent bird off-screen
const float PIPE_WIDTH          = 0.12f;
const float PIPE_GAP            = 0.38f;     // FIX #3 – original 0.25 gap was unplayable
const float PIPE_SPEED          = 0.004f;
// FIX #4 – replaced broken time-based spawn with distance-based spawn
const float PIPE_SPAWN_DIST     = 0.80f;     // spawn a new pipe every 0.8 units of screen

// ─────────────────────────────────────────────────────────────
//  Enums
// ─────────────────────────────────────────────────────────────
enum BirdColor { RED, BLUE, GREEN, YELLOW, PURPLE };
enum GameState { MENU, PLAYING, GAME_OVER };

// ─────────────────────────────────────────────────────────────
//  Structs
// ─────────────────────────────────────────────────────────────
struct Button {
    float x, y, width, height;
    bool isClicked(float mx, float my) const {
        return mx >= x && mx <= x + width && my >= y && my <= y + height;
    }
};

struct Bird {
    float x, y;
    float velocity;
    BirdColor color;
};

struct Pipe {
    float x;
    float gapY;   // y-coordinate of the BOTTOM of the gap
    bool  scored;
};

// ─────────────────────────────────────────────────────────────
//  Globals
// ─────────────────────────────────────────────────────────────
Bird      bird            = { -0.5f, 0.0f, 0.0f, RED };
Pipe      pipes[20];
int       pipeCount       = 0;
int       score           = 0;
int       highScore       = 0;
float     lastPipeX       = 0.5f;   // tracks x of the most recently spawned pipe
GameState gameState       = MENU;

Button playButton         = { -0.2f, -0.12f, 0.4f, 0.12f };
Button colorButtons[5];

// ─────────────────────────────────────────────────────────────
//  Drawing helpers
// ─────────────────────────────────────────────────────────────
void drawCircle(float cx, float cy, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + radius * cosf(angle), cy + radius * sinf(angle));
    }
    glEnd();
}

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

// Draw a simple text-like score using thick rectangles (no font library needed)
// Draws digit characters at position (sx, sy) with given size
void drawDigit(int d, float sx, float sy, float sz) {
    // segment presence table: top, top-left, top-right, mid, bot-left, bot-right, bottom
    bool seg[10][7] = {
        {1,1,1,0,1,1,1}, // 0
        {0,0,1,0,0,1,0}, // 1
        {1,0,1,1,1,0,1}, // 2
        {1,0,1,1,0,1,1}, // 3
        {0,1,1,1,0,1,0}, // 4
        {1,1,0,1,0,1,1}, // 5
        {1,1,0,1,1,1,1}, // 6
        {1,0,1,0,0,1,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}, // 9
    };
    float t = sz * 0.08f; // thickness
    float w = sz * 0.5f;
    float h = sz;
    if (seg[d][0]) drawRect(sx,           sy + h - t,   w, t);       // top
    if (seg[d][1]) drawRect(sx,           sy + h/2,      t, h/2 - t); // top-left
    if (seg[d][2]) drawRect(sx + w - t,   sy + h/2,      t, h/2 - t); // top-right
    if (seg[d][3]) drawRect(sx,           sy + h/2 - t/2,w, t);       // middle
    if (seg[d][4]) drawRect(sx,           sy,            t, h/2 - t); // bot-left
    if (seg[d][5]) drawRect(sx + w - t,   sy,            t, h/2 - t); // bot-right
    if (seg[d][6]) drawRect(sx,           sy,            w, t);       // bottom
}

void drawScore(int s, float x, float y, float sz) {
    std::string str = std::to_string(s);
    float spacing = sz * 0.6f;
    // center the number
    float totalW = str.size() * spacing;
    float startX = x - totalW / 2.0f;
    for (size_t i = 0; i < str.size(); i++) {
        drawDigit(str[i] - '0', startX + i * spacing, y, sz);
    }
}

// ─────────────────────────────────────────────────────────────
//  Set colour from BirdColor enum
// ─────────────────────────────────────────────────────────────
void setBirdColor(BirdColor c) {
    switch (c) {
        case RED:    glColor3f(1.0f, 0.3f, 0.3f); break;
        case BLUE:   glColor3f(0.3f, 0.6f, 1.0f); break;
        case GREEN:  glColor3f(0.3f, 1.0f, 0.3f); break;
        case YELLOW: glColor3f(1.0f, 1.0f, 0.3f); break;
        case PURPLE: glColor3f(0.8f, 0.3f, 1.0f); break;
    }
}

// ─────────────────────────────────────────────────────────────
//  Draw bird
// ─────────────────────────────────────────────────────────────
void drawBird(const Bird& b) {
    setBirdColor(b.color);
    drawCircle(b.x, b.y, BIRD_SIZE, 32);

    // Eye white
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(b.x + BIRD_SIZE * 0.5f, b.y + BIRD_SIZE * 0.35f, BIRD_SIZE * 0.32f, 16);

    // Pupil
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(b.x + BIRD_SIZE * 0.55f, b.y + BIRD_SIZE * 0.35f, BIRD_SIZE * 0.16f, 12);

    // Beak
    glColor3f(1.0f, 0.6f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(b.x + BIRD_SIZE,        b.y);
    glVertex2f(b.x + BIRD_SIZE * 1.6f, b.y + BIRD_SIZE * 0.1f);
    glVertex2f(b.x + BIRD_SIZE,        b.y - BIRD_SIZE * 0.25f);
    glEnd();
}

// ─────────────────────────────────────────────────────────────
//  Draw pipes
// ─────────────────────────────────────────────────────────────
void drawPipes() {
    for (int i = 0; i < pipeCount; i++) {
        // Pipe body colour
        glColor3f(0.2f, 0.75f, 0.2f);

        // Bottom pipe  (from bottom of screen up to gapY)
        drawRect(pipes[i].x, -1.0f, PIPE_WIDTH, pipes[i].gapY + 1.0f);

        // Top pipe  (from gapY + GAP up to top of screen)
        drawRect(pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, 2.0f);

        // Pipe cap highlights
        glColor3f(0.15f, 0.55f, 0.15f);
        // bottom cap
        drawRect(pipes[i].x - 0.01f, pipes[i].gapY - 0.04f, PIPE_WIDTH + 0.02f, 0.04f);
        // top cap
        drawRect(pipes[i].x - 0.01f, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH + 0.02f, 0.04f);
    }
}

// ─────────────────────────────────────────────────────────────
//  Draw ground
// ─────────────────────────────────────────────────────────────
void drawGround() {
    glColor3f(0.55f, 0.37f, 0.18f);
    drawRect(-1.0f, -1.0f, 2.0f, 0.08f);
    glColor3f(0.35f, 0.70f, 0.25f);
    drawRect(-1.0f, -0.92f, 2.0f, 0.03f);
}

// ─────────────────────────────────────────────────────────────
//  Draw menu
// ─────────────────────────────────────────────────────────────
void drawMenu() {
    // Title banner
    glColor3f(1.0f, 0.65f, 0.0f);
    drawRect(-0.55f, 0.52f, 1.1f, 0.22f);

    glColor3f(0.9f, 0.2f, 0.1f);
    drawRect(-0.50f, 0.56f, 1.0f, 0.14f);

    // Bird preview (use current color)
    drawBird(bird);

    // PLAY button
    glColor3f(0.15f, 0.75f, 0.15f);
    drawRect(playButton.x, playButton.y, playButton.width, playButton.height);

    // "PLAY" label bars (crude but no font needed)
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRect(-0.07f, -0.06f, 0.14f, 0.02f);
    drawRect(-0.07f, -0.09f, 0.14f, 0.02f);

    // Colour picker buttons
    BirdColor colors[] = { RED, BLUE, GREEN, YELLOW, PURPLE };
    for (int i = 0; i < 5; i++) {
        colorButtons[i] = { -0.82f + i * 0.37f, -0.72f, 0.30f, 0.18f };
        setBirdColor(colors[i]);
        drawRect(colorButtons[i].x, colorButtons[i].y,
                 colorButtons[i].width, colorButtons[i].height);

        // Highlight selected
        if (bird.color == colors[i]) {
            glColor3f(1.0f, 1.0f, 1.0f);
            // thin border – draw 4 thin rects around the button
            float bx = colorButtons[i].x, by = colorButtons[i].y;
            float bw = colorButtons[i].width, bh = colorButtons[i].height;
            float t = 0.012f;
            drawRect(bx - t, by - t, bw + 2*t, t);       // bottom
            drawRect(bx - t, by + bh, bw + 2*t, t);      // top
            drawRect(bx - t, by, t, bh);                  // left
            drawRect(bx + bw, by, t, bh);                 // right
        }
    }

    // Instructions (tiny coloured bars stand-in for text)
    glColor3f(0.2f, 0.2f, 0.2f);
    drawRect(-0.45f, -0.88f, 0.9f, 0.025f);
    drawRect(-0.35f, -0.92f, 0.7f, 0.025f);
}

// ─────────────────────────────────────────────────────────────
//  Draw game-over overlay
// ─────────────────────────────────────────────────────────────
void drawGameOver() {
    // Semi-transparent dark overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    drawRect(-1.0f, -1.0f, 2.0f, 2.0f);

    // Panel
    glColor3f(1.0f, 0.95f, 0.85f);
    drawRect(-0.5f, 0.1f, 1.0f, 0.55f);

    // Red "game over" bar
    glColor3f(0.85f, 0.15f, 0.1f);
    drawRect(-0.45f, 0.50f, 0.90f, 0.12f);

    // Score display (on-screen using 7-seg digits) – FIX #5: score now shows in-game
    glColor3f(0.2f, 0.2f, 0.2f);
    drawScore(score,     -0.18f, 0.18f, 0.22f);

    glColor3f(0.8f, 0.55f, 0.0f);
    drawScore(highScore,  0.18f, 0.18f, 0.22f);

    // Divider
    glColor3f(0.75f, 0.75f, 0.75f);
    drawRect(-0.01f, 0.15f, 0.02f, 0.30f);

    // Restart button
    glColor3f(0.15f, 0.75f, 0.15f);
    drawRect(playButton.x, playButton.y, playButton.width, playButton.height);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRect(-0.07f, -0.06f, 0.14f, 0.02f);
    drawRect(-0.07f, -0.09f, 0.14f, 0.02f);
}

// ─────────────────────────────────────────────────────────────
//  Update bird physics
// ─────────────────────────────────────────────────────────────
void updateBird(float dt) {
    if (gameState != PLAYING) return;

    bird.velocity -= GRAVITY;          // apply gravity every frame (dt-scaled below)
    bird.y        += bird.velocity;

    if (bird.y - BIRD_SIZE <= -0.92f || bird.y + BIRD_SIZE >= 1.0f) {
        if (score > highScore) highScore = score;
        gameState = GAME_OVER;
    }
}

// ─────────────────────────────────────────────────────────────
//  Update pipes   (FIX #4 – distance-based spawning)
// ─────────────────────────────────────────────────────────────
void updatePipes() {
    if (gameState != PLAYING) return;

    // Move all pipes left
    for (int i = 0; i < pipeCount; i++) {
        pipes[i].x -= PIPE_SPEED;
    }

    // Spawn a new pipe when the last one has moved far enough left
    if (pipeCount == 0 || (lastPipeX - pipes[pipeCount - 1].x) >= PIPE_SPAWN_DIST) {
        if (pipeCount < 20) {
            // gapY is the bottom of the opening; keep it well within screen bounds
            float minGap = -0.55f;
            float maxGap =  0.42f;
            float gapY   = minGap + (rand() / (float)RAND_MAX) * (maxGap - minGap);

            pipes[pipeCount] = { 1.15f, gapY, false };
            lastPipeX = 1.15f;
            pipeCount++;
        }
    }

    // FIX #5 – score: mark pipe as scored ONLY after fully passing it, use correct index
    for (int i = 0; i < pipeCount; i++) {
        if (!pipes[i].scored && pipes[i].x + PIPE_WIDTH < bird.x - BIRD_SIZE) {
            pipes[i].scored = true;
            score++;
        }
    }

    // Remove off-screen pipes
    int keep = 0;
    for (int i = 0; i < pipeCount; i++) {
        if (pipes[i].x + PIPE_WIDTH > -1.05f) {
            pipes[keep++] = pipes[i];
        }
    }
    pipeCount = keep;
}

// ─────────────────────────────────────────────────────────────
//  Collision detection
// ─────────────────────────────────────────────────────────────
bool checkCollision() {
    for (int i = 0; i < pipeCount; i++) {
        bool xOverlap = bird.x + BIRD_SIZE > pipes[i].x &&
                        bird.x - BIRD_SIZE < pipes[i].x + PIPE_WIDTH;
        bool yOutside = bird.y - BIRD_SIZE < pipes[i].gapY ||
                        bird.y + BIRD_SIZE > pipes[i].gapY + PIPE_GAP;
        if (xOverlap && yOutside) return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────
//  Render  (FIX #6 – in-game score drawn on screen)
// ─────────────────────────────────────────────────────────────
void render() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameState == MENU) {
        drawGround();
        drawMenu();
    }
    else if (gameState == PLAYING) {
        drawGround();
        drawPipes();
        drawBird(bird);

        // On-screen score (white 7-seg digits, top-centre)
        glColor3f(1.0f, 1.0f, 1.0f);
        drawScore(score, 0.0f, 0.72f, 0.18f);
    }
    else if (gameState == GAME_OVER) {
        drawGround();
        drawPipes();
        drawBird(bird);
        drawGameOver();
    }
}

// ─────────────────────────────────────────────────────────────
//  Input callbacks
// ─────────────────────────────────────────────────────────────
void resetGame() {
    bird.x        = -0.5f;
    bird.y        =  0.0f;
    bird.velocity =  0.0f;
    score         =  0;
    pipeCount     =  0;
    lastPipeX     =  0.5f;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    float nx = (float)(xpos / WINDOW_WIDTH  * 2.0 - 1.0);
    float ny = (float)(1.0  - ypos / WINDOW_HEIGHT * 2.0);

    if (gameState == MENU) {
        if (playButton.isClicked(nx, ny)) {
            resetGame();
            gameState = PLAYING;
            return;
        }
        for (int i = 0; i < 5; i++) {
            if (colorButtons[i].isClicked(nx, ny)) {
                bird.color = static_cast<BirdColor>(i);
                return;
            }
        }
    }
    else if (gameState == PLAYING) {
        bird.velocity = JUMP_POWER;
    }
    else if (gameState == GAME_OVER) {
        if (playButton.isClicked(nx, ny)) {
            gameState = MENU;
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_SPACE:
            if (gameState == MENU) {
                resetGame();
                gameState = PLAYING;
            } else if (gameState == PLAYING) {
                bird.velocity = JUMP_POWER;
            } else if (gameState == GAME_OVER) {
                gameState = MENU;
            }
            break;
        case GLFW_KEY_1: bird.color = RED;    break;
        case GLFW_KEY_2: bird.color = BLUE;   break;
        case GLFW_KEY_3: bird.color = GREEN;  break;
        case GLFW_KEY_4: bird.color = YELLOW; break;
        case GLFW_KEY_5: bird.color = PURPLE; break;
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
}

// ─────────────────────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────────────────────
int main() {
    srand(static_cast<unsigned>(time(0)));

    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "Flappy Bird  |  SPACE = Jump  |  1-5 = Color  |  ESC = Quit",
        nullptr, nullptr
    );
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);   // vsync – limits to ~60 fps automatically

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW\n";
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "\n=== FLAPPY BIRD ===\n"
              << "  SPACE / Left-Click  – Jump\n"
              << "  Keys 1-5            – Change bird colour\n"
              << "  ESC                 – Quit\n"
              << "===================\n\n";

    while (!glfwWindowShouldClose(window)) {
        updateBird(0);      // dt not needed – vsync keeps frame rate steady
        updatePipes();

        if (checkCollision() && gameState == PLAYING) {
            if (score > highScore) highScore = score;
            gameState = GAME_OVER;
        }

        render();
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (gameState == PLAYING) {
            std::cout << "\rScore: " << score
                      << "  |  Best: " << highScore << "      ";
            std::cout.flush();
        }
    }

    std::cout << "\nFinal score: " << score
              << "  |  Best: " << highScore << "\n";
    glfwTerminate();
    return 0;
}