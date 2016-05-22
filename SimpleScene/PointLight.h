/*
    PointLight.h

    TODO May need to handle variable uniform name in shader like SpotLight.h
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_POINT_LIGHT_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_POINT_LIGHT_H_INCLUDED

#include <GL/glew.h>

#include "Material.h"

struct PointLight
{
    int id;
    glm::vec3 position;
    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;
    Material material;
    Material active;

    PointLight() = default;
    virtual ~PointLight() = default;
    virtual void Render() const;
    virtual void On();
    virtual void Off();
    virtual void Toggle();
};

#endif