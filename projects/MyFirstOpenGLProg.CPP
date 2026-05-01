#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

float wheelAngle = 0.0; // wheel rotation angle
float carX = 0.0f; // car start position (center)

void drawCircle(float cx, float cy, float r, int segments)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glRotatef(wheelAngle, 0, 0, 1);
    
    // Draw the wheel circle
    glColor3f(0.2f, 0.8f, 0.4f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * 3.1415926f * i / segments;
        float x = r * cos(angle);
        float y = r * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
    
    // Draw wheel spokes (so rotation is visible)
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 4; i++) {
        float angle = 2.0f * 3.1415926f * i / 4;
        float x = r * cos(angle);
        float y = r * sin(angle);
        glVertex2f(0, 0);
        glVertex2f(x, y);
    }
    glEnd();
    
    glPopMatrix();
}

void drawCarBody()
{
    glColor3f(1, 0, 1);
    glBegin(GL_POLYGON);
    glVertex2f(-0.5, -0.25);
    glVertex2f(0.5, -0.25);
    glVertex2f(0.5, 0.125);
    glVertex2f(0.3, 0.4);
    glVertex2f(-0.125, 0.4);
    glVertex2f(-0.3, 0.125);
    glVertex2f(-0.5, 0.125);
    glEnd();
}

void drawCar()
{
    drawCarBody();
    drawCircle(0.3f, -0.25f, 0.125f, 36);
    drawCircle(-0.3f, -0.25f, 0.125f, 36);
}

int main()
{
    if (!glfwInit())
        return -1;
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Car Animation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // Set up the viewport and projection
    glViewport(0, 0, 1280, 720);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.5, 1.5, -0.8, 0.8, -1, 1);  // Set coordinate system
    glMatrixMode(GL_MODELVIEW);
    
    // Enable blending for smooth edges (optional)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float carX =0.0f; // car start position (left side)
    
    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
        glClear(GL_COLOR_BUFFER_BIT);
        
        glLoadIdentity();  // Reset transformation
        
        // Update wheel rotation
        wheelAngle += 0.50f;  // Rotate wheels
        
        // Move the whole car
        glPushMatrix();
        glTranslatef(carX, 0, 0);
        drawCar();
        glPopMatrix();
        
        // Update motion
        carX -= 0.005f;  // Move right
        
        // Reset car when it leaves screen
        if (carX < 1.5f)
            carX = 1.5f;
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}