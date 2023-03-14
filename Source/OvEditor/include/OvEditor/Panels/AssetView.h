/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <variant>

#include <OvCore/Resources/Material.h>

#include "OvEditor/Panels/AViewControllable.h"
#include "Opengl//asset/shader.h"

namespace OvEditor::Panels
{
	/**
	* Provide a view for assets
	*/
	class AssetView : public OvEditor::Panels::AViewControllable
	{
	public:
		using ViewableResource = std::variant<OvRendering::Resources::Model*, OvRendering::Resources::Texture*, OvCore::Resources::Material*>;

		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AssetView
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Custom implementation of the render method
		*/
		virtual void _Render_Impl() override;

		/**
		* Defines the resource to preview
		* @parma p_resource
		*/
		void SetResource(ViewableResource p_resource);

		/**
		* Return the currently previewed resource
		*/
		ViewableResource GetResource() const;

	private:
        std::shared_ptr<asset::Shader>postprocess_shader;
       
        unsigned int Quad = 0;
        std::unique_ptr<OvRendering::Buffers::Framebuffer> m_resfbo;
		ViewableResource m_resource;
	};
}