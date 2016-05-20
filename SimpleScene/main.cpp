#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Geometry.h"

#define GLUT_KEY_ESCAPE 27
#define GLUT_KEY_W 119
#define GLUT_KEY_A 97
#define GLUT_KEY_S 115
#define GLUT_KEY_D 100

void one_time_initialize();
void display_callback();
void idle_callback();
void key_press_callback(unsigned char key, int x, int y);
void key_up_callback(unsigned char key, int x, int y);
void mouse_callback(int x, int y);
void scroll_callback(int wheel, int direction, int x, int y);
void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
// Window title
const GLchar* TITLE = "SimpleScene";

Shader lightingShader;
Shader lampShader;
Shader flatShader;
GLuint VBO, containerVAO, lightVAO;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLint lastX = WIDTH / 2;
GLint lastY = HEIGHT / 2;
bool keys[1024];

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2,
                           (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);
    glutCreateWindow(TITLE);
    if (glutGet(GLUT_VERSION) == 30000)
    {
        std::cout << "Status: Using FreeGLUT 3.0.0" << std::endl;
    }

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    one_time_initialize();
    glutDisplayFunc(display_callback);
    glutIdleFunc(idle_callback);
    glutKeyboardFunc(key_press_callback);
    glutKeyboardUpFunc(key_up_callback);
    glutPassiveMotionFunc(mouse_callback);
    glutMouseWheelFunc(scroll_callback);

    glutMainLoop();

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &containerVAO);
    glDeleteBuffers(1, &lightVAO);

    return 0;
}

void one_time_initialize()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    lightingShader.Setup("lighting");
    lampShader.Setup("lamp");
    flatShader.Setup("flat_shading");

    glGenVertexArrays(1, &containerVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(containerVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr); // Note that we skip over the normal vectors
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void display_callback()
{
    // Clear the colorbuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use cooresponding shader when setting uniforms/drawing objects
    lightingShader.Use();
    GLint objectColorLoc = glGetUniformLocation(lightingShader(), "objectColor");
    GLint lightColorLoc = glGetUniformLocation(lightingShader(), "lightColor");
    GLint lightPosLoc = glGetUniformLocation(lightingShader(), "lightPos");
    GLint viewPosLoc = glGetUniformLocation(lightingShader(), "viewPos");
    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

    // Create camera transformations
    glm::mat4 view;
    view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f);
    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(lightingShader(), "model");
    GLint viewLoc = glGetUniformLocation(lightingShader(), "view");
    GLint projLoc = glGetUniformLocation(lightingShader(), "projection");
    // Pass the matrices to the shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the container (using container's vertex attributes)
    glBindVertexArray(containerVAO);
    glm::mat4 model;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Flat shader test and glutSolidTeapot
    flatShader.Use();
    objectColorLoc = glGetUniformLocation(flatShader(), "objectColor");
    lightColorLoc = glGetUniformLocation(flatShader(), "lightColor");
    lightPosLoc = glGetUniformLocation(flatShader(), "lightPos");
    viewPosLoc = glGetUniformLocation(flatShader(), "viewPos");
    modelLoc = glGetUniformLocation(flatShader(), "model");
    viewLoc = glGetUniformLocation(flatShader(), "view");
    projLoc = glGetUniformLocation(flatShader(), "projection");
    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glutSetVertexAttribCoord3(0);
    glutSetVertexAttribNormal(1);
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.6f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glutSolidTeapot(0.2f);
    glutSetVertexAttribCoord3(-1);
    glutSetVertexAttribNormal(-1);

    // Also draw the lamp object, again binding the appropriate shader
    lampShader.Use();
    // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
    modelLoc = glGetUniformLocation(lampShader(), "model");
    viewLoc = glGetUniformLocation(lampShader(), "view");
    projLoc = glGetUniformLocation(lampShader(), "projection");
    // Set matrices
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Draw the light object (using light's vertex attributes)
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glutSwapBuffers();
}

void idle_callback()
{
    // Calculate deltatime of current frame
    GLfloat currentFrame = static_cast<GLfloat>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    do_movement();

    glutPostRedisplay();
}

void key_press_callback(unsigned char key, int x, int y)
{
    if (key == GLUT_KEY_ESCAPE)
        return;
    if (key >= 0 && key < 1024)
    {
        keys[key] = true;
    }
}

void key_up_callback(unsigned char key, int x, int y)
{
    if (key == GLUT_KEY_ESCAPE)
        glutLeaveMainLoop();
    if (key >= 0 && key < 1024)
    {
        keys[key] = false;
    }
}

void do_movement()
{
    // Camera controls
    if (keys[GLUT_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLUT_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLUT_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLUT_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

bool firstMouse = true;

void mouse_callback(int x, int y)
{
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    GLint xoffset = x - lastX;
    GLint yoffset = lastY - y; // Reversed since y-coordinates go from bottom to left

    lastX = x;
    lastY = y;

    camera.ProcessMouseMovement(static_cast<GLfloat>(xoffset), static_cast<GLfloat>(yoffset));
}

void scroll_callback(int wheel, int direction, int x, int y)
{
    camera.ProcessMouseScroll(static_cast<GLfloat>(y));
}
