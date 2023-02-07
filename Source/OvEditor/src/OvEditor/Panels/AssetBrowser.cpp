#include <fstream>
#include <iostream>
#include <OvUI/Widgets/Texts/TextClickable.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Visual/Image.h>
#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Plugins/DDSource.h>
#include <OvUI/Plugins/DDTarget.h>
#include <OvUI/Plugins/ContextualMenu.h>
#include <OvWindowing/Dialogs/MessageBox.h>
#include <OvWindowing/Dialogs/SaveFileDialog.h>
#include <OvWindowing/Dialogs/OpenFileDialog.h>
#include <OvTools/Utils/SystemCalls.h>
#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/String.h>
#include "OvEditor/Panels/AssetBrowser.h"
#include<Opengl/asset/texture.h>
#include<Opengl/core/base.h>
using namespace OvUI::Panels;
using namespace OvUI::Widgets;

#define FILENAMES_CHARS OvEditor::Panels::AssetBrowser::__FILENAMES_CHARS

const std::string FILENAMES_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_=+ 0123456789()[]";

std::string GetAssociatedMetaFile(const std::string& p_assetPath)
{
    return p_assetPath + ".meta";
}

void RenameAsset(const std::string& p_prev, const std::string& p_new)
{
    std::filesystem::rename(p_prev, p_new);

    if (const std::string previousMetaPath = GetAssociatedMetaFile(p_prev); std::filesystem::exists(previousMetaPath))
    {
        if (const std::string newMetaPath = GetAssociatedMetaFile(p_new); !std::filesystem::exists(newMetaPath))
        {
            std::filesystem::rename(previousMetaPath, newMetaPath);
        }
        else
        {
            //
        }
    }
}

void RemoveAsset(const std::string& p_toDelete)
{
    std::filesystem::remove(p_toDelete);

    if (const std::string metaPath = GetAssociatedMetaFile(p_toDelete); std::filesystem::exists(metaPath))
    {
        std::filesystem::remove(metaPath);
    }
}
class TexturePreview : public OvUI::Plugins::IPlugin
{
public:
    TexturePreview() : image(0, { 80, 80 })
    {

    }

    void SetPath(const std::string& p_path)
    {
        //need to be better
        texture = MakeAsset<asset::Texture>(p_path);
    }

    virtual void Execute() override
    {
        if (ImGui::IsItemHovered())
        {
            if (texture)
                image.textureID.id = texture->ID();

            ImGui::BeginTooltip();
            image.Draw();
            ImGui::EndTooltip();
        }
    }

    asset_ref<asset::Texture> texture;
    OvUI::Widgets::Visual::Image image;
};

class BrowserItemContextualMenu : public OvUI::Plugins::ContextualMenu
{
public:
    BrowserItemContextualMenu(const std::string p_filePath, bool p_protected = false) : m_protected(p_protected), filePath(p_filePath) {}

    virtual void CreateList()
    {
        if (true)
        {
            auto& renameMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Rename to...");
            auto& deleteAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Delete");

            auto& nameEditor = renameMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            nameEditor.selectAllOnClick = true;

            renameMenu.ClickedEvent += [this, &nameEditor]
            {
                nameEditor.content = OvTools::Utils::PathParser::GetElementName(filePath);

                if (!std::filesystem::is_directory(filePath))
                    if (size_t pos = nameEditor.content.rfind('.'); pos != std::string::npos)
                        nameEditor.content = nameEditor.content.substr(0, pos);
            };

            deleteAction.ClickedEvent += [this] { DeleteItem(); };

            nameEditor.EnterPressedEvent += [this](std::string p_newName)
            {
                if (!std::filesystem::is_directory(filePath))
                    p_newName += '.' + OvTools::Utils::PathParser::GetExtension(filePath);

                /* Clean the name (Remove special chars) */
                p_newName.erase(std::remove_if(p_newName.begin(), p_newName.end(), [](auto& c)
                    {
                        return std::find(FILENAMES_CHARS.begin(), FILENAMES_CHARS.end(), c) == FILENAMES_CHARS.end();
                    }), p_newName.end());

                std::string containingFolderPath = OvTools::Utils::PathParser::GetContainingFolder(filePath);
                std::string newPath = containingFolderPath + p_newName;
                std::string oldPath = filePath;

                if (filePath != newPath && !std::filesystem::exists(newPath))
                    filePath = newPath;

                if (std::filesystem::is_directory(oldPath))
                    filePath += '\\';

                RenamedEvent.Invoke(oldPath, newPath);
            };
        }
    }

