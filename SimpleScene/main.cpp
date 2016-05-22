// Standard headers
#include <iostream>
#include <vector>
#include <functional>

// Third party headers
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Self defined headers
#include "Shader.h"
#include "Camera.h"
#include "PointLight.h"
#include "LightProperties.h"
#include "OrnamentProperties.h"
#include "SpotLight.h"
#include "Table.h"
#include "Chair.h"
#include "Plane.h"
#include "ObjectProperties.h"

// Constants
#define GLUT_KEY_ESCAPE 27
#define GLUT_KEY_PAGE_UP_CUSTOM 1000
#define GLUT_KEY_PAGE_DOWN_CUSTOM 1001
#define GLUT_KEY_HOME_CUSTOM 1002
#define GLUT_KEY_END_CUSTOM 1003
#define GLUT_KEY_UP_CUSTOM 1004
#define GLUT_KEY_DOWN_CUSTOM 1005
#define GLUT_KEY_LEFT_CUSTOM 1006
#define GLUT_KEY_RIGHT_CUSTOM 1007

//const GLuint WIDTH = 800, HEIGHT = 600;
const GLchar* TITLE = "SimpleScene";
const int NUM_OF_POINT_LIGHTS = 2;
const int NUM_OF_DISCO_LIGHTS = 4;
const int NUM_OF_ORNAMENTS = 8;

// Stores the state of a window
struct WindowInfo
{
    // Shaders
    Shader smoothShader;
    Shader flatShader;
    Shader lampShader;
    //std::function<void()> useCurrentShader;

    // Camera
    glm::vec3 cameraStartPosition;
    Camera camera;

    // Lights
    PointLight pointLights[NUM_OF_POINT_LIGHTS];
    SpotLight spotLight;
    SpotLight discoLights[NUM_OF_DISCO_LIGHTS];
    GLfloat spotLightSwingSpeed = 1.0f;
    GLfloat discoLightSwingSpeed = 2.0f;

    // Objects, VBOs & VAOs
    std::vector<std::function<void()>> ornaments;
    GLuint chairVBOPos, chairVBONormals, chairVAO;
    GLuint tableVBOPos, tableVBONormals, tableVAO;
    GLuint planeVBOPos, planeVBONormals, planeVAO;

    // OpenGL variables
    bool useSmoothShading = true;
    bool useColorTracking = false;
    bool useBrightAmbientLight = false;
    bool useBackfaceCulling = true;
    bool cullFrontFace = false;
    bool useDepthTesting = true;

    // User input
    bool keys[1024];

    // MVP matrices
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    ~WindowInfo()
    {
        glDeleteBuffers(1, &chairVBOPos);
        glDeleteBuffers(1, &chairVBONormals);
        glDeleteBuffers(1, &tableVBOPos);
        glDeleteBuffers(1, &tableVBONormals);
        glDeleteBuffers(1, &planeVBOPos);
        glDeleteBuffers(1, &planeVBONormals);

        glDeleteVertexArrays(1, &chairVAO);
        glDeleteVertexArrays(1, &tableVAO);
        glDeleteVertexArrays(1, &planeVAO);
    }
};

void initialize(int windowId);
void display(int windowId);
void handleKeyPress(int windowId, unsigned char key, int x, int y);
void handleKeyUp(int windowId, unsigned char key, int x, int y);
void handleSpecialPress(int windowId, int key, int x, int y);
void handleSpecialUp(int windowid, int key, int x, int y);
void handleSmoothInput(int windowId);

void mainWindowDisplayCallback();
void leftWindowDisplayCallback();
void rightWindowDisplayCallback();
void instructionDisplayCallback();
void idleCallback();
void leftWindowKeyPressCallback(unsigned char key, int x, int y);
void leftWindowKeyUpCallback(unsigned char key, int x, int y);
void leftWindowSpecialPressCallback(int key, int x, int y);
void leftWindowSpecialUpCallback(int key, int x, int y);
void rightWindowKeyPressCallback(unsigned char key, int x, int y);
void rightWindowKeyUpCallback(unsigned char key, int x, int y);
void rightWindowSpecialPressCallback(int key, int x, int y);
void rightWindowSpecialUpCallback(int key, int x, int y);

void renderText(const float &x, const float &y, void *font, const char *const str);

// Window ids
int mainWindow;
int leftWindow;
int rightWindow;
int instructionWindow;

// Two subwindow states
WindowInfo window[2];

// Deltatime
GLfloat deltaTime = 0.00f; // Time between current frame and last frame
GLfloat lastFrame = 0.00f; // Time of last frame

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    if (glutGet(GLUT_VERSION) == 30000)
    {
        std::cout << "Status: Using FreeGLUT 3.0.0" << std::endl;
    }

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA | GLUT_MULTISAMPLE);
    glutInitWindowSize(1200, 650);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 1200) / 2,
                           (glutGet(GLUT_SCREEN_HEIGHT) - 650) / 2);
    mainWindow = glutCreateWindow(TITLE);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    glutDisplayFunc(mainWindowDisplayCallback);
    glutIdleFunc(idleCallback);

    leftWindow = glutCreateSubWindow(mainWindow, 0, 0, 600, 450);
    initialize(0);
    glutDisplayFunc(leftWindowDisplayCallback);
    glutKeyboardFunc(leftWindowKeyPressCallback);
    glutKeyboardUpFunc(leftWindowKeyUpCallback);
    glutSpecialFunc(leftWindowSpecialPressCallback);
    glutSpecialUpFunc(leftWindowSpecialUpCallback);

    rightWindow = glutCreateSubWindow(mainWindow, 600, 0, 600, 450);
    initialize(1);
    glutDisplayFunc(rightWindowDisplayCallback);
    glutKeyboardFunc(rightWindowKeyPressCallback);
    glutKeyboardUpFunc(rightWindowKeyUpCallback);
    glutSpecialFunc(rightWindowSpecialPressCallback);
    glutSpecialUpFunc(rightWindowSpecialUpCallback);

    instructionWindow = glutCreateSubWindow(mainWindow, 0, 450, 1200, 200);
    glutDisplayFunc(instructionDisplayCallback);

    glutMainLoop();

    return 0;
}

