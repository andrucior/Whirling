#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "ShaderProgram.h"

class SceneObject
{
private:
    enum class DrawSource
    {
        Elements,
        Arrays,
    };

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
    GLsizei m_IndexCount = 0;
    GLsizei m_VertexCount = 0;
    GLenum m_DrawMode = GL_TRIANGLES;
    DrawSource m_DrawSource = DrawSource::Elements;

    glm::vec3 m_Position{0.0f, 0.0f, 0.0f};
    glm::quat m_Rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 m_Scale{1.0f, 1.0f, 1.0f};
    glm::vec3 m_Pivot{0.0f};

public:
    SceneObject() = default;
    SceneObject(const float* vertices, size_t vertexFloatCount, const unsigned int* indices, size_t indexCount,
                bool drawAsLines = false, glm::vec3 pivot = glm::vec3{0.0f}, const unsigned int* extraEdges = nullptr,
                size_t extraEdgeCount = 0);
    virtual ~SceneObject();

    void SetPosition(const glm::vec3& position)
    {
        m_Position = position;
    }
    const glm::vec3& GetPosition() const
    {
        return m_Position;
    }
    void Translate(const glm::vec3& delta)
    {
        m_Position += delta;
    }

    void SetRotation(const glm::quat& rotation)
    {
        m_Rotation = rotation;
    }
    const glm::quat& GetRotation() const
    {
        return m_Rotation;
    }
    void Rotate(float angleRadians, const glm::vec3& axis)
    {
        m_Rotation = glm::normalize(glm::angleAxis(angleRadians, glm::normalize(axis)) * m_Rotation);
    }

    void SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
    }
    void SetScale(float uniformScale)
    {
        m_Scale = glm::vec3(uniformScale);
    }
    const glm::vec3& GetScale() const
    {
        return m_Scale;
    }

    glm::mat4 GetModelMatrix() const;

    void Draw(const ShaderProgram& shader, const glm::mat4& viewProjection) const;

protected:
    void SetGeometry(const float* vertices, size_t vertexFloatCount, const unsigned int* indices, size_t indexCount,
                      GLenum drawMode, GLenum usage = GL_STATIC_DRAW);

    void ReserveVertices(size_t vertexFloatCapacity, GLenum drawMode, GLenum usage = GL_DYNAMIC_DRAW);

    void UpdateVertices(const float* vertices, size_t vertexFloatCount, size_t vertexFloatOffset) const;

    void SetVertexCount(size_t vertexCount);

private:
    void EnsureBuffers();

    static std::vector<unsigned int> QuadTrianglesToEdges(const unsigned int* indices, size_t count);
};
