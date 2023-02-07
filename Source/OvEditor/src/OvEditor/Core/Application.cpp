#include<glad/glad.h>
#include <OvTools/Time/Clock.h>
#include "OvEditor/Core/Application.h"
#include "OvEditor/Panels/AView.h"
OvEditor::Core::Application::Application(const std::string& p_projectPath, const std::string& p_projectName) 
{
    /* Settings */
    OvWindowing::Settings::DeviceSettings deviceSettings;
    deviceSettings.contextMajorVersion = 4;
    deviceSettings.contextMinorVersion = 6;
    windowSettings.title = "Overload Editor";
    windowSettings.width = 1280;
    windowSettings.height = 720;
    windowSettings.maximized = false;
    /* Window creation */
    device = std::make_unique<OvWindowing::Context::Device>(deviceSettings);
    window = std::make_unique<OvWindowing::Window>(*device, windowSettings);
    std::vector<uint64_t> iconRaw = { 0,0,144115188614240000,7500771567664627712,7860776967494637312,0,0,0,0,7212820467466371072,11247766461832697600,14274185407633888512,12905091124788992000,5626708973701824512,514575842263176960,0,0,6564302121125019648,18381468271671515136,18381468271654737920,18237353083595659264,18165295488836311040,6708138037527189504,0,4186681893338480640,7932834557741046016,17876782538917681152,11319824055216379904,15210934132358518784,18381468271520454400,1085667680982603520,0,18093237891929479168,18309410677600032768,11391881649237530624,7932834561381570304,17300321784231761408,15210934132375296000,8293405106311272448,2961143145139082752,16507969723533236736,17516777143216379904,10671305705855129600,7356091234422036224,16580027318695106560,2240567205413984000,18381468271470188544,10959253511276599296,4330520004484136960,10815138323200743424,11607771853338181632,8364614976649238272,17444719546862998784,2669156352,18381468269893064448,6419342512197474304,11103650170688640000,6492244531366860800,14346241902646925312,13841557270159628032,7428148827772098304,3464698581331941120,18381468268953606144,1645680384,18381468271554008832,7140201027266418688,5987558797656659712,17588834734687262208,7284033640602212096,14273902834169157632,18381468269087692288,6852253225049397248,17732667349600245504,16291515470083266560,10022503688432981760,11968059825861367552,9733991836700645376,14850363587428816640,18381468271168132864,16147400282007410688,656430432014827520,18381468270950094848,15715054717226194944,72057596690306560,11823944635485519872,15859169905251653376,17084149004500473856,8581352906816952064,2527949855582584832,18381468271419856896,8581352907253225472,252776704,1376441223417430016,14994761349590357760,10527190521537370112,0,9806614576878321664,18381468271671515136,17156206598538401792,6059619689256392448,10166619973990488064,18381468271403079424,17444719549178451968,420746240,870625192710242304,4906133035823863552,18381468269289150464,18381468271671515136,18381468271671515136,9950729769032620032,14778305994951169792,269422336,0,0,18381468268785833984,8941923452686178304,18381468270950094848,34406,1233456333565402880,0,0,0,11823944636091210240,2388,16724143605745719296,2316836,0,0 };
    window->SetIconFromMemory(reinterpret_cast<uint8_t*>(iconRaw.data()), 16, 16);
    inputManager = std::make_unique<OvWindowing::Inputs::InputManager>(*window);
    window->MakeCurrentContext();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::exception("unable to glad");
    }
    
    device->SetVsync(true);

    uiManager = std::make_unique<OvUI::Core::UIManager>(window->GetGlfwWindow(), OvUI::Styling::EStyle::CUSTOM);
    uiManager->LoadFont("Ruda_Big",  "Resource\\font\\trebuc.ttf", 20);
    uiManager->LoadFont("Ruda_Small", "Resource\\font\\Lato.ttf", 12);
    uiManager->LoadFont("Ruda_Medium", "Resource\\font\\Lato.ttf", 14);
    uiManager->UseFont("Ruda_Big");
   // uiManager->SetEditorLayoutSaveFilename(std::string(getenv("APPDATA")) + "\\OverloadTech\\OvEditor\\layout.ini");
    uiManager->SetEditorLayoutAutosaveFrequency(60.0f);
    uiManager->EnableEditorLayoutSave(true);
    uiManager->EnableDocking(true);
    uiManager->SetCanvas(m_editor.m_canvas);
    m_editor.SetupUI();
    scene = std::make_unique<scene::Scene05>("scene05");
    scene->Init();
    dynamic_cast<scene::Scene05*>(scene.get())->setRenderFBO(m_editor.m_panelsManager.GetPanelAs<OvEditor::Panels::AView>("Scene View").m_fbo);
    
}

OvEditor::Core::Application::~Application()
{
}

void OvEditor::Core::Application::Run()
{
	OvTools::Time::Clock clock;

	while (IsRunning())
	{
        glfwPollEvents();

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClearStencil(0);  // 8-bit integer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_editor.Update(clock.GetDeltaTime());
       
        scene->OnSceneRender(clock.GetDeltaTime());
        uiManager->Render();
        window->SwapBuffers();
        inputManager->ClearEvents();
		clock.Update();
	}
}

bool OvEditor::Core::Application::IsRunning() const
{
	return !window->ShouldClose();
}
