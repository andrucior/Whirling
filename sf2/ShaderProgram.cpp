#include "ShaderProgram.h"

ShaderProgram::~ShaderProgram()
{
    if (m_ProgramID != 0)
    {
        glDeleteProgram(m_ProgramID);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
{
    m_ProgramID = other.m_ProgramID;
    m_UniformLocationCache = std::move(other.m_UniformLocationCache);
    other.m_ProgramID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if (this != &other)
    {
        if (m_ProgramID != 0)
        {
            glDeleteProgram(m_ProgramID);
        }
        m_ProgramID = other.m_ProgramID;
        m_UniformLocationCache = std::move(other.m_UniformLocationCache);
        other.m_ProgramID = 0;
    }
    return *this;
}

GLuint ShaderProgram::CompileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

        std::string shaderTypeStr = "Unknown";
        if (type == GL_VERTEX_SHADER)
        {
            shaderTypeStr = "Vertex";
        }
        else if (type == GL_FRAGMENT_SHADER)
        {
            shaderTypeStr = "Fragment";
        }
        else if (type == GL_TESS_CONTROL_SHADER)
        {
            shaderTypeStr = "Tessellation Control (TCS)";
        }
        else if (type == GL_TESS_EVALUATION_SHADER)
        {
            shaderTypeStr = "Tessellation Evaluation (TES)";
        }

        std::cerr << "ERROR::SHADER::COMPILATION_FAILED [" << shaderTypeStr << "]:\n" << infoLog << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ShaderProgram::CreateFromSources(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    if (vs == 0 || fs == 0)
    {
        return false;
    }

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vs);
    glAttachShader(m_ProgramID, fs);
    glLinkProgram(m_ProgramID);

    int success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_ProgramID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED:\n" << infoLog << "\n";
        m_ProgramID = 0;
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

bool ShaderProgram::CreateFromSources(const std::string& vertexSrc, const std::string& tessControlSrc,
                                      const std::string& tessEvalSrc, const std::string& fragmentSrc)
{
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint tcs = CompileShader(GL_TESS_CONTROL_SHADER, tessControlSrc);
    GLuint tes = CompileShader(GL_TESS_EVALUATION_SHADER, tessEvalSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    if (vs == 0 || tcs == 0 || tes == 0 || fs == 0)
    {
        return false;
    }

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vs);
    glAttachShader(m_ProgramID, tcs);
    glAttachShader(m_ProgramID, tes);
    glAttachShader(m_ProgramID, fs);
    glLinkProgram(m_ProgramID);

    int success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_ProgramID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED (WITH TESSELLATION):\n" << infoLog << "\n";
        m_ProgramID = 0;
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(fs);
    return true;
}

void ShaderProgram::Use() const
{
    glUseProgram(m_ProgramID);
}

void ShaderProgram::Unuse() const
{
    glUseProgram(0);
}

GLint ShaderProgram::GetUniformLocation(const std::string& name) const
{
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
    {
        return it->second;
    }

    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    if (location == -1)
    {
        std::cerr << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << "\n";
    }
    m_UniformLocationCache[name] = location;
    return location;
}

void ShaderProgram::SetBool(const std::string& name, bool value) const
{
    glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void ShaderProgram::SetInt(const std::string& name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void ShaderProgram::SetFloat(const std::string& name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void ShaderProgram::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& matrix) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}