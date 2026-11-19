#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include <iostream>

class ShaderProgram
{
private:
    GLuint m_ProgramID = 0;

    mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;

    GLuint CompileShader(GLenum type, const std::string& source);
    GLint GetUniformLocation(const std::string& name) const;

public:
    ShaderProgram() = default;
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    bool CreateFromSources(const std::string& vertexSrc, const std::string& fragmentSrc);

    bool CreateFromSources(const std::string& vertexSrc, const std::string& tessControlSrc,
                           const std::string& tessEvalSrc, const std::string& fragmentSrc);

    void Use() const;
    void Unuse() const;
    GLuint GetID() const
    {
        return m_ProgramID;
    }

    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& matrix) const;
};
