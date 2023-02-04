#include "OvEditor/Core/Editor.h"
using namespace OvEditor::Panels;
OvEditor::Core::Editor::Editor() : 
	m_panelsManager(m_canvas)

{
	SetupUI();

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

}

void OvEditor::Core::Editor::RenderEditorUI(float p_deltaTime)
{

}

void OvEditor::Core::Editor::PostUpdate()
{

	++m_elapsedFrames;
}