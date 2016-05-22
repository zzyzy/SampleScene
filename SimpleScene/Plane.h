/*
    Plane.h
*/

#pragma once
#ifndef SIMPLE_SCENE_INCLUDE_PLANE_H_INCLUDED
#define SIMPLE_SCENE_INCLUDE_PLANE_H_INCLUDED

#include <GL/glew.h>

static const int planeVertices = 6;

static const GLfloat planePositions[] = {1.0f,-0.626866f,1.0f,1.0f,-0.626866f,-1.0f,-1.0f,-0.626866f,-1.0f,
    -1.0f,-0.626866f,1.0f,1.0f,-0.626866f,1.0f,-1.0f,-0.626866f,-1.0f,
};

static const GLfloat planeNormals[] = {0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,
};

#endif
