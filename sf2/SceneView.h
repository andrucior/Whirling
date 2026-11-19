#pragma once

#include <glm/glm.hpp>

class SceneView
{
public:
    SceneView(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& worldUp);

    void HandleMouse(bool overUI);

    glm::mat4 ViewProjection(float aspect) const;

private:
    void UpdateDragState(bool overUI);
    void ApplyDrag();

    glm::vec3 m_Eye;
    glm::vec3 m_Target;
    glm::vec3 m_WorldUp;

    glm::vec3 m_ScreenRight;
    glm::vec3 m_ScreenUp;

    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;
    float m_Scale = 1.0f;
    glm::vec3 m_Pan{0.0f};

    bool m_DraggingRotate = false;
    bool m_DraggingPan = false;
    bool m_PrevLeftDown = false;
    bool m_PrevRightDown = false;
};
