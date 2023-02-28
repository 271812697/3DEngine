/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Plugins/DDTarget.h>
#include "OvEditor/Core/EditorRenderer.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Panels/SceneView.h"
#include "OvEditor/Panels/GameView.h"
#include "OvEditor/Settings/EditorSettings.h"
#include "Opengl//asset/shader.h"
#include "OvRendering/Resources/Texture2D.h"

std::shared_ptr<asset::Shader>skys;
std::shared_ptr<OvRendering::Resources::Texture2D>env_map;
std::shared_ptr<asset::CShader>bloom_shader;


OvEditor::Panels::SceneView::SceneView
(
    const std::string& p_title,
    bool p_opened,
    const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : AViewControllable(p_title, p_opened, p_windowSettings, true),
m_sceneManager(EDITOR_CONTEXT(sceneManager))

{
    m_mulfbo = std::make_unique<OvRendering::Buffers::Framebuffer>(1, 1);
    m_mulfbo->AddColorTexture(2, true);
    m_mulfbo->AddDepStRenderBuffer(true);

    m_resfbo = std::make_unique<OvRendering::Buffers::Framebuffer>(1, 1);
    m_resfbo->AddColorTexture(2);

    m_bloomfbo = std::make_unique<OvRendering::Buffers::Framebuffer>(1, 1);
    m_bloomfbo->AddColorTexture(2);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    std::tie(irradiance_map, prefiltered_map, BRDF_LUT) =
        Ext::PrecomputeIBL("Resource\\texture\\HDRI\\Field-Path-Fence-Steinbacher-Street-4K.hdr");
    skys = std::make_shared<asset::Shader>("Resource\\Shader\\skybox01.glsl");
    skys->SetUniform(0, 1.0f);
    skys->SetUniform(1, 0.0f);

    glBindTextureUnit(0, irradiance_map->ID());
    glBindTextureUnit(1, prefiltered_map->ID());
    glBindTextureUnit(2, BRDF_LUT->ID());

    m_actorPickingFramebuffer = std::make_unique<OvRendering::Buffers::Framebuffer>(1, 1);
    m_actorPickingFramebuffer->AddColorTexture(1);
    m_actorPickingFramebuffer->AddDepStRenderBuffer();
    m_camera.SetClearColor({ 0.098f, 0.098f, 0.098f });
    m_camera.SetFar(5000.0f);
    m_image->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [this](auto p_data)
    {
        std::string path = p_data.first;

        switch (OvTools::Utils::PathParser::GetFileType(path))
        {
        case OvTools::Utils::PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
        case OvTools::Utils::PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
        }
    };
    ;

    bloom_shader = std::make_shared<asset::CShader>("Resource\\Shader\\bloom.glsl");
}

void OvEditor::Panels::SceneView::Update(float p_deltaTime)
{
    AViewControllable::Update(p_deltaTime);
    PrepareCamera();

    using namespace OvWindowing::Inputs;

    if (IsFocused() && !m_cameraController.IsRightMousePressed())
    {
        if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_W))
        {
            m_currentOperation = ImGuizmo::TRANSLATE;;
        }

        if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_E))
        {
            m_currentOperation = ImGuizmo::ROTATE;
        }

        if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_R))
        {
            m_currentOperation = ImGuizmo::SCALE;
        }
    }
}

void OvEditor::Panels::SceneView::_Render_Impl()
{


    auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();

    uint8_t glState = baseRenderer.FetchGLState();
    baseRenderer.ApplyStateMask(glState);
    HandleActorPicking();
    baseRenderer.ApplyStateMask(glState);
    RenderScene(glState);
    baseRenderer.ApplyStateMask(glState);
}

