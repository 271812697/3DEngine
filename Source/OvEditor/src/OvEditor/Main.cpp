/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/
#include<iostream>
#include<memory>
#include <filesystem>
#include <OvTools/Utils/String.h>
#include "OvEditor/Core/Application.h"

#undef APIENTRY
#include "Windows.h"

int main(int argc, char** argv)
{

    std::string projectPath, projectName;
    std::unique_ptr<OvEditor::Core::Application> app;

    try
    {
        
        app = std::make_unique<OvEditor::Core::Application>(projectPath, projectName);
        
    }
    catch (...) {}

    if (app)
        app->Run();

	return EXIT_SUCCESS;
}

