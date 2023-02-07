#pragma once
#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Visual/Image.h>
#include<Opengl/asset/fbo.h>

namespace OvEditor::Panels
{
    /**
    * Base class for any view
    */
    class AView : public OvUI::Panels::PanelWindow
    {
    public:

        AView
        (
            const std::string& p_title,
            bool p_opened,
            const OvUI::Settings::PanelWindowSettings& p_windowSettings
        );


        virtual void Update(float p_deltaTime);


        void _Draw_Impl() override;

        /**
        * Custom implementation of the render method to define in dervied classes
        */
        virtual void _Render_Impl() ;

        /**
        * Render the view
        */
        void Render();

     
        std::pair<uint16_t, uint16_t> GetSafeSize() const;

    public:
       std::shared_ptr<asset::FBO> m_fbo;
    protected:
       
        OvUI::Widgets::Visual::Image* m_image;

    };
}