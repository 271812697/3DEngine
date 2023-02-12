
#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace GLSLPT
{
    class ShaderInclude
    {
    public:

        struct ShaderSource
        {
            std::string src;
            std::string path;
        };

        // Return the source code of the complete shader
        static ShaderSource load(std::string path, std::string includeIndentifier = "#include")
        {
            includeIndentifier += ' ';
            static bool isRecursiveCall = false;

            std::string fullSourceCode = "";
            std::ifstream file(path);

            if (!file.is_open())
            {
                std::cerr << "ERROR: could not open the shader at: " << path << "\n" << std::endl;
                return ShaderSource{ fullSourceCode, path };
            }

            std::string lineBuffer;
            while (std::getline(file, lineBuffer))
            {
                // Look for the new shader include identifier
                if (lineBuffer.find(includeIndentifier) != lineBuffer.npos)
                {
                    // Remove the include identifier, this will cause the path to remain
                    lineBuffer.erase(0, includeIndentifier.size());

                    // The include path is relative to the current shader file path
                    std::string pathOfThisFile;
                    getFilePath(path, pathOfThisFile);
                    lineBuffer.insert(0, pathOfThisFile);

                    // By using recursion, the new include file can be extracted
                    // and inserted at this location in the shader source code
                    isRecursiveCall = true;
                    fullSourceCode += load(lineBuffer).src;

                    // Do not add this line to the shader source code, as the include
                    // path would generate a compilation issue in the final source code
                    continue;
                }

                fullSourceCode += lineBuffer + '\n';
            }

            // Only add the null terminator at the end of the complete file,
            // essentially skipping recursive function calls this way
            if (!isRecursiveCall)
                fullSourceCode += '\0';

            file.close();

            return ShaderSource{ fullSourceCode, path };;
        }



    private:
        static void getFilePath(const std::string& fullPath, std::string& pathWithoutFileName)
        {
            // Remove the file name and store the path to this folder
            size_t found = fullPath.find_last_of("/\\");
            pathWithoutFileName = fullPath.substr(0, found + 1);
        }
    };
}