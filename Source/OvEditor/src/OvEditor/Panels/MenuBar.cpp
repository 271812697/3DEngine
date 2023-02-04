/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvTools/Utils/SystemCalls.h>


#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Widgets/Sliders/SliderInt.h>
#include <OvUI/Widgets/Sliders/SliderFloat.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include<OvUI/Widgets/Texts/Text.h>

#include "OvEditor/Panels/MenuBar.h"

#include "OvEditor/Settings/EditorSettings.h"


using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvUI::Widgets::Menu;


OvEditor::Panels::MenuBar::MenuBar()
{
	CreateFileMenu();
	CreateBuildMenu();
	CreateWindowMenu();
	CreateActorsMenu();
	CreateResourcesMenu();
	CreateSettingsMenu();
	CreateLayoutMenu();
	CreateHelpMenu();
}

void OvEditor::Panels::MenuBar::HandleShortcuts(float p_deltaTime)
{

}

void OvEditor::Panels::MenuBar::CreateFileMenu()
{
	auto& fileMenu = CreateWidget<MenuList>("File");
	fileMenu.CreateWidget<MenuItem>("New Scene", "CTRL + N");
	fileMenu.CreateWidget<MenuItem>("Save Scene", "CTRL + S");
	fileMenu.CreateWidget<MenuItem>("Save Scene As...", "CTRL + SHIFT + S");
	fileMenu.CreateWidget<MenuItem>("Exit", "ALT + F4");
}

void OvEditor::Panels::MenuBar::CreateBuildMenu()
{
	auto& buildMenu = CreateWidget<MenuList>("Build");
	buildMenu.CreateWidget<MenuItem>("Build game").ClickedEvent				;
	buildMenu.CreateWidget<MenuItem>("Build game and run").ClickedEvent		;
	buildMenu.CreateWidget<Visual::Separator>();
	buildMenu.CreateWidget<MenuItem>("Temporary build").ClickedEvent		;
}

