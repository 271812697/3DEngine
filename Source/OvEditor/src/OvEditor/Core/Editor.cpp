#include "OvEditor/Core/Editor.h"
#include "OvEditor/Panels/AssetBrowser.h"
#include "OvEditor/Panels/AView.h"
using namespace OvEditor::Panels;
OvEditor::Core::Editor::Editor() : 
	m_panelsManager(m_canvas)

{
	

}

OvEditor::Core::Editor::~Editor()
{
	
}

void OvEditor::Core::Editor::SetupUI()
{
	OvUI::Settings::PanelWindowSettings settings;
	settings.closable = true;
	settings.collapsable = true;
	settings.dockable = true;

    m_panelsManager.CreatePanel<OvEditor::Panels::MenuBar>("Menu Bar");
    m_panelsManager.CreatePanel<OvEditor::Panels::AssetBrowser>("Asset Browser", true, settings,"E:\\C++\\Overload\\Build\\Debug\\Data\\Engine\\", "C:\\Users\\271812697\\Documents\\demo\\Assets\\", "C:\\Users\\271812697\\Documents\\demo\\Scripts\\");
    m_panelsManager.CreatePanel<OvEditor::Panels::AView>("Scene View", true, settings);
	

	m_canvas.MakeDockspace(true);
	
}

void OvEditor::Core::Editor::PreUpdate()
{


}

void OvEditor::Core::Editor::Update(float p_deltaTime)
{
	HandleGlobalShortcuts();
	UpdateCurrentEditorMode(p_deltaTime);
	PrepareRendering(p_deltaTime);
	UpdateEditorPanels(p_deltaTime);
	RenderViews(p_deltaTime);
	

}

void OvEditor::Core::Editor::HandleGlobalShortcuts()
{

}

void OvEditor::Core::Editor::UpdateCurrentEditorMode(float p_deltaTime)
{

}

void OvEditor::Core::Editor::UpdatePlayMode(float p_deltaTime)
{

}

void OvEditor::Core::Editor::UpdateEditMode(float p_deltaTime)
{

}

void OvEditor::Core::Editor::UpdateEditorPanels(float p_deltaTime)
{

}

void OvEditor::Core::Editor::PrepareRendering(float p_deltaTime)
{

}

void OvEditor::Core::Editor::RenderViews(float p_deltaTime)
{
    auto& sceneView = m_panelsManager.GetPanelAs<OvEditor::Panels::AView>("Scene View");
    {
        sceneView.Update(p_deltaTime);
    }
    if (sceneView.IsOpened())
    {
        sceneView.Render();
    }
}

void OvEditor::Core::Editor::RenderEditorUI(float p_deltaTime)
{

}

void OvEditor::Core::Editor::PostUpdate()
{

	++m_elapsedFrames;
}