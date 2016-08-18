/*
    Material.h
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_MATERIAL_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_MATERIAL_H_INCLUDED

#include <glm/glm.hpp>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    Material() = default;
    Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess = 0)
        : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
    {
    }
    ~Material() = default;

    void operator=(const Material& material) {
        this->ambient = material.ambient;
        this->diffuse = material.diffuse;
        this->specular = material.specular;
        this->shininess = material.shininess;
    }

    bool operator==(const Material& material) const {
        return this->ambient == material.ambient &&
            this->diffuse == material.diffuse &&
            this->specular == material.specular &&
            this->shininess == material.shininess;
    }
};

#endif