    virtual void Execute() override
    {
        if (m_widgets.size() > 0)
            OvUI::Plugins::ContextualMenu::Execute();
    }

    virtual void DeleteItem() = 0;

public:
    bool m_protected;
    std::string filePath;
    OvTools::Eventing::Event<std::string> DestroyedEvent;
    OvTools::Eventing::Event<std::string, std::string> RenamedEvent;
};

class FolderContextualMenu : public BrowserItemContextualMenu
{
public:
    FolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& showInExplorer = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Show in explorer");
        showInExplorer.ClickedEvent += [this]
        {
            OvTools::Utils::SystemCalls::ShowInExplorer(filePath);
        };

        if (!m_protected)
        {
            auto& importAssetHere = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Import Here...");
     

            auto& createMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Create..");

            auto& createFolderMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Folder");
            auto& createSceneMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Scene");
            auto& createShaderMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Shader");
            auto& createMaterialMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Material");

            auto& createStandardShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Standard template");
            auto& createStandardPBRShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Standard PBR template");
            auto& createUnlitShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Unlit template");
            auto& createLambertShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Lambert template");

            auto& createEmptyMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Empty");
            auto& createStandardMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Standard");
            auto& createStandardPBRMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Standard PBR");
            auto& createUnlitMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Unlit");
            auto& createLambertMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Lambert");

            auto& createFolder = createFolderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createScene = createSceneMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

            auto& createEmptyMaterial = createEmptyMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createStandardMaterial = createStandardMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createStandardPBRMaterial = createStandardPBRMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createUnlitMaterial = createUnlitMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createLambertMaterial = createLambertMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

            auto& createStandardShader = createStandardShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createStandardPBRShader = createStandardPBRShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createUnlitShader = createUnlitShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
            auto& createLambertShader = createLambertShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

            createFolderMenu.ClickedEvent += [&createFolder] { createFolder.content = ""; };
            createSceneMenu.ClickedEvent += [&createScene] { createScene.content = ""; };
            createStandardShaderMenu.ClickedEvent += [&createStandardShader] { createStandardShader.content = ""; };
            createStandardPBRShaderMenu.ClickedEvent += [&createStandardPBRShader] { createStandardPBRShader.content = ""; };
            createUnlitShaderMenu.ClickedEvent += [&createUnlitShader] { createUnlitShader.content = ""; };
            createLambertShaderMenu.ClickedEvent += [&createLambertShader] { createLambertShader.content = ""; };
            createEmptyMaterialMenu.ClickedEvent += [&createEmptyMaterial] { createEmptyMaterial.content = ""; };
            createStandardMaterialMenu.ClickedEvent += [&createStandardMaterial] { createStandardMaterial.content = ""; };
            createStandardPBRMaterialMenu.ClickedEvent += [&createStandardPBRMaterial] { createStandardPBRMaterial.content = ""; };
            createUnlitMaterialMenu.ClickedEvent += [&createUnlitMaterial] { createUnlitMaterial.content = ""; };
            createLambertMaterialMenu.ClickedEvent += [&createLambertMaterial] { createLambertMaterial.content = ""; };

  
            BrowserItemContextualMenu::CreateList();
        }
    }

    virtual void DeleteItem() override
    {
        using namespace OvWindowing::Dialogs;
        MessageBox message("Delete folder", "Deleting a folder (and all its content) is irreversible, are you sure that you want to delete \"" + filePath + "\"?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO);


    }

public:
    OvTools::Eventing::Event<std::string> ItemAddedEvent;
};

class ScriptFolderContextualMenu : public FolderContextualMenu
{
public:
    ScriptFolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FolderContextualMenu(p_filePath, p_protected) {}