void initialize(int windowId)
{
    auto width = glutGet(GLUT_WINDOW_WIDTH);
    auto height = glutGet(GLUT_WINDOW_HEIGHT);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

    window[windowId].smoothShader.Setup("smooth_shader");
    window[windowId].flatShader.Setup("flat_shader");
    window[windowId].lampShader.Setup("lamp");
    //window[windowId].useCurrentShader = std::bind(&Shader::Use, window[windowId].smoothShader);

    window[windowId].cameraStartPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    window[windowId].camera.SetupCamera(window[windowId].cameraStartPosition);

    for (auto i = 0; i < NUM_OF_POINT_LIGHTS; ++i)
    {
        window[windowId].pointLights[i].id = i;
        window[windowId].pointLights[i].position = glm::vec3(lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
        window[windowId].pointLights[i].material = lightMaterials[i];
        window[windowId].pointLights[i].active = lightMaterials[i];
        window[windowId].pointLights[i].constant = lightConstant;
        window[windowId].pointLights[i].linear = lightAttenuation[1][0];
        window[windowId].pointLights[i].quadratic = lightAttenuation[1][1];
    }

    window[windowId].spotLight.id = 0;
    window[windowId].spotLight.position = spotLightPos;
    window[windowId].spotLight.direction = glm::normalize(spotLightDir);
    window[windowId].spotLight.material = spotLightMaterial;
    window[windowId].spotLight.active = spotLightMaterial;
    window[windowId].spotLight.constant = lightConstant;
    window[windowId].spotLight.linear = spotLightAttenuation[0];
    window[windowId].spotLight.quadratic = spotLightAttenuation[1];
    window[windowId].spotLight.cutOff = spotLightCutOff;
    window[windowId].spotLight.outerCutOff = spotLightOuterCutOff;

    for (auto i = 0; i < NUM_OF_DISCO_LIGHTS; ++i)
    {
        window[windowId].discoLights[i].id = i;
        window[windowId].discoLights[i].position = discoLightsPos;
        window[windowId].discoLights[i].direction = discoLightsDir;
        window[windowId].discoLights[i].material = discoLightMaterials[i];
        window[windowId].discoLights[i].active = discoLightMaterials[i];
        window[windowId].discoLights[i].constant = lightConstant;
        window[windowId].discoLights[i].linear = discoLightsAttenuation[0];
        window[windowId].discoLights[i].quadratic = discoLightsAttenuation[1];
        window[windowId].discoLights[i].cutOff = discoLightsCutOff;
        window[windowId].discoLights[i].outerCutOff = discoLightsOuterCutOff;
    }

    window[windowId].ornaments.push_back([] { glutSolidTeapot(0.15f); });
    window[windowId].ornaments.push_back([] { glutSolidSphere(0.15f, 100, 100); });
    window[windowId].ornaments.push_back([] { glutSolidCone(0.15f, 0.5f, 100, 100); });
    window[windowId].ornaments.push_back([] { glutSolidTorus(0.1f, 0.2f, 100, 100); });
    window[windowId].ornaments.push_back([] { glutSolidDodecahedron(); });
    window[windowId].ornaments.push_back([] { glutSolidOctahedron(); });
    window[windowId].ornaments.push_back([] { glutSolidTetrahedron(); });
    window[windowId].ornaments.push_back([] { glutSolidIcosahedron(); });

    glGenVertexArrays(1, &window[windowId].chairVAO);
    glGenBuffers(1, &window[windowId].chairVBOPos);
    glGenBuffers(1, &window[windowId].chairVBONormals);
    glBindVertexArray(window[windowId].chairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, window[windowId].chairVBOPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chairPositions), chairPositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, window[windowId].chairVBONormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chairNormals), chairNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindVertexArray(0);

    glGenVertexArrays(1, &window[windowId].tableVAO);
    glGenBuffers(1, &window[windowId].tableVBOPos);
    glGenBuffers(1, &window[windowId].tableVBONormals);
    glBindVertexArray(window[windowId].tableVAO);
    glBindBuffer(GL_ARRAY_BUFFER, window[windowId].tableVBOPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tablePositions), tablePositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, window[windowId].tableVBONormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tableNormals), tableNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindVertexArray(0);

    glGenVertexArrays(1, &window[windowId].planeVAO);
    glGenBuffers(1, &window[windowId].planeVBOPos);
    glGenBuffers(1, &window[windowId].planeVBONormals);
    glBindVertexArray(window[windowId].planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, window[windowId].planeVBOPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planePositions), planePositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, window[windowId].planeVBONormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeNormals), planeNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindVertexArray(0);
}

