#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

const GLint WIDTH = 800;
const GLint HEIGHT = 600;
const GLchar* TITLE = "Sample Scene";

Shader shader;

GLfloat vertices[] = {
    // Positions          // Colors           // Texture Coords
    0.5f,  0.5f, 0.0f, // Top Right
    0.5f, -0.5f, 0.0f, // Bottom Right
    -0.5f, -0.5f, 0.0f, // Bottom Left
    -0.5f,  0.5f, 0.0f, // Top Left 
};
GLuint indices[] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};
GLuint VBO, VAO, EBO;

void InitGL();
void Display();
void Idle();

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

    InitGL();
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);

    glutMainLoop();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    return 0;
}

void InitGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    shader.Setup("transformations.vert", "transformations.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    shader.Use();

    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glutSwapBuffers();
}

void Idle()
{
    shader.Use();
    glm::mat4 transform;
    transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
    transform = glm::rotate(transform, (static_cast<GLfloat>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f) * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    GLint transformLoc = glGetUniformLocation(shader(), "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glutPostRedisplay();
}