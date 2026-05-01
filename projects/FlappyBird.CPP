#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

// Game constants
const float WINDOW_WIDTH = 800.0f;
const float WINDOW_HEIGHT = 600.0f;
const float BIRD_SIZE = 0.05f;
const float GRAVITY = 0.0008f;
const float JUMP_POWER = 0.03f;
const float PIPE_WIDTH = 0.12f;
const float PIPE_GAP = 0.25f;
const float PIPE_SPEED = 0.003f;
const float PIPE_SPAWN_INTERVAL = 0.25f;

// Bird color options
enum BirdColor { RED, BLUE, GREEN, YELLOW, PURPLE };

// Game states
enum GameState { MENU, PLAYING, GAME_OVER };

// Button structure
struct Button {
    float x, y;
    float width, height;
    
    bool isClicked(float mouseX, float mouseY) {
        return mouseX >= x && mouseX <= x + width && 
               mouseY >= y && mouseY <= y + height;
    }
};

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
    bool scored;
};

// Global variables
Bird bird = {-0.7f, 0.0f, 0.0f, RED};
Pipe pipes[20];
int pipeCount = 0;
int score = 0;
int highScore = 0;
float pipeTimer = 0.0f;
GameState gameState = MENU;
float gameSpeed = 1.0f;
Button playButton = {-0.2f, -0.1f, 0.4f, 0.1f};
Button colorButtons[5];

double mouseX = 0.0, mouseY = 0.0;

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
            glColor3f(1.0f, 0.3f, 0.3f);
            break;
        case BLUE:
            glColor3f(0.3f, 0.6f, 1.0f);
            break;
        case GREEN:
            glColor3f(0.3f, 1.0f, 0.3f);
            break;
        case YELLOW:
            glColor3f(1.0f, 1.0f, 0.3f);
            break;
        case PURPLE:
            glColor3f(1.0f, 0.3f, 1.0f);
            break;
    }
    
    drawCircle(b.x, b.y, BIRD_SIZE, 32);
    
    // Draw eye (white)
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(b.x + BIRD_SIZE/2, b.y + BIRD_SIZE/3, BIRD_SIZE/3, 16);
    
    // Draw pupil (black)
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(b.x + BIRD_SIZE/2, b.y + BIRD_SIZE/3, BIRD_SIZE/6, 12);
}

// Function to draw pipes
void drawPipes() {
    glColor3f(0.2f, 0.7f, 0.2f);
    
    for (int i = 0; i < pipeCount; i++) {
        // Top pipe
        drawRect(pipes[i].x, pipes[i].gapY + PIPE_GAP, PIPE_WIDTH, 3.0f);
        
        // Bottom pipe
        drawRect(pipes[i].x, -3.0f, PIPE_WIDTH, pipes[i].gapY);
    }
}

// Function to draw menu
void drawMenu() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw title background
    glColor3f(1.0f, 0.6f, 0.0f);
    drawRect(-0.6f, 0.5f, 1.2f, 0.25f);
    
    // Draw play button
    glColor3f(0.2f, 0.8f, 0.2f);
    drawRect(playButton.x, playButton.y, playButton.width, playButton.height);
    
    // Draw color selection buttons
    float colorX = -0.8f;
    BirdColor colors[] = {RED, BLUE, GREEN, YELLOW, PURPLE};
    for (int i = 0; i < 5; i++) {
        colorButtons[i] = {colorX + i * 0.35f, -0.6f, 0.3f, 0.15f};
        
        switch(colors[i]) {
            case RED:
                glColor3f(1.0f, 0.3f, 0.3f);
                break;
            case BLUE:
                glColor3f(0.3f, 0.6f, 1.0f);
                break;
            case GREEN:
                glColor3f(0.3f, 1.0f, 0.3f);
                break;
            case YELLOW:
                glColor3f(1.0f, 1.0f, 0.3f);
                break;
            case PURPLE:
                glColor3f(1.0f, 0.3f, 1.0f);
                break;
        }
        drawRect(colorButtons[i].x, colorButtons[i].y, colorButtons[i].width, colorButtons[i].height);
    }
}

