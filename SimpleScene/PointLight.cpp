#include "PointLight.h"

#include <string>

void PointLight::Render() const
{
    GLuint program;
    auto uniformName = "pointLights[" + std::to_string(id) + "]";
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&program));
    glUniform3f(glGetUniformLocation(program, (uniformName + ".position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(program, (uniformName + ".ambient").c_str()), active.ambient.x, active.ambient.y, active.ambient.z);
    glUniform3f(glGetUniformLocation(program, (uniformName + ".diffuse").c_str()), active.diffuse.x, active.diffuse.y, active.diffuse.z);
    glUniform3f(glGetUniformLocation(program, (uniformName + ".specular").c_str()), active.specular.x, active.specular.y, active.specular.z);
    glUniform1f(glGetUniformLocation(program, (uniformName + ".constant").c_str()), constant);
    glUniform1f(glGetUniformLocation(program, (uniformName + ".linear").c_str()), linear);
    glUniform1f(glGetUniformLocation(program, (uniformName + ".quadratic").c_str()), quadratic);
}

void PointLight::On()
{
    active = material;
}

void PointLight::Off()
{
    active = Material(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void PointLight::Toggle()
{
    (active == material ? Off() : On());
}