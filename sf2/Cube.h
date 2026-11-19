#pragma once

#include "SceneObject.h"

class Cube : public SceneObject
{
private:
    static const inline float kCubeVertices[] = {
        -0.5f, -0.5f, 0.5f,  // front-bottom-left 
        0.5f,  -0.5f, 0.5f,  // front-bottom-right
        0.5f,  0.5f,  0.5f,  // front-top-right   
        -0.5f, 0.5f,  0.5f,  // front-top-left 
        -0.5f, -0.5f, -0.5f, // back-bottom-left  
        0.5f,  -0.5f, -0.5f, // back-bottom-right 
        0.5f,  0.5f,  -0.5f, // back-top-right 
        -0.5f, 0.5f,  -0.5f, // back-top-left   
    };

    static const inline unsigned int kCubeIndices[] = {
        0, 1, 2, 2, 3, 0, // front
        5, 4, 7, 7, 6, 5, // back
        4, 0, 3, 3, 7, 4, // left
        1, 5, 6, 6, 2, 1, // right
        3, 2, 6, 6, 7, 3, // top
        4, 5, 1, 1, 0, 4, // bottom
    };

public:
    Cube(glm::vec3 pivot = glm::vec3{-0.5f, -0.5f, -0.5f})
        : SceneObject(kCubeVertices, sizeof(kCubeVertices) / sizeof(float), kCubeIndices,
                      sizeof(kCubeIndices) / sizeof(unsigned int), /*drawAsLines=*/true, pivot) {}
};