void OvEditor::Panels::SceneView::RenderScene(uint8_t p_defaultRenderState)
{

    auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();
    auto& currentScene = *m_sceneManager.GetCurrentScene();
    auto& gameView = EDITOR_PANEL(OvEditor::Panels::GameView, "Game View");

    // If the game is playing, and ShowLightFrustumCullingInSceneView is true, apply the game view frustum culling to the scene view (For debugging purposes)
    if (auto gameViewFrustum = gameView.GetActiveFrustum(); gameViewFrustum.has_value() && gameView.GetCamera().HasFrustumLightCulling() && Settings::EditorSettings::ShowLightFrustumCullingInSceneView)
    {
        m_editorRenderer.UpdateLightsInFrustum(currentScene, gameViewFrustum.value());
    }
    else
    {
        m_editorRenderer.UpdateLights(currentScene);
    }

    auto [winWidth, winHeight] = GetSafeSize();

    m_resfbo->Resize(winWidth, winHeight);
    m_bloomfbo->Resize(winWidth / 2, winHeight / 2);
    m_mulfbo->Resize(winWidth, winHeight);
    //m_fbo.Bind();
    m_mulfbo->Bind();
    baseRenderer.SetStencilMask(0xFF);
    baseRenderer.Clear(m_camera);
    baseRenderer.SetStencilMask(0x00);
    baseRenderer.SetCapability(OvRendering::Settings::ERenderingCapability::MULTISAMPLE, true);

    skys->Bind();
    auto model = EDITOR_CONTEXT(editorResources)->GetModel("Cube")->GetMeshes();
    for (auto mesh : model) {
        mesh->Bind();
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        mesh->Unbind();
    }
    skys->Unbind();





    m_editorRenderer.RenderGrid(m_cameraPosition, m_gridColor);
    m_editorRenderer.RenderCameras();

    // If the game is playing, and ShowGeometryFrustumCullingInSceneView is true, apply the game view frustum culling to the scene view (For debugging purposes)
    if (auto gameViewFrustum = gameView.GetActiveFrustum(); gameViewFrustum.has_value() && gameView.GetCamera().HasFrustumGeometryCulling() && Settings::EditorSettings::ShowGeometryFrustumCullingInSceneView)
    {
        m_camera.SetFrustumGeometryCulling(gameView.HasCamera() ? gameView.GetCamera().HasFrustumGeometryCulling() : false);
        m_editorRenderer.RenderScene(m_cameraPosition, m_camera, &gameViewFrustum.value());
        m_camera.SetFrustumGeometryCulling(false);
    }
    else
    {
        m_editorRenderer.RenderScene(m_cameraPosition, m_camera);
    }

    m_editorRenderer.RenderLights();

    if (EDITOR_EXEC(IsAnyActorSelected()))
    {
        auto& selectedActor = EDITOR_EXEC(GetSelectedActor());

        if (selectedActor.IsActive())
        {
            m_editorRenderer.RenderActorOutlinePass(selectedActor, true, true);
            baseRenderer.ApplyStateMask(p_defaultRenderState);
            m_editorRenderer.RenderActorOutlinePass(selectedActor, false, true);
        }
    }

    if (m_highlightedActor.has_value())
    {
        m_editorRenderer.RenderActorOutlinePass(m_highlightedActor.value().get(), true, false);
        baseRenderer.ApplyStateMask(p_defaultRenderState);
        m_editorRenderer.RenderActorOutlinePass(m_highlightedActor.value().get(), false, false);
    }


    m_mulfbo->Unbind();
    //MSAA pass
    OvRendering::Buffers::Framebuffer::CopyColor(*m_mulfbo.get(), 0, *m_resfbo.get(), 0);
    OvRendering::Buffers::Framebuffer::CopyColor(*m_mulfbo.get(), 1, *m_resfbo.get(), 1);
    //bloom pass
    OvRendering::Buffers::Framebuffer::CopyColor(*m_resfbo.get(), 1, *m_bloomfbo.get(), 0);

    //postprocess pass


    //m_fbo->Bind();
    OvRendering::Buffers::Framebuffer::CopyColor(*m_resfbo.get(), 0, *m_fbo.get(), 0);
    /*
        auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();
        auto& currentScene = *m_sceneManager.GetCurrentScene();



        auto [winWidth, winHeight] = GetSafeSize();

        m_resfbo->Resize(winWidth, winHeight);
        m_bloomfbo->Resize(winWidth/2, winHeight/2);
        m_mulfbo->Resize(winWidth, winHeight);
        //m_fbo.Bind();
        m_mulfbo->Bind();
        baseRenderer.SetStencilMask(0xFF);
        baseRenderer.Clear(m_camera);
        baseRenderer.SetStencilMask(0x00);
        baseRenderer.SetCapability(OvRendering::Settings::ERenderingCapability::MULTISAMPLE, true);


        m_editorRenderer.RenderLights();



        m_mulfbo->Unbind();
        //MSAA pass
        OvRendering::Buffers::Framebuffer::CopyColor(*m_mulfbo.get(), 0, *m_resfbo.get(), 0);
        OvRendering::Buffers::Framebuffer::CopyColor(*m_mulfbo.get(), 1, *m_resfbo.get(), 1);
        //bloom pass
        OvRendering::Buffers::Framebuffer::CopyColor(*m_resfbo.get(), 1, *m_bloomfbo.get(), 0);
        auto ping = m_bloomfbo->GetTextureID(0);
        auto pong = m_bloomfbo->GetTextureID(1);
        bloom_shader->Bind();
        glBindImageTexture(0, ping, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindImageTexture(1, pong, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
        for(int i = 0; i < 6; ++i) {
            bloom_shader->SetUniform(0, i % 2 == 0);
            bloom_shader->Dispatch(winWidth / 64, winHeight / 36);
            bloom_shader->SyncWait(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }

        //postprocess pass


        //m_fbo->Bind();


        OvRendering::Buffers::Framebuffer::CopyColor(*m_bloomfbo.get(), 0, *m_fbo.get(), 0);


    */
}

