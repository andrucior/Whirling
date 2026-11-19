#pragma once

#include <glm/glm.hpp>

#include "Cube.h"
#include "GravityPlane.h"
#include "Polyline.h"
#include "SceneShader.h"
#include "Simulation.h"

struct RenderSettings
{
    bool showCube = true;
    bool showDiagonal = true;
    bool showTrajectory = true;
    bool showGravity = true;
    float gravityPlaneSize = 2.f;
};

class SceneRenderer
{
public:
    bool Initialize();

    RenderSettings& Settings()
    {
        return m_Settings;
    }
    const RenderSettings& Settings() const
    {
        return m_Settings;
    }

    void Update(Simulation& sim);
    void BeginFrame(int width, int height) const;
    void Draw(const Simulation& sim, const glm::mat4& viewProjection) const;

private:
    void UpdateGravityArrow(float cubeSize, const glm::vec3& gravity);
    void UpdateGravityPlane(const glm::vec3& gravity);

    RenderSettings m_Settings;
    SceneShader m_Shader;

    Cube m_Cube;
    Polyline m_Diagonal;
    Polyline m_GravityArrow;
    GravityPlane m_GravityPlane;

    glm::vec3 m_ClearColor{0.1f, 0.1f, 0.12f};
};