    void CreateScript(const std::string& p_name, const std::string& p_path)
    {
        std::string fileContent = "local " + p_name + " =\n{\n}\n\nfunction " + p_name + ":OnStart()\nend\n\nfunction " + p_name + ":OnUpdate(deltaTime)\nend\n\nreturn " + p_name;

        std::ofstream outfile(p_path);
        outfile << fileContent << std::endl; // Empty scene content

        ItemAddedEvent.Invoke(p_path);
        Close();
    }

    virtual void CreateList() override
    {
        FolderContextualMenu::CreateList();

        auto& newScriptMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("New script...");
        auto& nameEditor = newScriptMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

        newScriptMenu.ClickedEvent += [this, &nameEditor]
        {
            nameEditor.content = OvTools::Utils::PathParser::GetElementName("");
        };

        nameEditor.EnterPressedEvent += [this](std::string p_newName)
        {
            /* Clean the name (Remove special chars) */
            p_newName.erase(std::remove_if(p_newName.begin(), p_newName.end(), [](auto& c)
                {
                    return std::find(FILENAMES_CHARS.begin(), FILENAMES_CHARS.end(), c) == FILENAMES_CHARS.end();
                }), p_newName.end());

            std::string newPath = filePath + p_newName + ".lua";

            if (!std::filesystem::exists(newPath))
            {
                CreateScript(p_newName, newPath);
            }
        };
    }
};
class FileContextualMenu : public BrowserItemContextualMenu
{
public:
    FileContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& editAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Open");

        editAction.ClickedEvent += [this]
        {
            OvTools::Utils::SystemCalls::OpenFile(filePath);
        };

        if (!m_protected)
        {
            auto& duplicateAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Duplicate");

            duplicateAction.ClickedEvent += [this]
            {
                std::string filePathWithoutExtension = filePath;

                if (size_t pos = filePathWithoutExtension.rfind('.'); pos != std::string::npos)
                    filePathWithoutExtension = filePathWithoutExtension.substr(0, pos);

                std::string extension = "." + OvTools::Utils::PathParser::GetExtension(filePath);

                auto filenameAvailable = [&extension](const std::string& target)
                {
                    return !std::filesystem::exists(target + extension);
                };

                const auto newNameWithoutExtension = OvTools::Utils::String::GenerateUnique(filePathWithoutExtension, filenameAvailable);

                std::string finalPath = newNameWithoutExtension + extension;
                std::filesystem::copy(filePath, finalPath);

                DuplicateEvent.Invoke(finalPath);
            };
        }

        BrowserItemContextualMenu::CreateList();


        auto& editMetadata = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Properties");

   
    }

    virtual void DeleteItem() override
    {
        using namespace OvWindowing::Dialogs;
        MessageBox message("Delete file", "Deleting a file is irreversible, are you sure that you want to delete \"" + filePath + "\"?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO);


    }

public:
    OvTools::Eventing::Event<std::string> DuplicateEvent;
};


class PreviewableContextualMenu : public FileContextualMenu
{
public:
    PreviewableContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& previewAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Preview");

    

        FileContextualMenu::CreateList();
    }
};

class ShaderContextualMenu : public FileContextualMenu
{
public:
    ShaderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        FileContextualMenu::CreateList();

        auto& compileAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Compile");

      
    }
};

class ModelContextualMenu : public PreviewableContextualMenu
{
public:
    ModelContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& reloadAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Reload");

      

        if (!m_protected)
        {
            auto& generateMaterialsMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Generate materials...");

            generateMaterialsMenu.CreateWidget<OvUI::Widgets::Menu::MenuItem>("Standard");

            generateMaterialsMenu.CreateWidget<OvUI::Widgets::Menu::MenuItem>("StandardPBR");

            generateMaterialsMenu.CreateWidget<OvUI::Widgets::Menu::MenuItem>("Unlit");

            generateMaterialsMenu.CreateWidget<OvUI::Widgets::Menu::MenuItem>("Lambert");
        }

        PreviewableContextualMenu::CreateList();
    }
};

class TextureContextualMenu : public PreviewableContextualMenu
{
public:
    TextureContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& reloadAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Reload");
        PreviewableContextualMenu::CreateList();
    }
};

