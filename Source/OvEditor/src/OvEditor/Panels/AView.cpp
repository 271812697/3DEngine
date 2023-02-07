#include "OvEditor/Panels/AView.h"
OvEditor::Panels::AView::AView
(
    const std::string& p_title,
    bool p_opened,
    const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
  
    m_fbo = std::make_shared<asset::FBO>(1280,720);
    m_fbo->AddColorTexture(1);
    m_fbo->AddDepStRenderBuffer();
   
    int id = m_fbo->GetColorTexture(0).ID();
    m_image = &CreateWidget<OvUI::Widgets::Visual::Image>(id, OvMaths::FVector2{0.f, 0.f});

    scrollable = false;
}

void OvEditor::Panels::AView::Update(float p_deltaTime)
{
    auto [winWidth, winHeight] = GetSafeSize();
    static uint16_t w = 0;
    static uint16_t h = 0;
    if (w != winWidth ||h!= winHeight) {
        w = winWidth;
        h = winHeight;
        m_image->size = OvMaths::FVector2(static_cast<float>(winWidth), static_cast<float>(winHeight));
       if(w>0&&h>0) m_fbo->Resize(w,h);
    }

   
    
    
    //m_fbo.Resize(winWidth, winHeight);
}

void OvEditor::Panels::AView::_Draw_Impl()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    OvUI::Panels::PanelWindow::_Draw_Impl();

    ImGui::PopStyleVar();
}

void OvEditor::Panels::AView::_Render_Impl()
{
}

void OvEditor::Panels::AView::Render()
{

}


std::pair<uint16_t, uint16_t> OvEditor::Panels::AView::GetSafeSize() const
{
    auto result = GetSize() - OvMaths::FVector2{ 0.f, 25.f }; // 25 == title bar height
    return { static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y) };
}

