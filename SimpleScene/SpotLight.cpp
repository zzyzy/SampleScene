#include "SpotLight.h"

#include <glm/glm.hpp>

void SpotLight::Render() const
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&program));
    glUniform3f(glGetUniformLocation(program, "spotLight.position"), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(program, "spotLight.direction"), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(program, "spotLight.ambient"), active.ambient.x, active.ambient.y, active.ambient.z);
    glUniform3f(glGetUniformLocation(program, "spotLight.diffuse"), active.diffuse.x, active.diffuse.y, active.diffuse.z);
    glUniform3f(glGetUniformLocation(program, "spotLight.specular"), active.specular.x, active.specular.y, active.specular.z);
    glUniform1f(glGetUniformLocation(program, "spotLight.constant"), constant);
    glUniform1f(glGetUniformLocation(program, "spotLight.linear"), linear);
    glUniform1f(glGetUniformLocation(program, "spotLight.quadratic"), quadratic);
    glUniform1f(glGetUniformLocation(program, "spotLight.cutOff"), glm::cos(glm::radians(cutOff)));
    glUniform1f(glGetUniformLocation(program, "spotLight.outerCutOff"), glm::cos(glm::radians(outerCutOff)));
}
