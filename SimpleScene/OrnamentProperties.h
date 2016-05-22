/*
    OrnamentProperties.h
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_ORNAMENT_PROPERTIES_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_ORNAMENT_PROPERTIES_H_INCLUDED

#include "Material.h"

static const glm::vec3 ornamentPositions[] = {
    glm::vec3(0.0f, 0.17f, 0.0f), // Teapot
    glm::vec3(0.4f, 0.20f, -0.4f), // Sphere
    glm::vec3(0.644f, 0.06f, 0.444f), // Cone
    glm::vec3(-0.644f, 0.15f, -0.444f), // Torus
    glm::vec3(-0.794f, 0.20f, 0.444f), // Dodecahedron
    glm::vec3(-0.41111f, 0.18f, 0.4222f), // Octahedron
    glm::vec3(0.21111f, 0.12f, 0.7222f), // Tetrahedron
    glm::vec3(0.71111f, 0.21f, -0.5222f) // Icosahedron
};

static const glm::vec3 ornamentRotations[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(-90.0f, 0.0f, 0.0f),
    glm::vec3(-90.0f, 0.0f, 0.0f),
    glm::vec3(60.0f, 0.0f, 0.0f),
    glm::vec3(36.0f, 0.0f, 45.0f),
    glm::vec3(0.0f, 0.0f, -20.0f),
    glm::vec3(36.0f, 0.0f, 13.0f)
};

static const glm::vec3 ornamentScales[] = {
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(0.1f),
    glm::vec3(0.2f),
    glm::vec3(0.2f),
    glm::vec3(0.2f),
};

static const Material ornamentMaterials[] = {
    Material(glm::vec3(0.0215f, 0.1745f, 0.0215f), glm::vec3(0.07568f, 0.61424f, 0.07568f), glm::vec3(0.633f, 0.727811f, 0.633f), 0.6f * 128),
    Material(glm::vec3(0.135f, 0.2225f, 0.1575f), glm::vec3(0.54f, 0.89f, 0.63f), glm::vec3(0.316228f, 0.316228f, 0.316228f), 0.1f * 128),
    Material(glm::vec3(0.05375f, 0.05f, 0.06625f), glm::vec3(0.18275f, 0.17f, 0.22525f), glm::vec3(0.332741f, 0.328634f, 0.346435f), 0.3f * 128),
    Material(glm::vec3(0.25f, 0.20725f, 0.20725f), glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.296648f, 0.296648f, 0.296648f), 0.088f * 128),
    Material(glm::vec3(0.1745f, 0.01175f, 0.01175f), glm::vec3(0.61424f, 0.04136f, 0.04136f), glm::vec3(0.727811f, 0.626959f, 0.626959f), 0.6f * 128),
    Material(glm::vec3(0.1f, 0.18725f, 0.1745f), glm::vec3(0.396f, 0.74151f, 0.69102f), glm::vec3(0.297254f, 0.30829f, 0.306678f), 0.1f * 128),
    Material(glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 0.21794872f * 128),
    Material(glm::vec3(0.2125f, 0.1275f, 0.054f), glm::vec3(0.714f, 0.4284f, 0.18144f), glm::vec3(0.393548f, 0.271906f, 0.166721f), 0.2f * 128),
    Material(glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.774597f, 0.774597f, 0.774597f), 0.6f * 128),
    Material(glm::vec3(0.19125f, 0.0735f, 0.0225f), glm::vec3(0.7038f, 0.27048f, 0.0828f), glm::vec3(0.256777f, 0.137622f, 0.086014f), 0.1f * 128)
};

static const glm::vec3 ornamentColors[] = {
    glm::vec3(0.07568f, 0.61424f, 0.07568f),
    glm::vec3(0.54f, 0.89f, 0.63f),
    glm::vec3(0.18275f, 0.17f, 0.22525f),
    glm::vec3(1.0f, 0.829f, 0.829f),
    glm::vec3(0.61424f, 0.04136f, 0.04136f),
    glm::vec3(0.396f, 0.74151f, 0.69102f),
    glm::vec3(0.780392f, 0.568627f, 0.113725f),
    glm::vec3(0.714f, 0.4284f, 0.18144f),
    glm::vec3(0.4f, 0.4f, 0.4f),
    glm::vec3(0.7038f, 0.27048f, 0.0828f)
};

#endif