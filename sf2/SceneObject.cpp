#include "SceneObject.h"

#include <set>
#include <utility>

SceneObject::SceneObject(const float* vertices, size_t vertexFloatCount, const unsigned int* indices, size_t indexCount,
                         bool drawAsLines, glm::vec3 pivot, const unsigned int* extraEdges, size_t extraEdgeCount)
    : m_Pivot(pivot)
{
    std::vector<unsigned int> edgeIndices;
    if (drawAsLines)
    {
        edgeIndices = QuadTrianglesToEdges(indices, indexCount);
        edgeIndices.insert(edgeIndices.end(), extraEdges, extraEdges + extraEdgeCount);
        indices = edgeIndices.data();
        indexCount = edgeIndices.size();
    }

    SetGeometry(vertices, vertexFloatCount, indices, indexCount, drawAsLines ? GL_LINES : GL_TRIANGLES, GL_STATIC_DRAW);
}

void SceneObject::EnsureBuffers()
{
    if (m_VAO == 0)
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
    }
}

void SceneObject::SetGeometry(const float* vertices, size_t vertexFloatCount, const unsigned int* indices,
                              size_t indexCount, GLenum drawMode, GLenum usage)
{
    EnsureBuffers();
    if (m_EBO == 0)
    {
        glGenBuffers(1, &m_EBO);
    }

    m_DrawMode = drawMode;
    m_DrawSource = DrawSource::Elements;
    m_IndexCount = static_cast<GLsizei>(indexCount);
    m_VertexCount = static_cast<GLsizei>(vertexFloatCount / 3);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexFloatCount * sizeof(float)), vertices, usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexCount * sizeof(unsigned int)), indices, usage);

    const GLsizei stride = 3 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SceneObject::ReserveVertices(size_t vertexFloatCapacity, GLenum drawMode, GLenum usage)
{
    EnsureBuffers();

    m_DrawMode = drawMode;
    m_DrawSource = DrawSource::Arrays;
    // Ewentualny EBO z wczesniejszego SetGeometry moze zostac - przy DrawSource::Arrays
    // jest po prostu ignorowany.
    m_IndexCount = 0;
    m_VertexCount = 0;

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // Jedna alokacja na pelna pojemnosc. Pozniej leci tylko glBufferSubData, wiec sterownik
    // nie realokuje (ani nie duplikuje) bufora w kazdej klatce.
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexFloatCapacity * sizeof(float)), nullptr, usage);

    // Format wierzcholka zapisuje sie w VAO raz - przy samej aktualizacji danych nie trzeba go powtarzac.
    const GLsizei stride = 3 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SceneObject::UpdateVertices(const float* vertices, size_t vertexFloatCount, size_t vertexFloatOffset) const
{
    if (vertexFloatCount == 0)
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(vertexFloatOffset * sizeof(float)),
                    static_cast<GLsizeiptr>(vertexFloatCount * sizeof(float)), vertices);
}

void SceneObject::SetVertexCount(size_t vertexCount)
{
    m_VertexCount = static_cast<GLsizei>(vertexCount);
}

SceneObject::~SceneObject()
{
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
    }
    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
    }
    if (m_EBO != 0)
    {
        glDeleteBuffers(1, &m_EBO);
    }
}

glm::mat4 SceneObject::GetModelMatrix() const
{
    glm::mat4 t = glm::translate(glm::mat4(1.0f), m_Position);
    glm::mat4 r = glm::mat4_cast(m_Rotation);
    glm::mat4 s = glm::scale(glm::mat4(1.0f), m_Scale);
    glm::mat4 p = glm::translate(glm::mat4(1.0f), -m_Pivot);
    return t * r * s * p;
}

void SceneObject::Draw(const ShaderProgram& shader, const glm::mat4& viewProjection) const
{
    const GLsizei count = m_DrawSource == DrawSource::Elements ? m_IndexCount : m_VertexCount;
    if (count == 0)
    {
        return;
    }

    glm::mat4 mvp = viewProjection * GetModelMatrix();
    shader.SetMat4("uMVP", mvp);

    glBindVertexArray(m_VAO);
    if (m_DrawSource == DrawSource::Elements)
    {
        glDrawElements(m_DrawMode, count, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(m_DrawMode, 0, count);
    }
    glBindVertexArray(0);
}

std::vector<unsigned int> SceneObject::QuadTrianglesToEdges(const unsigned int* indices, size_t count)
{
    std::set<std::pair<unsigned int, unsigned int>> unique;
    std::vector<unsigned int> edges;

    for (size_t i = 0; i + 5 < count; i += 6)
    {
        const unsigned int quad[4] = {indices[i], indices[i + 1], indices[i + 2], indices[i + 4]};

        for (int k = 0; k < 4; ++k)
        {
            unsigned int a = quad[k];
            unsigned int b = quad[(k + 1) % 4];
            if (a > b)
            {
                std::swap(a, b); // (1,5) i (5,1) to ta sama krawedz
            }

            if (unique.insert({a, b}).second)
            {
                edges.push_back(a);
                edges.push_back(b);
            }
        }
    }
    return edges;
}
