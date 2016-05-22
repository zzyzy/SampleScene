/*
    ObjectProperties.h
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_OBJECT_PROPERTIES_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_OBJECT_PROPERTIES_H_INCLUDED

#include "Material.h"

static const Material planeMaterial(
    glm::vec3(0.25f, 0.20725f, 0.20725f),
    glm::vec3(1.0f, 0.829f, 0.829f),
    glm::vec3(0.296648f, 0.296648f, 0.296648f),
    11.264f
);

static const Material tableMaterial(
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.55f, 0.55f, 0.55f),
    glm::vec3(0.70f, 0.70f, 0.70f),
    32.0f
);

static const Material chairMaterial(
    glm::vec3(0.02f, 0.02f, 0.02f),
    glm::vec3(0.01f, 0.01f, 0.01f),
    glm::vec3(0.4f, 0.4f, 0.4f),
    10.0f
);

#endif