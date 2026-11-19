#include "SceneView.h"

#include <glm/gtc/matrix_transform.hpp>

#include "MouseController.h"

constexpr float kRotateSensitivity = 0.005f;
constexpr float kPanSensitivity = 0.004f;
constexpr float kPitchLimitDegrees = 89.0f;
constexpr float kMinScale = 0.1f;
constexpr float kMaxScale = 10.0f;

constexpr float kFovDegrees = 45.0f;
constexpr float kNearPlane = 0.1f;
constexpr float kFarPlane = 100.0f;
constexpr float pitchLimit = glm::radians(kPitchLimitDegrees);

SceneView::SceneView(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& worldUp)
    : m_Eye(eye), m_Target(target), m_WorldUp(glm::normalize(worldUp))
{
    const glm::vec3 front = glm::normalize(m_Target - m_Eye);
    m_ScreenRight = glm::normalize(glm::cross(front, m_WorldUp));
    m_ScreenUp = glm::normalize(glm::cross(m_ScreenRight, front));
}

void SceneView::HandleMouse(bool overUI)
{
    UpdateDragState(overUI);
    ApplyDrag();

    if (MouseController::g_scrolled && !overUI)
    {
        m_Scale = glm::clamp(m_Scale * (1.0f + MouseController::g_scroll), kMinScale, kMaxScale);
    }

    MouseController::flush();
}

void SceneView::UpdateDragState(bool overUI)
{
    if (MouseController::g_leftDown && !m_PrevLeftDown)
    {
        m_DraggingRotate = !overUI;
    }
    if (!MouseController::g_leftDown)
    {
        m_DraggingRotate = false;
    }
    m_PrevLeftDown = MouseController::g_leftDown;

    if (MouseController::g_rightDown && !m_PrevRightDown)
    {
        m_DraggingPan = !overUI;
    }
    if (!MouseController::g_rightDown)
    {
        m_DraggingPan = false;
    }
    m_PrevRightDown = MouseController::g_rightDown;
}

void SceneView::ApplyDrag()
{
    if (m_DraggingRotate)
    {
        m_Yaw += MouseController::g_dx * kRotateSensitivity;
        m_Pitch = glm::clamp(m_Pitch + MouseController::g_dy * kRotateSensitivity, -pitchLimit, pitchLimit);
    }
    if (m_DraggingPan)
    {
        m_Pan += (MouseController::g_dx * m_ScreenRight + MouseController::g_dy * m_ScreenUp) * kPanSensitivity;
    }
}

glm::mat4 SceneView::ViewProjection(float aspect) const
{
    const glm::mat4 projection = glm::perspective(glm::radians(kFovDegrees), aspect, kNearPlane, kFarPlane);
    const glm::mat4 view = glm::lookAt(m_Eye, m_Target, m_WorldUp);

    glm::mat4 sceneTransform = glm::translate(glm::mat4(1.0f), m_Pan);
    sceneTransform = glm::scale(sceneTransform, glm::vec3(m_Scale));
    sceneTransform = glm::rotate(sceneTransform, m_Pitch, m_ScreenRight);
    sceneTransform = glm::rotate(sceneTransform, m_Yaw, m_WorldUp);

    return projection * view * sceneTransform;
}
