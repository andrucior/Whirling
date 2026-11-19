#pragma once
#include "ShaderProgram.h"

class SceneShader : public ShaderProgram
{
private:
    static const inline char* kVertexShaderSrc = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
        )glsl";

    static const inline char* kFragmentShaderSrc = R"glsl(
        #version 330 core

        out vec4 FragColor;

        uniform float uAlpha;
        uniform vec3 uColor;

        void main()
        {
            FragColor = vec4(uColor, uAlpha);
        }
        )glsl";

public:
    bool Create()
    {
        return CreateFromSources(kVertexShaderSrc, kFragmentShaderSrc);
    }

    void SetAlpha(float alpha) const
    {
        SetFloat("uAlpha", alpha);
    }

    void SetColor(glm::vec3 color) const
    {
        SetVec3("uColor", color);
    }
};
