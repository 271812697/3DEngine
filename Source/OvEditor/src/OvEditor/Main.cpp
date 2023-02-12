#define EDITOR
#ifdef EDITOR
#include<iostream>
#include<memory>
#include <filesystem>
#include <OvTools/Utils/String.h>
#include "OvEditor/Core/Application.h"
#include<Opengl/core/log.h>
#undef APIENTRY
#include "Windows.h"


int main(int argc, char** argv)
{

    std::string projectPath, projectName;
    std::unique_ptr<OvEditor::Core::Application> app;
    ::core::Log::Init();

    try
    {

        app = std::make_unique<OvEditor::Core::Application>(projectPath, projectName);


        
    }
    catch (...) {}

    if (app)
        app->Run();
    ::core::Log::Shutdown();

	return EXIT_SUCCESS;
}


#endif // EDITOR