class SceneContextualMenu : public FileContextualMenu
{
public:
    SceneContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& editAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Edit");



        FileContextualMenu::CreateList();
    }
};

class MaterialContextualMenu : public PreviewableContextualMenu
{
public:
    MaterialContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

    virtual void CreateList() override
    {
        auto& editAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Edit");
        auto& reload = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Reload");
        PreviewableContextualMenu::CreateList();
    }
};

OvEditor::Panels::AssetBrowser::AssetBrowser
(
    const std::string& p_title,
    bool p_opened,
    const OvUI::Settings::PanelWindowSettings& p_windowSettings,
    const std::string& p_engineAssetFolder,
    const std::string& p_projectAssetFolder,
    const std::string& p_projectScriptFolder
) :
    PanelWindow(p_title, p_opened, p_windowSettings),
    m_engineAssetFolder(p_engineAssetFolder),
    m_projectAssetFolder(p_projectAssetFolder),
    m_projectScriptFolder(p_projectScriptFolder)
{
    if (!std::filesystem::exists(m_projectAssetFolder))
    {
        std::filesystem::create_directories(m_projectAssetFolder);

        OvWindowing::Dialogs::MessageBox message
        (
            "Assets folder not found",
            "The \"Assets/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
            OvWindowing::Dialogs::MessageBox::EMessageType::WARNING,
            OvWindowing::Dialogs::MessageBox::EButtonLayout::OK
        );
    }

    if (!std::filesystem::exists(m_projectScriptFolder))
    {
        std::filesystem::create_directories(m_projectScriptFolder);

        OvWindowing::Dialogs::MessageBox message
        (
            "Scripts folder not found",
            "The \"Scripts/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
            OvWindowing::Dialogs::MessageBox::EMessageType::WARNING,
            OvWindowing::Dialogs::MessageBox::EButtonLayout::OK
        );
    }

    auto& refreshButton = CreateWidget<Buttons::Button>("Rescan assets");
    refreshButton.ClickedEvent += std::bind(&AssetBrowser::Refresh, this);
    refreshButton.lineBreak = false;
    refreshButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };

    auto& importButton = CreateWidget<Buttons::Button>("Import asset");
    importButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

    m_assetList = &CreateWidget<Layout::Group>();

    Fill();
}

void OvEditor::Panels::AssetBrowser::Fill()
{
    m_assetList->CreateWidget<OvUI::Widgets::Visual::Separator>();
    ConsiderItem(nullptr, std::filesystem::directory_entry(m_engineAssetFolder), true);
    m_assetList->CreateWidget<OvUI::Widgets::Visual::Separator>();
    ConsiderItem(nullptr, std::filesystem::directory_entry(m_projectAssetFolder), false);
    m_assetList->CreateWidget<OvUI::Widgets::Visual::Separator>();
    ConsiderItem(nullptr, std::filesystem::directory_entry(m_projectScriptFolder), false, false, true);
}

void OvEditor::Panels::AssetBrowser::Clear()
{
    m_assetList->RemoveAllWidgets();
}

void OvEditor::Panels::AssetBrowser::Refresh()
{
    Clear();
    Fill();
}

void OvEditor::Panels::AssetBrowser::ParseFolder(Layout::TreeNode& p_root, const std::filesystem::directory_entry& p_directory, bool p_isEngineItem, bool p_scriptFolder)
{
    /* Iterates another time to display list files */
    for (auto& item : std::filesystem::directory_iterator(p_directory))
        if (item.is_directory())
            ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);

    /* Iterates another time to display list files */
    for (auto& item : std::filesystem::directory_iterator(p_directory))
        if (!item.is_directory())
            ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
}

