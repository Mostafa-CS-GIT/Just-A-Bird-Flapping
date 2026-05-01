#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

// Game constants
const float WINDOW_WIDTH = 800.0f;
const float WINDOW_HEIGHT = 600.0f;
const float BIRD_SIZE = 0.04f;
const float GRAVITY = 0.0015f;
const float JUMP_POWER = 0.045f;
const float PIPE_WIDTH = 0.15f;
const float PIPE_GAP = 0.35f;
const float PIPE_SPEED = 0.0035f;
const float PIPE_SPAWN_INTERVAL = 0.2f;
const float INITIAL_PIPE_DELAY = 1.5f;

// Bird color options
enum BirdColor { RED, BLUE, GREEN, YELLOW, PURPLE };

// Game states
enum GameState { MENU, PLAYING, GAME_OVER };

// Bird structure
struct Bird {
    float x, y;
    float velocity;
    BirdColor color;
};

// Pipe structure
struct Pipe {
    float x;
    float gapY;
};

// Global variables
Bird bird = {0.1f, 0.5f, 0.0f, RED};
Pipe pipes[10];
int pipeCount = 0;
int score = 0;
int highScore = 0;
float pipeTimer = 0.0f;
float gameStartTimer = 0.0f;
GameState gameState = MENU;
float gameSpeed = 1.0f;

// Function to draw a circle (for bird)
void drawCircle(float cx, float cy, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

// Function to draw a rectangle
void drawRect(float x, float y, float width, float height) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to draw the bird
void drawBird(Bird& b) {
    // Set bird color based on selection
    switch(b.color) {
        case RED:
            glColor3f(1.0f, 0.2f, 0.2f);
            break;
        case BLUE:
            glColor3f(0.2f, 0.5f, 1.0f);
            break;
        case GREEN:
            glColor3f(0.2f, 1.0f, 0.2f);
            break;
        case YELLOW:
            glColor3f(1.0f, 1.0f, 0.2f);
            break;
        case PURPLE:
            glColor3f(1.0f, 0.2f, 1.0f);
            break;
    }
    
    drawCircle(b.x, b.y, BIRD_SIZE, 32);
    
    // Draw eye
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(b.x + BIRD_SIZE/2, b.y + BIRD_SIZE/3, BIRD_SIZE/4, 16);
}

// Function to draw pipes
void drawPipes() {
    glColor3f(0.2f, 0.8f, 0.2f);
    
    for (int i = 0; i < pipeCount; i++) {
        // Top pipe
        drawRect(pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, 2.0f);
        
        // Bottom pipe
        drawRect(pipes[i].x, -2.0f, PIPE_WIDTH, pipes[i].gapY);
    }
}

// Function to draw score
void drawScore() {
    // Score display (simple text rendering using rectangles)
    glColor3f(0.0f, 0.0f, 0.0f);
}

// Function to draw menu
void drawMenu() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // Title
    glColor3f(1.0f, 0.5f, 0.0f);
    drawRect(-0.3f, 0.3f, 0.6f, 0.15f);
    
    // Instructions
    glColor3f(0.2f, 0.2f, 0.2f);
}

// Function to draw game over screen
void drawGameOverScreen() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    drawRect(-0.2f, 0.2f, 0.4f, 0.1f);
}

// Function to update bird
void updateBird(float deltaTime) {
    if (gameState == PLAYING) {
        bird.velocity -= GRAVITY * gameSpeed;
        bird.y += bird.velocity * gameSpeed;
        
        // Collision with ground
        if (bird.y - BIRD_SIZE <= -0.95f) {
            gameState = GAME_OVER;
            if (score > highScore) highScore = score;
        }
        
        // Collision with ceiling
        if (bird.y + BIRD_SIZE >= 0.95f) {
            gameState = GAME_OVER;
            if (score > highScore) highScore = score;
        }
    }
}

