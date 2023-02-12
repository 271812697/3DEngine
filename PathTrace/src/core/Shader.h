

#pragma once

#include <string>
#include "ShaderIncludes.h"
#include "Config.h"

namespace GLSLPT
{
    class Shader
    {
    private:
        GLuint object;
    public:
        Shader(const ShaderInclude::ShaderSource& sourceObj, GLuint shaderType);
        GLuint getObject() const;
    };
}