void OvEditor::Panels::AssetBrowser::ConsiderItem(OvUI::Widgets::Layout::TreeNode* p_root, const std::filesystem::directory_entry& p_entry, bool p_isEngineItem, bool p_autoOpen, bool p_scriptFolder)
{
    bool isDirectory = p_entry.is_directory();
    std::string itemname = OvTools::Utils::PathParser::GetElementName(p_entry.path().string());
    std::string path = p_entry.path().string();
    if (isDirectory && path.back() != '\\') // Add '\\' if is directory and backslash is missing
        path += '\\';
    std::string resourceFormatPath = ":";// EDITOR_EXEC(GetResourcePath(path, p_isEngineItem));
    bool protectedItem = !p_root || p_isEngineItem;

    OvTools::Utils::PathParser::EFileType fileType = OvTools::Utils::PathParser::GetFileType(itemname);

    // Unknown file, so we skip it
    if (fileType == OvTools::Utils::PathParser::EFileType::UNKNOWN && !isDirectory)
    {
        return;
    }

    /* If there is a given treenode (p_root) we attach the new widget to it */
    auto& itemGroup = p_root ? p_root->CreateWidget<Layout::Group>() : m_assetList->CreateWidget<Layout::Group>();

    /* Find the icon to apply to the item */
    uint32_t iconTextureID = 3;//; isDirectory ? EDITOR_CONTEXT(editorResources)->GetTexture("Icon_Folder")->id : EDITOR_CONTEXT(editorResources)->GetFileIcon(itemname)->id;

    itemGroup.CreateWidget<Visual::Image>(iconTextureID, OvMaths::FVector2{ 16, 16 }).lineBreak = false;

    /* If the entry is a directory, the content must be a tree node, otherwise (= is a file), a text will suffice */
    if (isDirectory)
    {
        auto& treeNode = itemGroup.CreateWidget<Layout::TreeNode>(itemname);

        if (p_autoOpen)
            treeNode.Open();

        auto& ddSource = treeNode.AddPlugin<OvUI::Plugins::DDSource<std::pair<std::string, Layout::Group*>>>("Folder", resourceFormatPath, std::make_pair(resourceFormatPath, &itemGroup));

        if (!p_root || p_scriptFolder)
            treeNode.RemoveAllPlugins();

        auto& contextMenu = !p_scriptFolder ? treeNode.AddPlugin<FolderContextualMenu>(path, protectedItem && resourceFormatPath != "") : treeNode.AddPlugin<ScriptFolderContextualMenu>(path, protectedItem && resourceFormatPath != "");
        contextMenu.userData = static_cast<void*>(&treeNode);



        if (!p_scriptFolder)
        {
            if (!p_isEngineItem) /* Prevent engine item from being DDTarget (Can't Drag and drop to engine folder) */
            {
                treeNode.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("Folder");
                treeNode.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("File");
            }
        }
        contextMenu.CreateList();
        ParseFolder(treeNode, std::filesystem::directory_entry(path), p_isEngineItem);
    }
    else
    {
        auto& clickableText = itemGroup.CreateWidget<Texts::TextClickable>(itemname);

        FileContextualMenu* contextMenu = nullptr;

        switch (fileType)
        {
        case OvTools::Utils::PathParser::EFileType::MODEL:		contextMenu = &clickableText.AddPlugin<ModelContextualMenu>(path, protectedItem);		break;
        case OvTools::Utils::PathParser::EFileType::TEXTURE:	contextMenu = &clickableText.AddPlugin<TextureContextualMenu>(path, protectedItem); 	break;
        case OvTools::Utils::PathParser::EFileType::SHADER:		contextMenu = &clickableText.AddPlugin<ShaderContextualMenu>(path, protectedItem);		break;
        case OvTools::Utils::PathParser::EFileType::MATERIAL:	contextMenu = &clickableText.AddPlugin<MaterialContextualMenu>(path, protectedItem);	break;
        case OvTools::Utils::PathParser::EFileType::SCENE:		contextMenu = &clickableText.AddPlugin<SceneContextualMenu>(path, protectedItem);		break;
        default: contextMenu = &clickableText.AddPlugin<FileContextualMenu>(path, protectedItem); break;
        }

        contextMenu->CreateList();
        auto& ddSource = clickableText.AddPlugin<OvUI::Plugins::DDSource<std::pair<std::string, Layout::Group*>>>
            (
                "File",
                resourceFormatPath,
                std::make_pair(resourceFormatPath, &itemGroup)
                );
        if (fileType == OvTools::Utils::PathParser::EFileType::TEXTURE)
        {
            auto& texturePreview = clickableText.AddPlugin<TexturePreview>();
            texturePreview.SetPath(path);
        }
    }
}

