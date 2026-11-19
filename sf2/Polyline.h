#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "SceneObject.h"

class Polyline : public SceneObject
{
    static_assert(sizeof(glm::vec3) == 3 * sizeof(float), "glm::vec3 musi byc ciasno upakowany");

public:
    void SetPoints(const std::vector<glm::vec3>& points)
    {
        std::vector<unsigned int> indices;
        if (points.size() >= 2)
        {
            indices.reserve((points.size() - 1) * 2);
            for (unsigned int i = 0; i + 1 < points.size(); ++i)
            {
                indices.push_back(i);
                indices.push_back(i + 1);
            }
        }

        SetGeometry(reinterpret_cast<const float*>(points.data()), points.size() * 3, indices.data(), indices.size(),
                    GL_LINES, GL_DYNAMIC_DRAW);
    }

protected:
    void ReservePoints(size_t pointCapacity)
    {
        ReserveVertices(pointCapacity * 3, GL_LINE_STRIP);
    }

    void UploadPoints(const glm::vec3* points, size_t count, size_t firstDirty)
    {
        if (firstDirty < count)
        {
            UpdateVertices(reinterpret_cast<const float*>(points + firstDirty), (count - firstDirty) * 3,
                           firstDirty * 3);
        }

        SetVertexCount(count);
    }
};
