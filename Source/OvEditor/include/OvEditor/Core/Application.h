#pragma once
#include"../../example/scene_05.h"
#include <OvWindowing/Context/Device.h>
#include <OvWindowing/Inputs/InputManager.h>
#include <OvWindowing/Window.h>
#include <OvUI/Core/UIManager.h>
#include "OvEditor/Core/Editor.h"

namespace OvEditor::Core
{
	/**
	* Entry point of OvEditor
	*/
	class Application
	{
	public:
		/**
		* Constructor
		* @param p_projectPath
		* @param p_projectName
		*/
		Application(const std::string& p_projectPath, const std::string& p_projectName);

		/**
		* Destructor
		*/
		~Application();

		/**
		* Run the app
		*/
		void Run();

		/**
		* Returns true if the app is running
		*/
		bool IsRunning() const;
        std::unique_ptr<OvWindowing::Context::Device>			device;
        std::unique_ptr<OvWindowing::Window>					window;
        std::unique_ptr<OvWindowing::Inputs::InputManager>		inputManager;
        std::unique_ptr<OvUI::Core::UIManager>					uiManager;
        OvWindowing::Settings::WindowSettings windowSettings;
        std::unique_ptr<scene::Scene> scene;

	private:
		
		Editor m_editor;
	};
}