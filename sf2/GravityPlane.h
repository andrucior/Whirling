#pragma once

#include "SceneObject.h"

class GravityPlane : public SceneObject
{
private:
    static const inline float kVertices[] = {
        -1.0f, 0.0f, -1.0f,
        1.0f,  0.0f, -1.0f,
        1.0f,  0.0f, 1.0f,  
        -1.0f, 0.0f, 1.0f,  
    };

    static const inline unsigned int kIndices[] = {0, 1, 2, 2, 3, 0};

public:
    GravityPlane()
        : SceneObject(kVertices, sizeof(kVertices) / sizeof(float), kIndices,
                      sizeof(kIndices) / sizeof(unsigned int), /*drawAsLines=*/false) {}
};