void OvEditor::Panels::SceneView::RenderSceneForActorPicking()
{
    auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();
    auto [winWidth, winHeight] = GetSafeSize();
    m_actorPickingFramebuffer->Resize(winWidth, winHeight);
    m_actorPickingFramebuffer->Bind();
    baseRenderer.SetClearColor(1.0f, 1.0f, 1.0f);
    baseRenderer.Clear();
    m_editorRenderer.RenderSceneForActorPicking();
    m_actorPickingFramebuffer->Unbind();
}
void OvEditor::Panels::SceneView::_Draw_ImplInWindow() {
    if (EDITOR_EXEC(IsAnyActorSelected()))
    {
        auto& selectedActor = EDITOR_EXEC(GetSelectedActor());
        OvMaths::FMatrix4 model = selectedActor.transform.GetWorldMatrix();
        model = OvMaths::FMatrix4::Transpose(model);
        // ImGuizmo::BeginFrame();
        auto pos = this->GetPosition();
        auto [winWidth, winHeight] = GetSafeSize();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(pos.x, pos.y+25, winWidth, winHeight);
     
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::Manipulate(OvMaths::FMatrix4::Transpose(m_camera.GetViewMatrix()).data,
            OvMaths::FMatrix4::Transpose(m_camera.GetProjectionMatrix()).data, static_cast<ImGuizmo::OPERATION>(m_currentOperation), ImGuizmo::WORLD,
            model.data);
        OvMaths::FVector3 p, r, s;
        ImGuizmo::DecomposeMatrixToComponents(model.data, (float*) & p,(float*) & r, (float*) & s);
        selectedActor.transform.GetFTransform().GenerateMatrices(p, r, s);

    }
}
bool IsResizing()
{
    auto cursor = ImGui::GetMouseCursor();

    return
        cursor == ImGuiMouseCursor_ResizeEW ||
        cursor == ImGuiMouseCursor_ResizeNS ||
        cursor == ImGuiMouseCursor_ResizeNWSE ||
        cursor == ImGuiMouseCursor_ResizeNESW ||
        cursor == ImGuiMouseCursor_ResizeAll;;
}
void OvEditor::Panels::SceneView::HandleActorPicking()
{
    using namespace OvWindowing::Inputs;
    auto& inputManager = *EDITOR_CONTEXT(inputManager);
    if (IsHovered() && !IsResizing())
    {
        RenderSceneForActorPicking();
        // Look actor under mouse
        auto [mouseX, mouseY] = inputManager.GetMousePosition();
        mouseX -= m_position.x;
        mouseY -= m_position.y;
        mouseY = GetSafeSize().second - mouseY + 25;
        m_actorPickingFramebuffer->Bind();
        uint8_t pixel[3];
        EDITOR_CONTEXT(renderer)->ReadPixels(static_cast<int>(mouseX), static_cast<int>(mouseY), 1, 1, OvRendering::Settings::EPixelDataFormat::RGB, OvRendering::Settings::EPixelDataType::UNSIGNED_BYTE, pixel);
        m_actorPickingFramebuffer->Unbind();
        uint32_t actorID = (0 << 24) | (pixel[2] << 16) | (pixel[1] << 8) | (pixel[0] << 0);
        auto actorUnderMouse = EDITOR_CONTEXT(sceneManager).GetCurrentScene()->FindActorByID(actorID);
        /* Click */
        if (inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_LEFT) && !m_cameraController.IsRightMousePressed())
        {
                if (actorUnderMouse)
                {
                    EDITOR_EXEC(SelectActor(*actorUnderMouse));
                }
    
        }
        if(inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_RIGHT)&&!actorUnderMouse)
        {
            EDITOR_EXEC(UnselectActor());
        }
        m_highlightedActor={};
        if (actorUnderMouse) {
            m_highlightedActor = *actorUnderMouse;
        }
    }
}