void display(int windowId)
{
    auto width = glutGet(GLUT_WINDOW_WIDTH);
    auto height = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (window[windowId].useSmoothShading)
    {
        window[windowId].smoothShader.Use();
    } else
    {
        window[windowId].flatShader.Use();
    }

    //window[windowId].useCurrentShader();
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&program));

    if (window[windowId].useBackfaceCulling)
    {
        glEnable(GL_CULL_FACE);
    } else
    {
        glDisable(GL_CULL_FACE);
    }

    if (window[windowId].cullFrontFace)
    {
        glCullFace(GL_FRONT);
    } else
    {
        glCullFace(GL_BACK);
    }

    if (window[windowId].useDepthTesting)
    {
        glEnable(GL_DEPTH_TEST);
    } else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glUniform3f(glGetUniformLocation(program, "viewPos"), window[windowId].camera.Position.x, window[windowId].camera.Position.y, window[windowId].camera.Position.z);
    for (auto i = 0; i < NUM_OF_POINT_LIGHTS; ++i)
    {
        window[windowId].pointLights[i].Render();
    }

    //TODO Refactor this
    window[windowId].spotLight.direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].spotLightSwingSpeed);
    window[windowId].spotLight.direction = glm::normalize(window[windowId].spotLight.direction);
    window[windowId].spotLight.Render();

    window[windowId].discoLights[0].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].discoLightSwingSpeed);
    window[windowId].discoLights[0].direction = glm::normalize(window[windowId].discoLights[0].direction);

    window[windowId].discoLights[1].direction.z = cos((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].discoLightSwingSpeed);
    window[windowId].discoLights[1].direction = glm::normalize(window[windowId].discoLights[1].direction);

    //discoLights[2].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
    window[windowId].discoLights[2].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].discoLightSwingSpeed);
    window[windowId].discoLights[2].direction.z = cos((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].discoLightSwingSpeed);
    window[windowId].discoLights[2].direction = glm::normalize(window[windowId].discoLights[2].direction);

    window[windowId].discoLights[3].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].discoLightSwingSpeed);
    window[windowId].discoLights[3].direction.z = cos((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * window[windowId].discoLightSwingSpeed);
    window[windowId].discoLights[3].direction = glm::normalize(window[windowId].discoLights[3].direction);

    for (auto i = 0; i < NUM_OF_DISCO_LIGHTS; ++i)
    {
        auto uniformVariable = "discoLights[" + std::to_string(i) + "].";
        glUniform3f(glGetUniformLocation(program, (uniformVariable + "position").c_str()), window[windowId].discoLights[i].position.x, window[windowId].discoLights[i].position.y, window[windowId].discoLights[i].position.z);
        glUniform3f(glGetUniformLocation(program, (uniformVariable + "direction").c_str()), window[windowId].discoLights[i].direction.x, window[windowId].discoLights[i].direction.y, window[windowId].discoLights[i].direction.z);
        glUniform3f(glGetUniformLocation(program, (uniformVariable + "ambient").c_str()), window[windowId].discoLights[i].active.ambient.x, window[windowId].discoLights[i].active.ambient.y, window[windowId].discoLights[i].active.ambient.z);
        glUniform3f(glGetUniformLocation(program, (uniformVariable + "diffuse").c_str()), window[windowId].discoLights[i].active.diffuse.x, window[windowId].discoLights[i].active.diffuse.y, window[windowId].discoLights[i].active.diffuse.z);
        glUniform3f(glGetUniformLocation(program, (uniformVariable + "specular").c_str()), window[windowId].discoLights[i].active.specular.x, window[windowId].discoLights[i].active.specular.y, window[windowId].discoLights[i].active.specular.z);
        glUniform1f(glGetUniformLocation(program, (uniformVariable + "constant").c_str()), window[windowId].discoLights[i].constant);
        glUniform1f(glGetUniformLocation(program, (uniformVariable + "linear").c_str()), window[windowId].discoLights[i].linear);
        glUniform1f(glGetUniformLocation(program, (uniformVariable + "quadratic").c_str()), window[windowId].discoLights[i].quadratic);
        glUniform1f(glGetUniformLocation(program, (uniformVariable + "cutOff").c_str()), glm::cos(glm::radians(window[windowId].discoLights[i].cutOff)));
        glUniform1f(glGetUniformLocation(program, (uniformVariable + "outerCutOff").c_str()), glm::cos(glm::radians(window[windowId].discoLights[i].outerCutOff)));
    }


    if (windowId == 1)
    //if (window[windowId].projectionMode == ProjectionMode::PERSPECTIVE)
        window[windowId].projection = glm::perspective(glm::radians(window[windowId].camera.Zoom),
            static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 0.1f, 100.0f);
    else
        window[windowId].projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    
    // Create camera transformations
    window[windowId].view = window[windowId].camera.GetViewMatrix();

    GLint modelLoc = glGetUniformLocation(program, "model");
    GLint viewLoc = glGetUniformLocation(program, "view");
    GLint projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].projection));

    glutSetVertexAttribCoord3(0);
    glutSetVertexAttribNormal(1);
    for (auto i = 0; i < NUM_OF_ORNAMENTS; ++i)
    {
        if (window[windowId].useColorTracking)
        {
            glUniform3f(glGetUniformLocation(program, "material.ambient"),
                ornamentColors[i].x,
                ornamentColors[i].y,
                ornamentColors[i].z);
            glUniform3f(glGetUniformLocation(program, "material.diffuse"),
                ornamentColors[i].x,
                ornamentColors[i].y,
                ornamentColors[i].z);
            glUniform3f(glGetUniformLocation(program, "material.specular"),
                ornamentColors[i].x,
                ornamentColors[i].y,
                ornamentColors[i].z);
            glUniform1f(glGetUniformLocation(program, "material.shininess"),
                ornamentMaterials[i].shininess);
        }
        else
        {
            glUniform3f(glGetUniformLocation(program, "material.ambient"),
                ornamentMaterials[i].ambient.x,
                ornamentMaterials[i].ambient.y,
                ornamentMaterials[i].ambient.z);
            glUniform3f(glGetUniformLocation(program, "material.diffuse"),
                ornamentMaterials[i].diffuse.x,
                ornamentMaterials[i].diffuse.y,
                ornamentMaterials[i].diffuse.z);
            glUniform3f(glGetUniformLocation(program, "material.specular"),
                ornamentMaterials[i].specular.x,
                ornamentMaterials[i].specular.y,
                ornamentMaterials[i].specular.z);
            glUniform1f(glGetUniformLocation(program, "material.shininess"),
                ornamentMaterials[i].shininess);
        }

        window[windowId].model = glm::mat4();
        window[windowId].model = glm::translate(window[windowId].model, ornamentPositions[i]);
        window[windowId].model = glm::rotate(window[windowId].model, glm::radians(ornamentRotations[i].x), glm::vec3(1.0f, 0.0f, 0.0f));
        window[windowId].model = glm::rotate(window[windowId].model, glm::radians(ornamentRotations[i].y), glm::vec3(0.0f, 1.0f, 0.0f));
        window[windowId].model = glm::rotate(window[windowId].model, glm::radians(ornamentRotations[i].z), glm::vec3(0.0f, 0.0f, 1.0f));
        window[windowId].model = glm::scale(window[windowId].model, ornamentScales[i]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
        window[windowId].ornaments[i]();
    }
    glutSetVertexAttribCoord3(-1);
    glutSetVertexAttribNormal(-1);


    glUniform3f(glGetUniformLocation(program, "material.ambient"),
        planeMaterial.ambient.x,
        planeMaterial.ambient.y,
        planeMaterial.ambient.z);
    glUniform3f(glGetUniformLocation(program, "material.diffuse"),
        planeMaterial.diffuse.x,
        planeMaterial.diffuse.y,
        planeMaterial.diffuse.z);
    glUniform3f(glGetUniformLocation(program, "material.specular"),
        planeMaterial.specular.x,
        planeMaterial.specular.y,
        planeMaterial.specular.z);
    glUniform1f(glGetUniformLocation(program, "material.shininess"),
        planeMaterial.shininess);
    glBindVertexArray(window[windowId].planeVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::scale(window[windowId].model, glm::vec3(2.913f, 1.0f, 2.913f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
    glBindVertexArray(0);

    glBindVertexArray(window[windowId].planeVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(0.0f, 2.28f, -2.28f));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    window[windowId].model = glm::scale(window[windowId].model, glm::vec3(2.913f, 1.0f, 2.913f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
    glBindVertexArray(0);

    glBindVertexArray(window[windowId].planeVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(0.0f, 2.28f, 2.28f));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    window[windowId].model = glm::scale(window[windowId].model, glm::vec3(2.913f, 1.0f, 2.913f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
    glBindVertexArray(0);

    glBindVertexArray(window[windowId].planeVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(0.0f, 4.56f, 0.0f));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    window[windowId].model = glm::scale(window[windowId].model, glm::vec3(2.913f, 1.0f, 2.913f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
    glBindVertexArray(0);

    glBindVertexArray(window[windowId].planeVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(2.28f, 2.28f, 0.0f));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    window[windowId].model = glm::scale(window[windowId].model, glm::vec3(2.913f, 1.0f, 2.913f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
    glBindVertexArray(0);

    glBindVertexArray(window[windowId].planeVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(-2.28f, 2.28f, 0.0f));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    window[windowId].model = glm::scale(window[windowId].model, glm::vec3(2.913f, 1.0f, 2.913f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
    glBindVertexArray(0);


    glUniform3f(glGetUniformLocation(program, "material.ambient"),
        tableMaterial.ambient.x,
        tableMaterial.ambient.y,
        tableMaterial.ambient.z);
    glUniform3f(glGetUniformLocation(program, "material.diffuse"),
        tableMaterial.diffuse.x,
        tableMaterial.diffuse.y,
        tableMaterial.diffuse.z);
    glUniform3f(glGetUniformLocation(program, "material.specular"),
        tableMaterial.specular.x,
        tableMaterial.specular.y,
        tableMaterial.specular.z);
    glUniform1f(glGetUniformLocation(program, "material.shininess"),
        tableMaterial.shininess);
    glBindVertexArray(window[windowId].tableVAO);
    window[windowId].model = glm::mat4();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, tableVertices);
    glBindVertexArray(0);


    glUniform3f(glGetUniformLocation(program, "material.ambient"),
        chairMaterial.ambient.x,
        chairMaterial.ambient.y,
        chairMaterial.ambient.z);
    glUniform3f(glGetUniformLocation(program, "material.diffuse"),
        chairMaterial.diffuse.x,
        chairMaterial.diffuse.y,
        chairMaterial.diffuse.z);
    glUniform3f(glGetUniformLocation(program, "material.specular"),
        chairMaterial.specular.x,
        chairMaterial.specular.y,
        chairMaterial.specular.z);
    glUniform1f(glGetUniformLocation(program, "material.shininess"),
        chairMaterial.shininess);
    glBindVertexArray(window[windowId].chairVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(1.2482f, -0.34394f, 0.0f));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(28.01f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, chairVertices);
    glBindVertexArray(0);

    glBindVertexArray(window[windowId].chairVAO);
    window[windowId].model = glm::mat4();
    window[windowId].model = glm::translate(window[windowId].model, glm::vec3(-0.12125f, -0.34394, -1.34712));
    window[windowId].model = glm::rotate(window[windowId].model, glm::radians(130.738f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
    glDrawArrays(GL_TRIANGLES, 0, chairVertices);
    glBindVertexArray(0);

    window[windowId].lampShader.Use();
    modelLoc = glGetUniformLocation(window[windowId].lampShader(), "model");
    viewLoc = glGetUniformLocation(window[windowId].lampShader(), "view");
    projLoc = glGetUniformLocation(window[windowId].lampShader(), "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].projection));
    glutSetVertexAttribCoord3(0);
    glutSetVertexAttribNormal(1);
    for (auto i = 0; i < NUM_OF_POINT_LIGHTS; ++i)
    {
        window[windowId].model = glm::mat4();
        window[windowId].model = glm::translate(window[windowId].model, lightPositions[i]);
        window[windowId].model = glm::scale(window[windowId].model, glm::vec3(0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(window[windowId].model));
        glutSolidSphere(1.0f, 100, 100);
    }
    glutSetVertexAttribCoord3(-1);
    glutSetVertexAttribNormal(-1);

    glutSwapBuffers();
}

void handleKeyPress(int windowId, unsigned char key, int x, int y)
{
    if (key == GLUT_KEY_ESCAPE)
        return;

    if (key == 'z')
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        return;
    }

    if (key == 'x')
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        return;
    }

    if (key == 'c')
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        return;
    }

    if (key == 'v')
    {
        window[windowId].useSmoothShading = !window[windowId].useSmoothShading;
        return;
    }

    if (key == 'b')
    {
        window[windowId].useColorTracking = !window[windowId].useColorTracking;
        return;
    }

    if (key == 'n')
    {
        window[windowId].useBrightAmbientLight = !window[windowId].useBrightAmbientLight;
        return;
    }

    if (key == 'm')
    {
        window[windowId].useBackfaceCulling = !window[windowId].useBackfaceCulling;
        return;
    }

    if (key == ',')
    {
        window[windowId].cullFrontFace = !window[windowId].cullFrontFace;
        return;
    }

    if (key == '.')
    {
        window[windowId].useDepthTesting = !window[windowId].useDepthTesting;
        return;
    }

    if (key == '1')
    {
        window[windowId].pointLights[0].Toggle();
        return;
    }

    if (key == '2')
    {
        window[windowId].pointLights[1].Toggle();
        return;
    }

    if (key == '3')
    {
        window[windowId].spotLight.Toggle();
        return;
    }

    if (key == '4')
    {
        return;
    }

    if (key == '[')
    {
        window[windowId].spotLightSwingSpeed += 0.1f;
        return;
    }

    if (key == ']')
    {
        window[windowId].spotLightSwingSpeed -= 0.1f;
        if (window[windowId].spotLightSwingSpeed < 1.0f)
        {
            window[windowId].spotLightSwingSpeed = 1.0f;
        }
        return;
    }

    if (key >= 0 && key < 1024)
    {
        window[windowId].keys[key] = true;
    }
}

void handleKeyUp(int windowId, unsigned char key, int x, int y)
{
    if (key == GLUT_KEY_ESCAPE)
        glutLeaveMainLoop();
    if (key >= 0 && key < 1024)
    {
        window[windowId].keys[key] = false;
    }
}

void handleSpecialPress(int windowId, int key, int x, int y)
{
    if (key == GLUT_KEY_PAGE_UP)
        window[windowId].keys[GLUT_KEY_PAGE_UP_CUSTOM] = true;
    else if (key == GLUT_KEY_PAGE_DOWN)
        window[windowId].keys[GLUT_KEY_PAGE_DOWN_CUSTOM] = true;

    if (key == GLUT_KEY_HOME)
        window[windowId].keys[GLUT_KEY_HOME_CUSTOM] = true;
    else if (key == GLUT_KEY_END)
        window[windowId].keys[GLUT_KEY_END_CUSTOM] = true;

    if (key == GLUT_KEY_UP)
        window[windowId].keys[GLUT_KEY_UP_CUSTOM] = true;
    else if (key == GLUT_KEY_DOWN)
        window[windowId].keys[GLUT_KEY_DOWN_CUSTOM] = true;
    else if (key == GLUT_KEY_LEFT)
        window[windowId].keys[GLUT_KEY_LEFT_CUSTOM] = true;
    else if (key == GLUT_KEY_RIGHT)
        window[windowId].keys[GLUT_KEY_RIGHT_CUSTOM] = true;
}

void handleSpecialUp(int windowId, int key, int x, int y)
{
    if (key == GLUT_KEY_PAGE_UP)
        window[windowId].keys[GLUT_KEY_PAGE_UP_CUSTOM] = false;
    else if (key == GLUT_KEY_PAGE_DOWN)
        window[windowId].keys[GLUT_KEY_PAGE_DOWN_CUSTOM] = false;

    if (key == GLUT_KEY_HOME)
        window[windowId].keys[GLUT_KEY_HOME_CUSTOM] = false;
    else if (key == GLUT_KEY_END)
        window[windowId].keys[GLUT_KEY_END_CUSTOM] = false;

    if (key == GLUT_KEY_UP)
        window[windowId].keys[GLUT_KEY_UP_CUSTOM] = false;
    else if (key == GLUT_KEY_DOWN)
        window[windowId].keys[GLUT_KEY_DOWN_CUSTOM] = false;
    else if (key == GLUT_KEY_LEFT)
        window[windowId].keys[GLUT_KEY_LEFT_CUSTOM] = false;
    else if (key == GLUT_KEY_RIGHT)
        window[windowId].keys[GLUT_KEY_RIGHT_CUSTOM] = false;
}

void handleSmoothInput(int windowId)
{
    // Camera controls
    if (window[windowId].keys['w'])
    {
        window[windowId].camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (window[windowId].keys['s'])
    {
        window[windowId].camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (window[windowId].keys['a'])
    {
        window[windowId].camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (window[windowId].keys['d'])
    {
        window[windowId].camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (window[windowId].keys[GLUT_KEY_PAGE_UP_CUSTOM])
    {
        window[windowId].camera.ProcessKeyboard(UP, deltaTime);
    }

    if (window[windowId].keys[GLUT_KEY_PAGE_DOWN_CUSTOM])
    {
        window[windowId].camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (window[windowId].keys[GLUT_KEY_HOME_CUSTOM]) {
        window[windowId].camera.SetZoom(50.0f * deltaTime);
    }

    if (window[windowId].keys[GLUT_KEY_END_CUSTOM]) {
        window[windowId].camera.SetZoom(-50.0f * deltaTime);
    }

    if (window[windowId].keys[GLUT_KEY_UP_CUSTOM])
    {
        window[windowId].camera.SetLookAt(0.0f, 50.0f * deltaTime, 0.0f);
    }

    if (window[windowId].keys[GLUT_KEY_DOWN_CUSTOM])
    {
        window[windowId].camera.SetLookAt(0.0f, -50.0f * deltaTime, 0.0f);
    }

    if (window[windowId].keys[GLUT_KEY_LEFT_CUSTOM])
    {
        window[windowId].camera.SetLookAt(-50.0f * deltaTime, 0.0f, 0.0f);
    }

    if (window[windowId].keys[GLUT_KEY_RIGHT_CUSTOM])
    {
        window[windowId].camera.SetLookAt(50.0f * deltaTime, 0.0f, 0.0f);
    }

    if (window[windowId].keys['q'])
    {
        window[windowId].camera.SetLookAt(0.0f, 0.0f, 50.0f * deltaTime);
    }

    if (window[windowId].keys['e'])
    {
        window[windowId].camera.SetLookAt(0.0f, 0.0f, -50.0f * deltaTime);
    }

    if (window[windowId].keys['0'])
    {
        window[windowId].camera.ResetToPosition(window[windowId].cameraStartPosition);
    }
}

//void oneTimeInitialize()
//{
//    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    glViewport(0, 0, WIDTH, HEIGHT);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_MULTISAMPLE);
//    glEnable(GL_CULL_FACE);
//
//    smoothShader.Setup("smooth_shader");
//    flatShader.Setup("flat_shader");
//    lampShader.Setup("lamp");
//
//    glGenVertexArrays(1, &chairVAO);
//    glGenBuffers(1, &chairVBOPos);
//    glGenBuffers(1, &chairVBONormals);
//
//    glBindVertexArray(chairVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, chairVBOPos);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(chairPositions), chairPositions, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
//    glBindBuffer(GL_ARRAY_BUFFER, chairVBONormals);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(chairNormals), chairNormals, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
//    glBindVertexArray(0);
//
//    glGenVertexArrays(1, &tableVAO);
//    glGenBuffers(1, &tableVBOPos);
//    glGenBuffers(1, &tableVBONormals);
//
//    glBindVertexArray(tableVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, tableVBOPos);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(tablePositions), tablePositions, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
//    glBindBuffer(GL_ARRAY_BUFFER, tableVBONormals);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(tableNormals), tableNormals, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
//    glBindVertexArray(0);
//
//    glGenVertexArrays(1, &planeVAO);
//    glGenBuffers(1, &planeVBOPos);
//    glGenBuffers(1, &planeVBONormals);
//
//    glBindVertexArray(planeVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, planeVBOPos);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(planePositions), planePositions, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
//    glBindBuffer(GL_ARRAY_BUFFER, planeVBONormals);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(planeNormals), planeNormals, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
//    glBindVertexArray(0);
//    
//    for (auto i = 0; i < NUM_OF_POINT_LIGHTS; ++i)
//    {
//        pointLights[i].id = i;
//        pointLights[i].position = glm::vec3(lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
//        pointLights[i].material = lightMaterials[i];
//        pointLights[i].active = lightMaterials[i];
//        pointLights[i].constant = lightConstant;
//        pointLights[i].linear = lightAttenuation[1][0];
//        pointLights[i].quadratic = lightAttenuation[1][1];
//    }
//
//    spotLight.id = 0;
//    spotLight.position = spotLightPos;
//    spotLight.direction = glm::normalize(spotLightDir);
//    spotLight.material = spotLightMaterial;
//    spotLight.active = spotLightMaterial;
//    spotLight.constant = lightConstant;
//    spotLight.linear = spotLightAttenuation[0];
//    spotLight.quadratic = spotLightAttenuation[1];
//    spotLight.cutOff = spotLightCutOff;
//    spotLight.outerCutOff = spotLightOuterCutOff;
//
//    for (auto i = 0; i < NUM_OF_DISCO_LIGHTS; ++i)
//    {
//        discoLights[i].id = i;
//        discoLights[i].position = discoLightsPos;
//        discoLights[i].direction = discoLightsDir;
//        discoLights[i].material = discoLightMaterials[i];
//        discoLights[i].active = discoLightMaterials[i];
//        discoLights[i].constant = lightConstant;
//        discoLights[i].linear = discoLightsAttenuation[0];
//        discoLights[i].quadratic = discoLightsAttenuation[1];
//        discoLights[i].cutOff = discoLightsCutOff;
//        discoLights[i].outerCutOff = discoLightsOuterCutOff;
//    }
//
//    ornaments.push_back([] { glDisable(GL_CULL_FACE);  glutSolidTeapot(0.15f); glEnable(GL_CULL_FACE); });
//    ornaments.push_back([] { glutSolidSphere(0.15f, 100, 100); });
//    ornaments.push_back([] { glutSolidCone(0.15f, 0.5f, 100, 100); });
//    ornaments.push_back([] { glutSolidTorus(0.1f, 0.2f, 100, 100); });
//    ornaments.push_back([] { glutSolidDodecahedron(); });
//    ornaments.push_back([] { glutSolidOctahedron(); });
//    ornaments.push_back([] { glutSolidTetrahedron(); });
//    ornaments.push_back([] { glutSolidIcosahedron(); });
//
//    useCurrentShader = std::bind(&Shader::Use, smoothShader);
//    //camera.SetSphericalMode(glm::vec3(0.0f, 0.0f, 0.0f));
//}

void mainWindowDisplayCallback()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

void leftWindowDisplayCallback()
{
    display(0);
}

//void leftWindowDisplayCallback()
//{
//    // Clear the colorbuffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    useCurrentShader();
//    GLuint program;
//    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&program));
//    // Use cooresponding useCurrentShader when setting uniforms/drawing objects
//    
//    glUniform3f(glGetUniformLocation(program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
//    
//    for (auto i = 0; i < NUM_OF_POINT_LIGHTS; ++i)
//    {
//        pointLights[i].Render();
//    }
//
//
//    spotLight.direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * spotLightSwingSpeed);
//    spotLight.direction = glm::normalize(spotLight.direction);
//    spotLight.Render();
//
//    discoLights[0].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[0].direction = glm::normalize(discoLights[0].direction);
//
//    discoLights[1].direction.z = cos((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[1].direction = glm::normalize(discoLights[1].direction);
//
//    //discoLights[2].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[2].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[2].direction.z = cos((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[2].direction = glm::normalize(discoLights[2].direction);
//
//    discoLights[3].direction.x = sin((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[3].direction.z = cos((glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * discoLightSwingSpeed);
//    discoLights[3].direction = glm::normalize(discoLights[3].direction);
//
//    for (auto i = 0; i < NUM_OF_DISCO_LIGHTS; ++i)
//    {
//        std::string uniformVariable = "discoLights[" + std::to_string(i) + "].";
//        glUniform3f(glGetUniformLocation(program, (uniformVariable + "position").c_str()), discoLights[i].position.x, discoLights[i].position.y, discoLights[i].position.z);
//        glUniform3f(glGetUniformLocation(program, (uniformVariable + "direction").c_str()), discoLights[i].direction.x, discoLights[i].direction.y, discoLights[i].direction.z);
//        glUniform3f(glGetUniformLocation(program, (uniformVariable + "ambient").c_str()), discoLights[i].active.ambient.x, discoLights[i].active.ambient.y, discoLights[i].active.ambient.z);
//        glUniform3f(glGetUniformLocation(program, (uniformVariable + "diffuse").c_str()), discoLights[i].active.diffuse.x, discoLights[i].active.diffuse.y, discoLights[i].active.diffuse.z);
//        glUniform3f(glGetUniformLocation(program, (uniformVariable + "specular").c_str()), discoLights[i].active.specular.x, discoLights[i].active.specular.y, discoLights[i].active.specular.z);
//        glUniform1f(glGetUniformLocation(program, (uniformVariable + "constant").c_str()), discoLights[i].constant);
//        glUniform1f(glGetUniformLocation(program, (uniformVariable + "linear").c_str()), discoLights[i].linear);
//        glUniform1f(glGetUniformLocation(program, (uniformVariable + "quadratic").c_str()), discoLights[i].quadratic);
//        glUniform1f(glGetUniformLocation(program, (uniformVariable + "cutOff").c_str()), glm::cos(glm::radians(discoLights[i].cutOff)));
//        glUniform1f(glGetUniformLocation(program, (uniformVariable + "outerCutOff").c_str()), glm::cos(glm::radians(discoLights[i].outerCutOff)));
//    }
//
//    if (projectionMode == ProjectionMode::PERSPECTIVE)
//        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f);
//    else
//        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
//    /*GLint objectColorLoc = glGetUniformLocation(smoothShader(), "objectColor");
//    GLint lightColorLoc = glGetUniformLocation(smoothShader(), "lightColor");
//    GLint lightPosLoc = glGetUniformLocation(smoothShader(), "lightPos");
//    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
//    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
//    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
//    */
//
//    // Create camera transformations
//    view = camera.GetViewMatrix();
//    //view = camera.GetFixedViewMatrix();
//
//    /*GLfloat radius = 3.0f;
//    GLfloat camX = sin(camxoffset) * radius;
//    GLfloat camY = sin(camyoffset) * radius;
//    GLfloat camZ = cos(camzoffset) * radius;
//    view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/
//    //projection = glm::perspective(glm::radians(camera.Zoom), static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f);
//    //glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
//    // Get the uniform locations
//    GLint modelLoc = glGetUniformLocation(program, "model");
//    GLint viewLoc = glGetUniformLocation(program, "view");
//    GLint projLoc = glGetUniformLocation(program, "projection");
//    // Pass the matrices to the useCurrentShader
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//
//    glutSetVertexAttribCoord3(0);
//    glutSetVertexAttribNormal(1);
//    for (auto i = 0; i < NUM_OF_ORNAMENTS; ++i)
//    {
//        if (colorTracking)
//        {
//            glUniform3f(glGetUniformLocation(program, "material.ambient"),
//                ornamentColors[i].x,
//                ornamentColors[i].y,
//                ornamentColors[i].z);
//            glUniform3f(glGetUniformLocation(program, "material.diffuse"),
//                ornamentColors[i].x,
//                ornamentColors[i].y,
//                ornamentColors[i].z);
//            glUniform3f(glGetUniformLocation(program, "material.specular"),
//                ornamentColors[i].x,
//                ornamentColors[i].y,
//                ornamentColors[i].z);
//            glUniform1f(glGetUniformLocation(program, "material.shininess"),
//                ornamentMaterials[i].shininess);
//        } else
//        {
//            glUniform3f(glGetUniformLocation(program, "material.ambient"),
//                ornamentMaterials[i].ambient.x,
//                ornamentMaterials[i].ambient.y,
//                ornamentMaterials[i].ambient.z);
//            glUniform3f(glGetUniformLocation(program, "material.diffuse"),
//                ornamentMaterials[i].diffuse.x,
//                ornamentMaterials[i].diffuse.y,
//                ornamentMaterials[i].diffuse.z);
//            glUniform3f(glGetUniformLocation(program, "material.specular"),
//                ornamentMaterials[i].specular.x,
//                ornamentMaterials[i].specular.y,
//                ornamentMaterials[i].specular.z);
//            glUniform1f(glGetUniformLocation(program, "material.shininess"),
//                ornamentMaterials[i].shininess);
//        }
//        
//        model = glm::mat4();
//        model = glm::translate(model, ornamentPositions[i]);
//        model = glm::rotate(model, glm::radians(ornamentRotations[i].x), glm::vec3(1.0f, 0.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(ornamentRotations[i].y), glm::vec3(0.0f, 1.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(ornamentRotations[i].z), glm::vec3(0.0f, 0.0f, 1.0f));
//        model = glm::scale(model, ornamentScales[i]);
//        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//        ornaments[i]();
//    }
//    glutSetVertexAttribCoord3(-1);
//    glutSetVertexAttribNormal(-1);
//
//    glUniform3f(glGetUniformLocation(program, "material.ambient"),
//        planeMaterial.ambient.x,
//        planeMaterial.ambient.y,
//        planeMaterial.ambient.z);
//    glUniform3f(glGetUniformLocation(program, "material.diffuse"),
//        planeMaterial.diffuse.x,
//        planeMaterial.diffuse.y,
//        planeMaterial.diffuse.z);
//    glUniform3f(glGetUniformLocation(program, "material.specular"),
//        planeMaterial.specular.x,
//        planeMaterial.specular.y,
//        planeMaterial.specular.z);
//    glUniform1f(glGetUniformLocation(program, "material.shininess"),
//        planeMaterial.shininess);
//    glBindVertexArray(planeVAO);
//    model = glm::mat4();
//    model = glm::scale(model, glm::vec3(2.913f, 1.0f, 2.913f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
//    glBindVertexArray(0);
//
//    glBindVertexArray(planeVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(0.0f, 2.28f, -2.28f));
//    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(2.913f, 1.0f, 2.913f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
//    glBindVertexArray(0);
//
//
//    glBindVertexArray(planeVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(0.0f, 2.28f, 2.28f));
//    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(2.913f, 1.0f, 2.913f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
//    glBindVertexArray(0);
//
//    glBindVertexArray(planeVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(0.0f, 4.56f, 0.0f));
//    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(2.913f, 1.0f, 2.913f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
//    glBindVertexArray(0);
//
//
//    glBindVertexArray(planeVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(2.28f, 2.28f, 0.0f));
//    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    //model = rotate_with_keyboard(model);
//    model = glm::scale(model, glm::vec3(2.913f, 1.0f, 2.913f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
//    glBindVertexArray(0);
//
//    glBindVertexArray(planeVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(-2.28f, 2.28f, 0.0f));
//    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    //model = rotate_with_keyboard(model);
//    model = glm::scale(model, glm::vec3(2.913f, 1.0f, 2.913f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, planeVertices);
//    glBindVertexArray(0);
//
//
//    glUniform3f(glGetUniformLocation(program, "material.ambient"),
//        tableMaterial.ambient.x,
//        tableMaterial.ambient.y,
//        tableMaterial.ambient.z);
//    glUniform3f(glGetUniformLocation(program, "material.diffuse"),
//        tableMaterial.diffuse.x,
//        tableMaterial.diffuse.y,
//        tableMaterial.diffuse.z);
//    glUniform3f(glGetUniformLocation(program, "material.specular"),
//        tableMaterial.specular.x,
//        tableMaterial.specular.y,
//        tableMaterial.specular.z);
//    glUniform1f(glGetUniformLocation(program, "material.shininess"),
//        tableMaterial.shininess);
//    glBindVertexArray(tableVAO);
//    model = glm::mat4();
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, tableVertices);
//    glBindVertexArray(0);
//
//
//    glUniform3f(glGetUniformLocation(program, "material.ambient"),
//        chairMaterial.ambient.x,
//        chairMaterial.ambient.y,
//        chairMaterial.ambient.z);
//    glUniform3f(glGetUniformLocation(program, "material.diffuse"),
//        chairMaterial.diffuse.x,
//        chairMaterial.diffuse.y,
//        chairMaterial.diffuse.z);
//    glUniform3f(glGetUniformLocation(program, "material.specular"),
//        chairMaterial.specular.x,
//        chairMaterial.specular.y,
//        chairMaterial.specular.z);
//    glUniform1f(glGetUniformLocation(program, "material.shininess"),
//        chairMaterial.shininess);
//    glBindVertexArray(chairVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(1.2482f, -0.34394f, 0.0f));
//    model = glm::rotate(model, glm::radians(28.01f), glm::vec3(0.0f, 1.0f, 0.0f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, chairVertices);
//    glBindVertexArray(0);
//
//    glBindVertexArray(chairVAO);
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(-0.12125f, -0.34394, -1.34712));
//    model = glm::rotate(model, glm::radians(130.738f), glm::vec3(0.0f, 1.0f, 0.0f));
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, chairVertices);
//    glBindVertexArray(0);
//
//    // Also draw the lamp object, again binding the appropriate useCurrentShader
//    lampShader.Use();
//    //// Get location objects for the matrices on the lamp useCurrentShader (these could be different on a different useCurrentShader)
//    modelLoc = glGetUniformLocation(lampShader(), "model");
//    viewLoc = glGetUniformLocation(lampShader(), "view");
//    projLoc = glGetUniformLocation(lampShader(), "projection");
//    //// Set matrices
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//    glutSetVertexAttribCoord3(0);
//    glutSetVertexAttribNormal(1);
//    for (auto i = 0; i < NUM_OF_POINT_LIGHTS; ++i)
//    {
//        model = glm::mat4();
//        model = glm::translate(model, lightPositions[i]);
//        model = glm::scale(model, glm::vec3(0.05f)); // Make it a smaller cube
//        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//        glutSolidSphere(1.0f, 100, 100);
//    }
//    glutSetVertexAttribCoord3(-1);
//    glutSetVertexAttribNormal(-1);
//
//    glutSwapBuffers();
//}

void rightWindowDisplayCallback()
{
    /*glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glutSwapBuffers();*/
    display(1);
}

void instructionDisplayCallback()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 1200, 200);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0f, 1200, -1.0f, 200);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3ub(0, 0, 0);
    renderText(20, 180, GLUT_BITMAP_HELVETICA_12, "z - Points mode");
    renderText(20, 160, GLUT_BITMAP_HELVETICA_12, "x - Wireframe mode");
    renderText(20, 140, GLUT_BITMAP_HELVETICA_12, "c - Solid mode");
    renderText(20, 120, GLUT_BITMAP_HELVETICA_12, "v - Toggle shading");

    glColor3ub(255, 0, 0);
    renderText(20, 100, GLUT_BITMAP_HELVETICA_12, "b - Toggle color tracking");
    renderText(20, 80, GLUT_BITMAP_HELVETICA_12, "n - Toggle ambient lighting");
    glColor3ub(0, 0, 0);

    renderText(20, 60, GLUT_BITMAP_HELVETICA_12, "m - Toggle backface culling");
    renderText(20, 40, GLUT_BITMAP_HELVETICA_12, ", - Toggle culling face");
    renderText(20, 20, GLUT_BITMAP_HELVETICA_12, ". - Toggle depth testing");

    renderText(220, 180, GLUT_BITMAP_HELVETICA_12, "1 - Toggle point light 1");
    renderText(220, 160, GLUT_BITMAP_HELVETICA_12, "2 - Toggle point light 2");
    renderText(220, 140, GLUT_BITMAP_HELVETICA_12, "3 - Toggle spot light");
    renderText(220, 120, GLUT_BITMAP_HELVETICA_12, "4 - Toggle disco mode");
    renderText(220, 100, GLUT_BITMAP_HELVETICA_12, "[ - Speedup spotlight swing");
    renderText(220, 80, GLUT_BITMAP_HELVETICA_12, "] - Slowdown spotlight swing");
    renderText(220, 60, GLUT_BITMAP_HELVETICA_12, "ESC - Quit");

    renderText(420, 180, GLUT_BITMAP_HELVETICA_12, "w - Move forward");
    renderText(420, 160, GLUT_BITMAP_HELVETICA_12, "a - Move backward");
    renderText(420, 140, GLUT_BITMAP_HELVETICA_12, "s - Strafe left");
    renderText(420, 120, GLUT_BITMAP_HELVETICA_12, "d - Strafe right");
    renderText(420, 100, GLUT_BITMAP_HELVETICA_12, "q - Roll left");
    renderText(420, 80, GLUT_BITMAP_HELVETICA_12, "e - Roll right");
    renderText(420, 60, GLUT_BITMAP_HELVETICA_12, "UP - Look up");
    renderText(420, 40, GLUT_BITMAP_HELVETICA_12, "DOWN - Look down");
    renderText(420, 20, GLUT_BITMAP_HELVETICA_12, "LEFT - Look left");

    renderText(620, 180, GLUT_BITMAP_HELVETICA_12, "RIGHT - Look right");
    renderText(620, 160, GLUT_BITMAP_HELVETICA_12, "PAGE UP - Move up");
    renderText(620, 140, GLUT_BITMAP_HELVETICA_12, "PAGE DOWN - Move down");
    renderText(620, 120, GLUT_BITMAP_HELVETICA_12, "HOME - Zoom in");
    renderText(620, 100, GLUT_BITMAP_HELVETICA_12, "END - Zoom out");
    renderText(620, 80, GLUT_BITMAP_HELVETICA_12, "0 - Reset camera");

    glutSwapBuffers();
}

void idleCallback()
{
    // Calculate deltatime of current frame
    GLfloat currentFrame = static_cast<GLfloat>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    handleSmoothInput(0);
    handleSmoothInput(1);

    glutSetWindow(leftWindow);
    glutPostRedisplay();
    glutSetWindow(rightWindow);
    glutPostRedisplay();
}

void leftWindowKeyPressCallback(unsigned char key, int x, int y) {
    handleKeyPress(0, key, x, y);
}

void leftWindowKeyUpCallback(unsigned char key, int x, int y) {
    handleKeyUp(0, key, x, y);
}

void leftWindowSpecialPressCallback(int key, int x, int y) {
    handleSpecialPress(0, key, x, y);
}

void leftWindowSpecialUpCallback(int key, int x, int y) {
    handleSpecialUp(0, key, x, y);
}

void rightWindowKeyPressCallback(unsigned char key, int x, int y) {
    handleKeyPress(1, key, x, y);
}

void rightWindowKeyUpCallback(unsigned char key, int x, int y) {
    handleKeyUp(1, key, x, y);
}

void rightWindowSpecialPressCallback(int key, int x, int y) {
    handleSpecialPress(1, key, x, y);
}

void rightWindowSpecialUpCallback(int key, int x, int y) {
    handleSpecialUp(1, key, x, y);
}

//void key_press_callback(unsigned char key, int x, int y)
//{
//    handleKeyPress(0, key, x, y);
//}
//
//void key_up_callback(unsigned char key, int x, int y)
//{
//    if (key == GLUT_KEY_ESCAPE)
//        glutLeaveMainLoop();
//    if (key >= 0 && key < 1024)
//    {
//        keys[key] = false;
//    }
//}
//
//void key_special_press_callback(int key, int x, int y)
//{
//    if (key == GLUT_KEY_PAGE_UP)
//        keys[GLUT_KEY_PAGE_UP_CUSTOM] = true;
//    else if (key == GLUT_KEY_PAGE_DOWN)
//        keys[GLUT_KEY_PAGE_DOWN_CUSTOM] = true;
//
//    if (key == GLUT_KEY_HOME)
//        keys[GLUT_KEY_HOME_CUSTOM] = true;
//    else if (key == GLUT_KEY_END)
//        keys[GLUT_KEY_END_CUSTOM] = true;
//
//    if (key == GLUT_KEY_UP)
//        keys[GLUT_KEY_UP_CUSTOM] = true;
//    else if (key == GLUT_KEY_DOWN)
//        keys[GLUT_KEY_DOWN_CUSTOM] = true;
//    else if (key == GLUT_KEY_LEFT)
//        keys[GLUT_KEY_LEFT_CUSTOM] = true;
//    else if (key == GLUT_KEY_RIGHT)
//        keys[GLUT_KEY_RIGHT_CUSTOM] = true;
//}
//
//void key_special_up_callback(int key, int x, int y)
//{
//    if (key == GLUT_KEY_PAGE_UP)
//        keys[GLUT_KEY_PAGE_UP_CUSTOM] = false;
//    else if (key == GLUT_KEY_PAGE_DOWN)
//        keys[GLUT_KEY_PAGE_DOWN_CUSTOM] = false;
//
//    if (key == GLUT_KEY_HOME)
//        keys[GLUT_KEY_HOME_CUSTOM] = false;
//    else if (key == GLUT_KEY_END)
//        keys[GLUT_KEY_END_CUSTOM] = false;
//
//    if (key == GLUT_KEY_UP)
//        keys[GLUT_KEY_UP_CUSTOM] = false;
//    else if (key == GLUT_KEY_DOWN)
//        keys[GLUT_KEY_DOWN_CUSTOM] = false;
//    else if (key == GLUT_KEY_LEFT)
//        keys[GLUT_KEY_LEFT_CUSTOM] = false;
//    else if (key == GLUT_KEY_RIGHT)
//        keys[GLUT_KEY_RIGHT_CUSTOM] = false;
//}

//void do_movement()
//{
//    // Camera controls
//    if (keys[GLUT_KEY_W])
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (keys[GLUT_KEY_S])
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (keys[GLUT_KEY_A])
//    {
//        camera.ProcessKeyboard(LEFT, deltaTime);
//        /*camxoffset -= 2.0f * deltaTime;
//        camzoffset -= 2.0f * deltaTime;*/
//    }
//    if (keys[GLUT_KEY_D])
//        
//    {
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//        /*camxoffset += 2.0f * deltaTime;
//        camzoffset += 2.0f * deltaTime;*/
//    }
//    if (keys[GLUT_KEY_PAGE_UP_CUSTOM])
//    {
//        camera.ProcessKeyboard(UP, deltaTime);
//        /*if (camyoffset > 360.0f)
//            camyoffset = 0.0f;
//        camyoffset -= 2.0f * deltaTime;
//        camzoffset -= 2.0f * deltaTime;*/
//    }
//    if (keys[GLUT_KEY_PAGE_DOWN_CUSTOM])
//        
//    {
//        camera.ProcessKeyboard(DOWN, deltaTime);
//    }
//
//    if (keys[GLUT_KEY_HOME_CUSTOM]) {
//        camera.SetZoom(50.0f * deltaTime);
//    }
//    if (keys[GLUT_KEY_END_CUSTOM]) {
//        camera.SetZoom(-50.0f * deltaTime);
//    }
//
//    if (keys[GLUT_KEY_UP_CUSTOM])
//        camera.SetLookAt(0.0f, 50.0f * deltaTime, 0.0f);
//    if (keys[GLUT_KEY_DOWN_CUSTOM])
//        camera.SetLookAt(0.0f, -50.0f * deltaTime, 0.0f);
//    if (keys[GLUT_KEY_LEFT_CUSTOM])
//        camera.SetLookAt(-50.0f * deltaTime, 0.0f, 0.0f);
//    if (keys[GLUT_KEY_RIGHT_CUSTOM])
//        camera.SetLookAt(50.0f * deltaTime, 0.0f, 0.0f);
//    if (keys[GLUT_KEY_Q])
//        camera.SetLookAt(0.0f, 0.0f, 50.0f * deltaTime);
//    if (keys[GLUT_KEY_E])
//        camera.SetLookAt(0.0f, 0.0f, -50.0f * deltaTime);
//    
//    if (keys['o'])
//    {
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    }
//    if (keys['p'])
//    {
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    }
//    if (keys['u']) {
//        useCurrentShader = std::bind(&Shader::Use, flatShader);
//    }
//    if (keys['i']) {
//        useCurrentShader = std::bind(&Shader::Use, smoothShader);
//    }
//    if (keys['k'])
//        glEnable(GL_DEPTH_TEST);
//    if (keys['l'])
//        glDisable(GL_DEPTH_TEST);
//    
//    if (keys['0'])
//        camera.ResetToPosition(cameraStartPosition);
//    if (keys['='])
//    {
//        spotLightSwingSpeed+=0.1f;
//    }
//    if (keys['['])
//    {
//        projectionMode = ProjectionMode::PERSPECTIVE;
//    }
//    if (keys[']'])
//    {
//        projectionMode = ProjectionMode::ORTHO;
//    }
//    
//    if (keys['-']) {
//        spotLightSwingSpeed-=0.1f;
//        if (spotLightSwingSpeed < 1.0f)
//        {
//            spotLightSwingSpeed = 1.0f;
//        }
//    }
//    
//}

void renderText(const float &x, const float &y, void *font, const char *const str) {
    const char *ptr; // Temp pointer to position in string

    glRasterPos2f(x, y); // Place raster position

    for (ptr = str; *ptr != '\0'; ++ptr) {
        glutBitmapCharacter(font, *ptr); // Draw bitmap character
    }
}