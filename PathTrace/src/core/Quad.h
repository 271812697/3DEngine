#pragma once
#include "Config.h"
#include"Opengl/asset/shader.h"
namespace GLSLPT
{
    class Program;

    class Quad
    {
    public:
        Quad();
        void Draw(Program*);
        void Draw(asset::Shader* shader);

    private:
        GLuint vao;
        GLuint vbo;
    };
}