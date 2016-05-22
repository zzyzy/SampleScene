/*
    SpotLight.h

    TODO Find a way to better support multiple spotlights with different names in shader
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_SPOT_LIGHT_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_SPOT_LIGHT_H_INCLUDED

#include "PointLight.h"

struct SpotLight : PointLight
{
    glm::vec3 direction;
    GLfloat cutOff;
    GLfloat outerCutOff;

    SpotLight() = default;
    ~SpotLight() = default;
    void Render() const override;
};

#endif