// Function to draw game over screen
void drawGameOverScreen() {
    glColor3f(0.0f, 0.0f, 0.0f);
    drawRect(-0.5f, 0.3f, 1.0f, 0.2f);
    
    // Draw restart button
    glColor3f(0.2f, 0.8f, 0.2f);
    drawRect(playButton.x, playButton.y, playButton.width, playButton.height);
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
    
    pipeTimer += deltaTime;
    
    // Spawn new pipe
    if (pipeTimer >= PIPE_SPAWN_INTERVAL) {
        if (pipeCount < 20) {
            pipes[pipeCount].x = 1.2f;
            pipes[pipeCount].gapY = ((rand() % 50) / 100.0f) - 0.25f;
            pipes[pipeCount].scored = false;
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
            
            // Score increment when bird passes pipe
            if (!pipes[i].scored && pipes[i].x < bird.x) {
                score++;
                pipes[i].scored = true;
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

// Mouse button callback
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Convert screen coordinates to OpenGL coordinates
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Normalize to [-1, 1] range
        float normalizedX = (float)(xpos / WINDOW_WIDTH * 2.0 - 1.0);
        float normalizedY = (float)(1.0 - ypos / WINDOW_HEIGHT * 2.0);
        
        if (gameState == MENU) {
            // Check play button
            if (playButton.isClicked(normalizedX, normalizedY)) {
                gameState = PLAYING;
                bird.x = -0.7f;
                bird.y = 0.0f;
                bird.velocity = 0.0f;
                score = 0;
                pipeCount = 0;
                pipeTimer = 0.0f;
            }
            
            // Check color buttons
            for (int i = 0; i < 5; i++) {
                if (colorButtons[i].isClicked(normalizedX, normalizedY)) {
                    bird.color = static_cast<BirdColor>(i);
                }
            }
        } else if (gameState == GAME_OVER) {
            // Check play button to restart
            if (playButton.isClicked(normalizedX, normalizedY)) {
                gameState = MENU;
            }
        }
    }
}

// Keyboard callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_SPACE:
                if (gameState == MENU) {
                    gameState = PLAYING;
                    bird.x = -0.7f;
                    bird.y = 0.0f;
                    bird.velocity = 0.0f;
                    score = 0;
                    pipeCount = 0;
                    pipeTimer = 0.0f;
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
        // Draw menu with title
        glColor3f(1.0f, 0.6f, 0.0f);
        drawRect(-0.6f, 0.5f, 1.2f, 0.25f);
        
        // Draw play button
        glColor3f(0.2f, 0.8f, 0.2f);
        drawRect(playButton.x, playButton.y, playButton.width, playButton.height);
        
        // Draw color selection buttons
        BirdColor colors[] = {RED, BLUE, GREEN, YELLOW, PURPLE};
        for (int i = 0; i < 5; i++) {
            colorButtons[i] = {-0.8f + i * 0.35f, -0.6f, 0.3f, 0.15f};
            
            switch(colors[i]) {
                case RED:
                    glColor3f(1.0f, 0.3f, 0.3f);
                    break;
                case BLUE:
                    glColor3f(0.3f, 0.6f, 1.0f);
                    break;
                case GREEN:
                    glColor3f(0.3f, 1.0f, 0.3f);
                    break;
                case YELLOW:
                    glColor3f(1.0f, 1.0f, 0.3f);
                    break;
                case PURPLE:
                    glColor3f(1.0f, 0.3f, 1.0f);
                    break;
            }
            drawRect(colorButtons[i].x, colorButtons[i].y, colorButtons[i].width, colorButtons[i].height);
        }
    } else if (gameState == PLAYING) {
        // Draw background
        glColor3f(0.7f, 0.9f, 1.0f);
        
        // Draw ground
        glColor3f(0.6f, 0.4f, 0.2f);
        drawRect(-1.0f, -0.95f, 2.0f, 0.05f);
        
        // Draw pipes
        drawPipes();
        
        // Draw bird
        drawBird(bird);
        
        // Draw score on screen (simple version)
        glColor3f(0.0f, 0.0f, 0.0f);
        // Score will be printed to console
        
    } else if (gameState == GAME_OVER) {
        // Draw background
        glColor3f(0.7f, 0.9f, 1.0f);
        
        // Draw ground
        glColor3f(0.6f, 0.4f, 0.2f);
        drawRect(-1.0f, -0.95f, 2.0f, 0.05f);
        
        // Draw pipes (frozen)
        drawPipes();
        
        // Draw bird
        drawBird(bird);
        
        // Draw game over screen
        glColor3f(0.0f, 0.0f, 0.0f);
        drawRect(-0.5f, 0.3f, 1.0f, 0.2f);
        
        // Draw restart button
        glColor3f(0.2f, 0.8f, 0.2f);
        drawRect(playButton.x, playButton.y, playButton.width, playButton.height);
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    if (!glfwInit())
        return -1;
    
    GLFWwindow* window = glfwCreateWindow(
        static_cast<int>(WINDOW_WIDTH), 
        static_cast<int>(WINDOW_HEIGHT), 
        "Flappy Bird - Click Play | Select Color | Space to Jump | ESC to Exit", 
        NULL, NULL
    );
    
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    
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
    std::cout << "  CLICK - Select color or play button (in menu)\n";
    std::cout << "  SPACE - Jump (while playing) / Play (in menu)\n";
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
        
        // Print score to console
        if (gameState == PLAYING) {
            std::cout << "\rScore: " << score << " | High Score: " << highScore << "     ";
            std::cout.flush();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    std::cout << "\n\nFinal Score: " << score << "\n";
    std::cout << "High Score: " << highScore << "\n";
    
    glfwTerminate();
    return 0;
}