// Function to update pipes
void updatePipes(float deltaTime) {
    if (gameState != PLAYING) return;
    
    gameStartTimer += deltaTime;
    pipeTimer += deltaTime;
    
    // Spawn new pipe (with initial delay to let player prepare)
    if (gameStartTimer >= INITIAL_PIPE_DELAY && pipeTimer >= PIPE_SPAWN_INTERVAL) {
        if (pipeCount < 10) {
            pipes[pipeCount].x = 1.2f;
            pipes[pipeCount].gapY = ((rand() % 60) / 100.0f) - 0.25f;
            pipeCount++;
            pipeTimer = 0.0f;
        }
    }
    
    // Move pipes
    for (int i = 0; i < pipeCount; i++) {
        pipes[i].x -= PIPE_SPEED * gameSpeed;
    }
    
    // Remove off-screen pipes and update score
    int newPipeCount = 0;
    for (int i = 0; i < pipeCount; i++) {
        if (pipes[i].x > -PIPE_WIDTH - 0.1f) {
            pipes[newPipeCount] = pipes[i];
            
            // Score increment when bird passes pipe center
            static float lastScoredX = -2.0f;
            if (pipes[i].x < bird.x && pipes[i].x > lastScoredX) {
                score++;
                lastScoredX = pipes[i].x;
            }
            
            newPipeCount++;
        }
    }
    pipeCount = newPipeCount;
}

// Function to check collision
bool checkCollision() {
    for (int i = 0; i < pipeCount; i++) {
        if (bird.x + BIRD_SIZE > pipes[i].x && bird.x - BIRD_SIZE < pipes[i].x + PIPE_WIDTH) {
            if (bird.y - BIRD_SIZE < pipes[i].gapY || bird.y + BIRD_SIZE > pipes[i].gapY + PIPE_GAP) {
                return true;
            }
        }
    }
    return false;
}

// Keyboard callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_SPACE:
                if (gameState == MENU) {
                    gameState = PLAYING;
                    bird.y = 0.0f;
                    bird.velocity = 0.0f;
                    score = 0;
                    pipeCount = 0;
                    pipeTimer = 0.0f;
                    gameStartTimer = 0.0f;
                } else if (gameState == PLAYING) {
                    bird.velocity = JUMP_POWER;
                } else if (gameState == GAME_OVER) {
                    gameState = MENU;
                }
                break;
            
            case GLFW_KEY_1:
                bird.color = RED;
                break;
            case GLFW_KEY_2:
                bird.color = BLUE;
                break;
            case GLFW_KEY_3:
                bird.color = GREEN;
                break;
            case GLFW_KEY_4:
                bird.color = YELLOW;
                break;
            case GLFW_KEY_5:
                bird.color = PURPLE;
                break;
            
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
        }
    }
}

// Function to render game
void render() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (gameState == MENU) {
        drawMenu();
    } else if (gameState == PLAYING || gameState == GAME_OVER) {
        // Draw ground
        glColor3f(0.6f, 0.4f, 0.2f);
        drawRect(-1.0f, -1.0f, 2.0f, 0.05f);
        
        // Draw pipes
        drawPipes();
        
        // Draw bird
        drawBird(bird);
        
        // Draw score on console for now
        std::cout << "\rScore: " << score << " | High Score: " << highScore << " | Bird Color: ";
        switch(bird.color) {
            case RED: std::cout << "RED"; break;
            case BLUE: std::cout << "BLUE"; break;
            case GREEN: std::cout << "GREEN"; break;
            case YELLOW: std::cout << "YELLOW"; break;
            case PURPLE: std::cout << "PURPLE"; break;
        }
        std::cout << " | State: " << (gameState == PLAYING ? "PLAYING" : "GAME OVER");
        std::cout.flush();
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    if (!glfwInit())
        return -1;
    
    GLFWwindow* window = glfwCreateWindow(
        static_cast<int>(WINDOW_WIDTH), 
        static_cast<int>(WINDOW_HEIGHT), 
        "Flappy Bird - Color Select (1-5) | Space to Jump/Play | ESC to Exit", 
        NULL, NULL
    );
    
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    
    // Set up viewport and projection
    glViewport(0, 0, static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    double lastTime = glfwGetTime();
    
    std::cout << "\n=== FLAPPY BIRD GAME ===\n";
    std::cout << "Controls:\n";
    std::cout << "  SPACE - Jump/Start Game/Restart\n";
    std::cout << "  1-5   - Change bird color (Red, Blue, Green, Yellow, Purple)\n";
    std::cout << "  ESC   - Exit\n";
    std::cout << "======================\n\n";
    
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        
        updateBird(deltaTime);
        updatePipes(deltaTime);
        
        if (checkCollision()) {
            gameState = GAME_OVER;
            if (score > highScore) highScore = score;
        }
        
        render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
