#define TESTMAIN

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

#include"ImGuizmo.h"
#include "src/core/Scene.h"
#include "src/loaders/Loader.h"
#include "src/loaders/GLTFLoader.h"
#include "src/core/Renderer.h"
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"


using namespace std;
using namespace GLSLPT;

Scene* scene = nullptr;
Renderer* renderer = nullptr;

std::vector<string> sceneFiles;
std::vector<string> envMaps;

float mouseSensitivity = 0.01f;
bool keyPressed = false;
int sampleSceneIdx = 0;
int selectedInstance = 0;
double lastTime = 0.0;
int envMapIdx = 0;
bool done = false;

std::string shadersDir = "../../PathTrace/src/shaders/";
std::string assetsDir = "../../PathTrace/assets/";
std::string envMapDir = "../../PathTrace/assets/HDR/";

RenderOptions renderOptions;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw  %d: %s\n", error, description);
}
void GetSceneFiles()
{
    std::filesystem::directory_entry p_directory(assetsDir);
    for (auto& item : std::filesystem::directory_iterator(p_directory))
        if (!item.is_directory()) {
            auto ext = item.path().extension();
            if (ext == ".scene" || ext == ".gltf" || ext == ".glb")
            {
                sceneFiles.push_back(item.path().generic_string());
            }
        }
}
void GetEnvMaps()
{
    std::filesystem::directory_entry p_directory(envMapDir);
    for (auto& item : std::filesystem::directory_iterator(p_directory)) {
        if (item.path().extension() == ".hdr")
        {
            envMaps.push_back(item.path().generic_string());

        }
    }
}

void LoadScene(std::string sceneName)
{
    delete scene;
    scene = new Scene();
    std::string ext = sceneName.substr(sceneName.find_last_of(".") + 1);

    bool success = false;
    Mat4 xform;

    if (ext == "scene")
        success = LoadSceneFromFile(sceneName, scene, renderOptions);
    else if (ext == "gltf")
        success = LoadGLTF(sceneName, scene, renderOptions, xform, false);
    else if (ext == "glb")
        success = LoadGLTF(sceneName, scene, renderOptions, xform, true);

    if (!success)
    {
        printf("Unable to load scene\n");
        exit(0);
    }

    selectedInstance = 0;
    // Add a default HDR if there are no lights in the scene
    if (!scene->envMap && !envMaps.empty())
    {
        scene->AddEnvMap(envMaps[envMapIdx]);
        renderOptions.enableEnvMap = scene->lights.empty() ? true : false;
        renderOptions.envMapIntensity = 1.5f;
    }

    scene->renderOptions = renderOptions;
}

bool InitRenderer()
{
    delete renderer;
    renderer = new Renderer(scene, shadersDir);
    return true;
}
void SaveFrame(const std::string filename)
{
    unsigned char* data = nullptr;
    int w, h;
    renderer->GetOutputBuffer(&data, w, h);
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filename.c_str(), w, h, 4, data, w * 4);
    printf("Frame saved: %s\n", filename.c_str());
    delete[] data;
}

void Render()
{
    renderer->Render();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderOptions.windowResolution.x, renderOptions.windowResolution.y);
    renderer->Present();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Update(float secondsElapsed)
{
    keyPressed = false;

    //相机视角交互逻辑
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && ImGui::IsAnyMouseDown() && !ImGuizmo::IsOver())
    {
        if (ImGui::IsMouseDown(0))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(0, 0);
            scene->camera->OffsetOrientation(mouseDelta.x, mouseDelta.y);
            ImGui::ResetMouseDragDelta(0);
        }
        else if (ImGui::IsMouseDown(1))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(1, 0);
            scene->camera->SetRadius(mouseSensitivity * mouseDelta.y);
            ImGui::ResetMouseDragDelta(1);
        }
        else if (ImGui::IsMouseDown(2))
        {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(2, 0);
            scene->camera->Strafe(mouseSensitivity * mouseDelta.x, mouseSensitivity * mouseDelta.y);
            ImGui::ResetMouseDragDelta(2);
        }
        scene->dirty = true;
    }

    renderer->Update(secondsElapsed);
}

