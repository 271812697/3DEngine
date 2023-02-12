//#define TESTMAIN

#ifdef TESTMAIN
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include<iostream>
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include<OvUI/Core/UIManager.h>
#include<OvUI/Panels/PanelWindow.h>
#include<OvUI/Panels/PanelMenuBar.h>
#include<OvUI/Panels/PanelUndecorated.h>
#include<OvUI/Widgets/Buttons/Button.h>
#include<OvUI/Widgets/InputFields/InputText.h>
#include<OvUI/Widgets/Layout/Spacing.h>
#include<OvUI/Widgets/Layout/Columns.h>
#include<OvUI/Widgets/Visual/Separator.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw  %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    // GL 4.6 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    glfwSwapInterval(1); // Enable vsync

    auto m_uiManager= std::make_unique<OvUI::Core::UIManager>(window, OvUI::Styling::EStyle::ALTERNATIVE_DARK);
    m_uiManager->EnableEditorLayoutSave(false);
    m_uiManager->EnableDocking(false);
    m_uiManager->LoadFont("Lato", "E:\\C++\\LearnGL_UI\\Resource\\font\\Lato.ttf", 18);
    m_uiManager->LoadFont("forkawesome-webfont", "E:\\C++\\LearnGL_UI\\Resource\\font\\forkawesome-webfont.ttf", 18);
    m_uiManager->LoadFont("palatino", "E:\\C++\\LearnGL_UI\\Resource\\font\\palatino.ttf", 18);
    m_uiManager->LoadFont("trebuc", "E:\\C++\\LearnGL_UI\\Resource\\font\\trebuc.ttf", 18);
    m_uiManager->UseFont("trebuc");
    OvUI::Modules::Canvas m_canvas;
    m_uiManager->SetCanvas(m_canvas);

    std::unique_ptr<OvUI::Panels::PanelWindow> m_mainPanel=std::make_unique<OvUI::Panels::PanelWindow>("Overload - Project Hub", true );
    m_mainPanel->resizable = false;
    m_mainPanel->movable = false;
    m_mainPanel->titleBar = false;
    m_mainPanel->SetSize({1000,580});
    m_mainPanel->SetPosition({0.f,30.f});
    auto& openProjectButton = m_mainPanel->CreateWidget<OvUI::Widgets::Buttons::Button>("Open Project");
    auto& newProjectButton = m_mainPanel->CreateWidget<OvUI::Widgets::Buttons::Button>("New Project");
    auto& pathField = m_mainPanel->CreateWidget<OvUI::Widgets::InputFields::InputText>("");

    openProjectButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
    openProjectButton.hoveredBackgroundColor= { 0.7f, 0.0f, 0.f };
    openProjectButton.clickedBackgroundColor = { 0.0f, 0.0f, 1.0f };
    openProjectButton.textColor = { 0.0f, 1.0f, 1.0f };
    newProjectButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };
    openProjectButton.ClickedEvent += []
    {
        if (ImGui::BeginPopupContextItem())
        {
                 std::cout << "openclick" << std::endl;
      ImGui::Button("test");
            ImGui::EndPopup();
        }
         

    };
    openProjectButton.lineBreak = false;
    newProjectButton.lineBreak = false;
    pathField.lineBreak = false;
    for (uint8_t i = 0; i < 4; ++i)
        m_mainPanel->CreateWidget<OvUI::Widgets::Layout::Spacing>(4);

    m_mainPanel->CreateWidget<OvUI::Widgets::Visual::Separator>();

    for (uint8_t i = 0; i < 4; ++i)
        m_mainPanel->CreateWidget<OvUI::Widgets::Layout::Spacing>();

    auto& columns = m_mainPanel->CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
    columns.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
    columns.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
    columns.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
    columns.CreateWidget<OvUI::Widgets::Buttons::Button>("New Project");

    columns.widths = {750, 500 };
    m_canvas.AddPanel(*m_mainPanel);
    std::unique_ptr<OvUI::Panels::PanelMenuBar> m_menuPanel = std::make_unique<OvUI::Panels::PanelMenuBar>();
    m_menuPanel->CreateWidget<OvUI::Widgets::Buttons::Button>("test").idleBackgroundColor = { 0.7f, 0.5f, 0.f };;
    m_canvas.AddPanel(*m_menuPanel);



#define TEST
    while (!glfwWindowShouldClose(window))
    {
#ifdef TEST
        glfwPollEvents();
        m_uiManager->Render();
        glfwSwapBuffers(window);
#else


        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if ( ImGui::BeginMainMenuBar())
        {
            ImGui::Button("test");

            ImGui::EndMainMenuBar();
        }
        ImVec4 col={1.0f,1.0f,0.0f,1.0f};
        ImGui::ColorButton("color##1000",col, ImGuiColorEditFlags_NoAlpha);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        glfwSwapBuffers(window);
#endif // TEST


    }
    
    m_uiManager.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif // TESTMAIN