void OvEditor::Panels::MenuBar::CreateWindowMenu()
{
	m_windowMenu = &CreateWidget<MenuList>("Window");
	m_windowMenu->CreateWidget<MenuItem>("Close all").ClickedEvent	+= std::bind(&MenuBar::OpenEveryWindows, this, false);
	m_windowMenu->CreateWidget<MenuItem>("Open all").ClickedEvent		+= std::bind(&MenuBar::OpenEveryWindows, this, true);
	m_windowMenu->CreateWidget<Visual::Separator>();

	/* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
	m_windowMenu->ClickedEvent += std::bind(&MenuBar::UpdateToggleableItems, this);
}

void OvEditor::Panels::MenuBar::CreateActorsMenu()
{
	auto& actorsMenu = CreateWidget<MenuList>("Actors");
    //Utils::ActorCreationMenu::GenerateActorCreationMenu(actorsMenu);
}

void OvEditor::Panels::MenuBar::CreateResourcesMenu()
{
	auto& resourcesMenu = CreateWidget<MenuList>("Resources");
	resourcesMenu.CreateWidget<MenuItem>("Compile shaders");
	resourcesMenu.CreateWidget<MenuItem>("Save materials");
}

void OvEditor::Panels::MenuBar::CreateSettingsMenu()
{
	auto& settingsMenu = CreateWidget<MenuList>("Settings");
	settingsMenu.CreateWidget<MenuItem>("Spawn actors at origin", "", true, true);
	settingsMenu.CreateWidget<MenuItem>("Vertical Synchronization", "", true, true);
	auto& cameraSpeedMenu = settingsMenu.CreateWidget<MenuList>("Camera Speed");
	cameraSpeedMenu.CreateWidget<OvUI::Widgets::Sliders::SliderInt>(1, 50, 15, OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Scene View");
	cameraSpeedMenu.CreateWidget<OvUI::Widgets::Sliders::SliderInt>(1, 50, 15, OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Asset View");
	auto& cameraPositionMenu = settingsMenu.CreateWidget<MenuList>("Reset Camera");
	cameraPositionMenu.CreateWidget<MenuItem>("Scene View");
	cameraPositionMenu.CreateWidget<MenuItem>("Asset View");

	auto& viewColors = settingsMenu.CreateWidget<MenuList>("View Colors");
	auto& sceneViewBackground = viewColors.CreateWidget<MenuList>("Scene View Background");
	auto& sceneViewBackgroundPicker = sceneViewBackground.CreateWidget<Selection::ColorEdit>(false, OvUI::Types::Color{ 0.098f, 0.098f, 0.098f });

	sceneViewBackground.CreateWidget<MenuItem>("Reset");

	auto& sceneViewGrid = viewColors.CreateWidget<MenuList>("Scene View Grid");
    auto& sceneViewGridPicker = sceneViewGrid.CreateWidget<Selection::ColorEdit>(false, OvUI::Types::Color(0.176f, 0.176f, 0.176f));
	sceneViewGridPicker.ColorChangedEvent ;
	sceneViewGrid.CreateWidget<MenuItem>("Reset");

	auto& assetViewBackground = viewColors.CreateWidget<MenuList>("Asset View Background");
	auto& assetViewBackgroundPicker = assetViewBackground.CreateWidget<Selection::ColorEdit>(false, OvUI::Types::Color{ 0.098f, 0.098f, 0.098f });
	assetViewBackgroundPicker.ColorChangedEvent ;
	assetViewBackground.CreateWidget<MenuItem>("Reset");

	auto& assetViewGrid = viewColors.CreateWidget<MenuList>("Asset View Grid");
	auto& assetViewGridPicker = assetViewGrid.CreateWidget<Selection::ColorEdit>(false, OvUI::Types::Color(0.176f, 0.176f, 0.176f));

	assetViewGrid.CreateWidget<MenuItem>("Reset");

	auto& sceneViewBillboardScaleMenu = settingsMenu.CreateWidget<MenuList>("3D Icons Scales");
	auto& lightBillboardScaleSlider = sceneViewBillboardScaleMenu.CreateWidget<Sliders::SliderInt>(0, 100, static_cast<int>(Settings::EditorSettings::LightBillboardScale * 100.0f), OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Lights");
	lightBillboardScaleSlider.ValueChangedEvent += [this](int p_value) { Settings::EditorSettings::LightBillboardScale = p_value / 100.0f; };
	lightBillboardScaleSlider.format = "%d %%";

	auto& snappingMenu = settingsMenu.CreateWidget<MenuList>("Snapping");
	snappingMenu.CreateWidget<Drags::DragFloat>(0.001f, 999999.0f, Settings::EditorSettings::TranslationSnapUnit, 0.05f, "Translation Unit").ValueChangedEvent += [this](float p_value) { Settings::EditorSettings::TranslationSnapUnit = p_value; };
	snappingMenu.CreateWidget<Drags::DragFloat>(0.001f, 999999.0f, Settings::EditorSettings::RotationSnapUnit, 1.0f, "Rotation Unit").ValueChangedEvent += [this](float p_value) { Settings::EditorSettings::RotationSnapUnit = p_value; };
	snappingMenu.CreateWidget<Drags::DragFloat>(0.001f, 999999.0f, Settings::EditorSettings::ScalingSnapUnit, 0.05f, "Scaling Unit").ValueChangedEvent += [this](float p_value) { Settings::EditorSettings::ScalingSnapUnit = p_value; };

	auto& debuggingMenu = settingsMenu.CreateWidget<MenuList>("Debugging");
	debuggingMenu.CreateWidget<MenuItem>("Show geometry bounds", "", true, Settings::EditorSettings::ShowGeometryBounds).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::ShowGeometryBounds = p_value; };
	debuggingMenu.CreateWidget<MenuItem>("Show lights bounds", "", true, Settings::EditorSettings::ShowLightBounds).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::ShowLightBounds = p_value; };
	auto& subMenu = debuggingMenu.CreateWidget<MenuList>("Frustum culling visualizer...");
	subMenu.CreateWidget<MenuItem>("For geometry", "", true, Settings::EditorSettings::ShowGeometryFrustumCullingInSceneView).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::ShowGeometryFrustumCullingInSceneView = p_value; };
	subMenu.CreateWidget<MenuItem>("For lights", "", true, Settings::EditorSettings::ShowLightFrustumCullingInSceneView).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::ShowLightFrustumCullingInSceneView = p_value; };
}

void OvEditor::Panels::MenuBar::CreateLayoutMenu() 
{
	auto& layoutMenu = CreateWidget<MenuList>("Layout");
	layoutMenu.CreateWidget<MenuItem>("Reset");
}

void OvEditor::Panels::MenuBar::CreateHelpMenu()
{
    auto& helpMenu = CreateWidget<MenuList>("Help");
    helpMenu.CreateWidget<MenuItem>("GitHub").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/adriengivry/Overload"); };
    helpMenu.CreateWidget<MenuItem>("Tutorials").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/adriengivry/Overload/wiki/Tutorials"); };
    helpMenu.CreateWidget<MenuItem>("Scripting API").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/adriengivry/Overload/wiki/Scripting-API"); };
    helpMenu.CreateWidget<Visual::Separator>();
    helpMenu.CreateWidget<MenuItem>("Bug Report").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/adriengivry/Overload/issues/new?assignees=&labels=Bug&template=bug_report.md&title="); };
    helpMenu.CreateWidget<MenuItem>("Feature Request").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/adriengivry/Overload/issues/new?assignees=&labels=Feature&template=feature_request.md&title="); };
    helpMenu.CreateWidget<Visual::Separator>();
    helpMenu.CreateWidget<Texts::Text>("Version: 1.3.0");
}

void OvEditor::Panels::MenuBar::RegisterPanel(const std::string& p_name, OvUI::Panels::PanelWindow& p_panel)
{
	auto& menuItem = m_windowMenu->CreateWidget<MenuItem>(p_name, "", true, true);
	menuItem.ValueChangedEvent += std::bind(&OvUI::Panels::PanelWindow::SetOpened, &p_panel, std::placeholders::_1);

	m_panels.emplace(p_name, std::make_pair(std::ref(p_panel), std::ref(menuItem)));
}

void OvEditor::Panels::MenuBar::UpdateToggleableItems()
{
	for (auto&[name, panel] : m_panels)
		panel.second.get().checked = panel.first.get().IsOpened();
}

void OvEditor::Panels::MenuBar::OpenEveryWindows(bool p_state)
{
	for (auto&[name, panel] : m_panels)
		panel.first.get().SetOpened(p_state);
}
