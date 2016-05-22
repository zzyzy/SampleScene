/*
    LightProperties.h
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_LIGHT_PROPERTIES_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_LIGHT_PROPERTIES_H_INCLUDED

#include "Material.h"

static const glm::vec3 lightPositions[] = {
    glm::vec3(0.0f, 1.5f, 2.85f),
    glm::vec3(0.0f, 1.5f, -2.85f)
};
static const Material lightMaterials[] = {
    Material(glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f)),
    Material(glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f)), 
};
static const GLfloat lightConstant = 1.0f;
static const GLfloat lightAttenuation[][2] = {
    { 0.09f, 0.032f },
    { 0.09f, 0.032f }
};

static const glm::vec3 spotLightPos(0.0f, 2.0f, 0.0f);
static const glm::vec3 spotLightDir(0.0f, -1.0f, 0.0f);
static const Material spotLightMaterial(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
static const GLfloat spotLightAttenuation[2] = {
    0.09f, 0.032f
};
static const GLfloat spotLightCutOff = 22.5f;
static const GLfloat spotLightOuterCutOff = 25.0f;

static const glm::vec3 discoLightsPos(0.0f, 2.0f, 0.0f);
static const glm::vec3 discoLightsDir(0.0f, -1.0f, 0.0f);
static const Material discoLightMaterials[] = {
    Material(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
    Material(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
    Material(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,  	0.0f,  1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
    Material(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
};
static const GLfloat discoLightsAttenuation[2] = {
    0.09f, 0.032f
};
static const GLfloat discoLightsCutOff = 20.5f;
static const GLfloat discoLightsOuterCutOff = 23.0f;

#endif