void EditTransform(const float* view, const float* projection, float* matrix)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
    {
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }

    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
    {
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    }

    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
    {
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    }

    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
        {
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
        {
            mCurrentGizmoMode = ImGuizmo::WORLD;
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(view, projection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, NULL);
}
void MainLoop(void* arg)
{


    //渲染ui

    ImGuizmo::SetOrthographic(false);

    ImGuizmo::BeginFrame();
    {
        ImGui::Begin("Settings");

        ImGui::Text("Samples: %d ", renderer->GetSampleCount());

        ImGui::BulletText("LMB + drag to rotate");
        ImGui::BulletText("MMB + drag to pan");
        ImGui::BulletText("RMB + drag to zoom in/out");
        ImGui::BulletText("CTRL + click on a slider to edit its value");

        if (ImGui::Button("Save Screenshot"))
        {
            SaveFrame("./img_" + to_string(renderer->GetSampleCount()) + ".png");
        }

        // Scenes
        std::vector<const char*> scenes;
        for (int i = 0; i < sceneFiles.size(); ++i)
            scenes.push_back(sceneFiles[i].c_str());

        //场景切换逻辑
        if (ImGui::Combo("Scene", &sampleSceneIdx, scenes.data(), scenes.size()))
        {
            int w = renderOptions.windowResolution.x;
            int h= renderOptions.windowResolution.y;
           
            LoadScene(sceneFiles[sampleSceneIdx]);
            renderOptions.windowResolution.x = w;
            renderOptions.windowResolution.y =h;
            InitRenderer();
        }

        // Environment maps
        std::vector<const char*> envMapsList;
        for (int i = 0; i < envMaps.size(); ++i)
            envMapsList.push_back(envMaps[i].c_str());

        if (ImGui::Combo("EnvMaps", &envMapIdx, envMapsList.data(), envMapsList.size()))
        {
            scene->AddEnvMap(envMaps[envMapIdx]);
        }

        bool optionsChanged = false;
        bool reloadShaders = false;

        optionsChanged |= ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.001f, 1.0f);

        if (ImGui::CollapsingHeader("Render Settings"))
        {
            optionsChanged |= ImGui::SliderInt("Max Spp", &renderOptions.maxSpp, -1, 256);
            optionsChanged |= ImGui::SliderInt("Max Depth", &renderOptions.maxDepth, 1, 10);

            reloadShaders |= ImGui::Checkbox("Enable Russian Roulette", &renderOptions.enableRR);
            reloadShaders |= ImGui::SliderInt("Russian Roulette Depth", &renderOptions.RRDepth, 1, 10);
            reloadShaders |= ImGui::Checkbox("Enable Roughness Mollification", &renderOptions.enableRoughnessMollification);
            optionsChanged |= ImGui::SliderFloat("Roughness Mollification Amount", &renderOptions.roughnessMollificationAmt, 0, 1);
            reloadShaders |= ImGui::Checkbox("Enable Volume MIS", &renderOptions.enableVolumeMIS);
        }

        if (ImGui::CollapsingHeader("Environment"))
        {
            reloadShaders |= ImGui::Checkbox("Enable Uniform Light", &renderOptions.enableUniformLight);

            Vec3 uniformLightCol = Vec3::Pow(renderOptions.uniformLightCol, 1.0 / 2.2);
            optionsChanged |= ImGui::ColorEdit3("Uniform Light Color (Gamma Corrected)", (float*)(&uniformLightCol), 0);
            renderOptions.uniformLightCol = Vec3::Pow(uniformLightCol, 2.2);

            reloadShaders |= ImGui::Checkbox("Enable Environment Map", &renderOptions.enableEnvMap);
            optionsChanged |= ImGui::SliderFloat("Enviornment Map Intensity", &renderOptions.envMapIntensity, 0.1f, 10.0f);
            optionsChanged |= ImGui::SliderFloat("Enviornment Map Rotation", &renderOptions.envMapRot, 0.0f, 360.0f);
            reloadShaders |= ImGui::Checkbox("Hide Emitters", &renderOptions.hideEmitters);
            reloadShaders |= ImGui::Checkbox("Enable Background", &renderOptions.enableBackground);
            optionsChanged |= ImGui::ColorEdit3("Background Color", (float*)&renderOptions.backgroundCol, 0);
            reloadShaders |= ImGui::Checkbox("Transparent Background", &renderOptions.transparentBackground);
        }

        if (ImGui::CollapsingHeader("Tonemapping"))
        {
            ImGui::Checkbox("Enable Tonemap", &renderOptions.enableTonemap);

            if (renderOptions.enableTonemap)
            {
                ImGui::Checkbox("Enable ACES", &renderOptions.enableAces);
                if (renderOptions.enableAces)
                    ImGui::Checkbox("Simple ACES Fit", &renderOptions.simpleAcesFit);
            }
        }

        if (ImGui::CollapsingHeader("Denoiser"))
        {

            ImGui::Checkbox("Enable Denoiser", &renderOptions.enableDenoiser);
            ImGui::SliderInt("Number of Frames to skip", &renderOptions.denoiserFrameCnt, 5, 50);
        }

        if (ImGui::CollapsingHeader("Camera"))
        {
            float fov = Math::Degrees(scene->camera->fov);
            float aperture = scene->camera->aperture * 1000.0f;
            optionsChanged |= ImGui::SliderFloat("Fov", &fov, 10, 90);
            scene->camera->SetFov(fov);
            optionsChanged |= ImGui::SliderFloat("Aperture", &aperture, 0.0f, 10.8f);
            scene->camera->aperture = aperture / 1000.0f;
            optionsChanged |= ImGui::SliderFloat("Focal Distance", &scene->camera->focalDist, 0.01f, 50.0f);
            ImGui::Text("Pos: %.2f, %.2f, %.2f", scene->camera->position.x, scene->camera->position.y, scene->camera->position.z);
        }

        if (ImGui::CollapsingHeader("Objects"))
        {
            bool objectPropChanged = false;

            std::vector<std::string> listboxItems;
            for (int i = 0; i < scene->meshInstances.size(); i++)
            {
                listboxItems.push_back(scene->meshInstances[i].name);
            }

            // Object Selection
            if (ImGui::ListBoxHeader("Instances")) {
                for (int i = 0; i < scene->meshInstances.size(); i++)
                {
                    bool is_selected = selectedInstance == i;
                    if (ImGui::Selectable(listboxItems[i].c_str(), is_selected))
                    {
                        selectedInstance = i;
                    }
                }
                ImGui::ListBoxFooter();
            }
            ImGui::Separator();
            ImGui::Text("Materials");
            // Material Properties
            Material* mat = &scene->materials[scene->meshInstances[selectedInstance].materialID];
            // Gamma correction for color picker. Internally, the renderer uses linear RGB values for colors
            Vec3 albedo = Vec3::Pow(mat->baseColor, 1.0 / 2.2);
            objectPropChanged |= ImGui::ColorEdit3("Albedo (Gamma Corrected)", (float*)(&albedo), 0);
            mat->baseColor = Vec3::Pow(albedo, 2.2);

            objectPropChanged |= ImGui::SliderFloat("Metallic", &mat->metallic, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("Roughness", &mat->roughness, 0.001f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("SpecularTint", &mat->specularTint, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("Subsurface", &mat->subsurface, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("Anisotropic", &mat->anisotropic, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("Sheen", &mat->sheen, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("SheenTint", &mat->sheenTint, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("Clearcoat", &mat->clearcoat, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("ClearcoatGloss", &mat->clearcoatGloss, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("SpecTrans", &mat->specTrans, 0.0f, 1.0f);
            objectPropChanged |= ImGui::SliderFloat("Ior", &mat->ior, 1.001f, 2.0f);

            int mediumType = (int)mat->mediumType;
            if (ImGui::Combo("Medium Type", &mediumType, "None\0Absorb\0Scatter\0Emissive\0"))
            {
                reloadShaders = true;
                objectPropChanged = true;
                mat->mediumType = mediumType;
            }

            if (mediumType != MediumType::None)
            {
                Vec3 mediumColor = Vec3::Pow(mat->mediumColor, 1.0 / 2.2);
                objectPropChanged |= ImGui::ColorEdit3("Medium Color (Gamma Corrected)", (float*)(&mediumColor), 0);
                mat->mediumColor = Vec3::Pow(mediumColor, 2.2);

                objectPropChanged |= ImGui::SliderFloat("Medium Density", &mat->mediumDensity, 0.0f, 5.0f);

                if (mediumType == MediumType::Scatter)
                    objectPropChanged |= ImGui::SliderFloat("Medium Anisotropy", &mat->mediumAnisotropy, -0.9f, 0.9f);
            }

            int alphaMode = (int)mat->alphaMode;
            if (ImGui::Combo("Alpha Mode", &alphaMode, "Opaque\0Blend"))
            {
                reloadShaders = true;
                objectPropChanged = true;
                mat->alphaMode = alphaMode;
            }

            if (alphaMode != AlphaMode::Opaque)
                objectPropChanged |= ImGui::SliderFloat("Opacity", &mat->opacity, 0.0f, 1.0f);

            // Transforms
            ImGui::Separator();
            ImGui::Text("Transforms");
            {
                float viewMatrix[16];
                float projMatrix[16];

                auto io = ImGui::GetIO();
                scene->camera->ComputeViewProjectionMatrix(viewMatrix, projMatrix, io.DisplaySize.x / io.DisplaySize.y);
                Mat4 xform = scene->meshInstances[selectedInstance].transform;

                EditTransform(viewMatrix, projMatrix, (float*)&xform);

                if (memcmp(&xform, &scene->meshInstances[selectedInstance].transform, sizeof(float) * 16))
                {
                    scene->meshInstances[selectedInstance].transform = xform;
                    objectPropChanged = true;
                }
            }

            if (objectPropChanged)
                scene->RebuildInstances();
        }

        scene->renderOptions = renderOptions;

        if (optionsChanged)
            scene->dirty = true;

        if (reloadShaders)
        {
            scene->dirty = true;
            renderer->ReloadShaders();
        }

        ImGui::End();

        
    }
    static double lasttime= glfwGetTime();
    static double curtime = glfwGetTime();

    curtime= glfwGetTime();

   
    Update((float)(curtime-lasttime));
    lastTime = curtime;
    glClearColor(0., 0., 0., 0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    Render();
   
}


int main(int, char**)
{
    GetSceneFiles();
    GetEnvMaps();
    LoadScene(sceneFiles[sampleSceneIdx]);
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    // GL 4.6 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow* window = glfwCreateWindow(renderOptions.windowResolution.x, renderOptions.windowResolution.y, "GLSLPathTrace", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    glfwSwapInterval(1); // Enable vsync
    auto resizeCallback = [](GLFWwindow* p_window, int p_width, int p_height)
    {
      
        renderOptions.windowResolution.x = p_width;
        renderOptions.windowResolution.y = p_height;

        if (!renderOptions.independentRenderSize)
            renderOptions.renderResolution = renderOptions.windowResolution;

        scene->renderOptions = renderOptions;
        renderer->ResizeRenderer();
    };

    glfwSetWindowSizeCallback(window, resizeCallback);

    auto m_uiManager= std::make_unique<OvUI::Core::UIManager>(window, OvUI::Styling::EStyle::ALTERNATIVE_DARK);
    m_uiManager->EnableEditorLayoutSave(false);
    m_uiManager->EnableDocking(false);
    m_uiManager->LoadFont("Lato", "E:\\C++\\LearnGL_UI\\Resource\\font\\Lato.ttf", 18);
    m_uiManager->LoadFont("forkawesome-webfont", "E:\\C++\\LearnGL_UI\\Resource\\font\\forkawesome-webfont.ttf", 18);
    m_uiManager->LoadFont("palatino", "E:\\C++\\LearnGL_UI\\Resource\\font\\palatino.ttf", 18);
    m_uiManager->LoadFont("trebuc", "E:\\C++\\LearnGL_UI\\Resource\\font\\trebuc.ttf", 18);
    m_uiManager->UseFont("trebuc");
    if (!InitRenderer())
        return 1;
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

        MainLoop(nullptr);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
#endif // TEST
    }
    

    delete renderer;
    delete scene;
    m_uiManager.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif // TESTMAIN

