#include "SceneRenderer.h"

#include <cmath>

constexpr glm::vec3 kWorldUp(0.0f, 1.0f, 0.0f);
constexpr float kGravityPlaneAlpha = 0.25f;

bool SceneRenderer::Initialize()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Cube.SetPosition(glm::vec3(0.0f));

    return m_Shader.Create();
}

void SceneRenderer::Update(Simulation& sim)
{
    m_Cube.SetScale(sim.Params().a);
    m_Cube.SetRotation(sim.Orientation());

    if (m_Settings.showDiagonal)
    {
        const glm::vec3 freeEnd = m_Cube.GetPosition() + sim.FreeEndWorldPos();
        m_Diagonal.SetPoints({m_Cube.GetPosition(), freeEnd});
    }

    if (m_Settings.showTrajectory)
    {
        sim.Trail().Sync();
    }

    if (m_Settings.showGravity)
    {
        UpdateGravityArrow(sim.Params().a, sim.Params().g);
        UpdateGravityPlane(sim.Params().g);
    }
}

void SceneRenderer::UpdateGravityArrow(float cubeSize, const glm::vec3& gravity)
{
    const glm::vec3 gDir = glm::normalize(gravity);
    const glm::vec3 sideRef = std::abs(gDir.y) < 0.9f ? kWorldUp : glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 gPerp = glm::normalize(glm::cross(gDir, sideRef));

    const float arrowLength = 1.5f * cubeSize;

    const glm::vec3 tip = gDir * arrowLength;
    const glm::vec3 barbBase = tip - gDir * (0.2f * arrowLength);
    const glm::vec3 barbOffset = gPerp * (0.1f * arrowLength);

    m_GravityArrow.SetPoints({glm::vec3(0.0f), tip, barbBase + barbOffset, tip, barbBase - barbOffset});
}

void SceneRenderer::UpdateGravityPlane(const glm::vec3& gravity)
{
    m_GravityPlane.SetRotation(glm::quat(kWorldUp, -glm::normalize(gravity)));
    m_GravityPlane.SetScale(m_Settings.gravityPlaneSize);
}

void SceneRenderer::BeginFrame(int width, int height) const
{
    glViewport(0, 0, width, height);
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneRenderer::Draw(const Simulation& sim, const glm::mat4& viewProjection) const
{
    m_Shader.Use();
    m_Shader.SetAlpha(1.0f);

    if (m_Settings.showCube)
    {
        m_Shader.SetColor(glm::vec3(1.f, 0.f, 0.f));
        m_Cube.Draw(m_Shader, viewProjection);
    }
    if (m_Settings.showDiagonal)
    {
        m_Shader.SetColor(glm::vec3(0.f, 1.f, 1.f));
        m_Diagonal.Draw(m_Shader, viewProjection);
    }
    if (m_Settings.showTrajectory)
    {
        m_Shader.SetColor(glm::vec3(1.f, 0.f, 1.f));
        sim.Trail().Draw(m_Shader, viewProjection);
    }
    if (m_Settings.showGravity)
    {
        m_Shader.SetColor(glm::vec3(1.f, 0.f, 0.f));
        m_GravityArrow.Draw(m_Shader, viewProjection);

        glDepthMask(GL_FALSE);
        m_Shader.SetAlpha(kGravityPlaneAlpha);

        m_Shader.SetColor(glm::vec3(0.3f, 0.3f, 0.32f));
        m_GravityPlane.Draw(m_Shader, viewProjection);
        m_Shader.SetAlpha(1.0f);
        glDepthMask(GL_TRUE);
    }
}
