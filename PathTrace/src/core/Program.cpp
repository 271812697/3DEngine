#include <stdexcept>
#include "Program.h"

namespace GLSLPT
{
    Program::Program(const std::vector<Shader> shaders)
    {
        object = glCreateProgram();
        for (unsigned i = 0; i < shaders.size(); i++)
            glAttachShader(object, shaders[i].getObject());

        glLinkProgram(object);
        for (unsigned i = 0; i < shaders.size(); i++)
            glDetachShader(object, shaders[i].getObject());
        GLint success = 0;
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            std::string msg("Error while linking program\n");
            GLint logSize = 0;
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logSize);
            char* info = new char[logSize + 1];
            glGetShaderInfoLog(object, logSize, NULL, info);
            msg += info;
            delete[] info;
            glDeleteProgram(object);
            object = 0;
            printf("Error %s\n", msg.c_str());
            throw std::runtime_error(msg.c_str());
        }
    }

    Program::~Program()
    {
        glDeleteProgram(object);
    }

    void Program::Use()
    {
        glUseProgram(object);
    }

    void Program::StopUsing()
    {
        glUseProgram(0);
    }

    GLuint Program::getObject()
    {
        return object;
    }
}