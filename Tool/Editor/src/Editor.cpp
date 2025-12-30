#include "Editor.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include <GLFW/glfw3.h>
#include <hello_imgui/hello_imgui.h>

namespace MEngine::Tool
{

Editor::Editor()
{
    mParams.appWindowParams.windowTitle = "MEngine Editor";
    mParams.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
    // mParams.imGuiWindowParams.enableViewports = true;
    mParams.dockingParams.dockingSplits = {// 先分割底部
                                           {"MainDockSpace", "BottomDockSpace", ImGuiDir_Down, 0.25f},
                                           // 然后分割右侧
                                           {"MainDockSpace", "RightDockSpace", ImGuiDir_Right, 0.25f},
                                           // 最后分割左侧
                                           {"MainDockSpace", "LeftDockSpace", ImGuiDir_Left, 0.2f}};

    mParams.dockingParams.dockableWindows = {
        {"Hierarchy", "LeftDockSpace", [this] { ViewPort(); }, true, true},
        {"AssetBrowser", "BottomDockSpace", [this] { AssetBrowser(); }, true, true},
        {"Console", "BottomDockSpace", [this] { Console(); }, true, true},
        {"Inspector", "RightDockSpace", [this] { Inspector(); }, true, true},
        {"Viewport", "MainDockSpace", [this] { ViewPort(); }, true, true}};
    mParams.dockingParams.layoutCondition = HelloImGui::DockingLayoutCondition::ApplicationStart;
    mParams.dockingParams.mainDockSpaceNodeFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    uint32_t vulkanInstanceExtensionCount = 0;
    const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
    std::vector<const char *> extensions(vulkanInstanceExtensions,
                                         vulkanInstanceExtensions + vulkanInstanceExtensionCount);

    RHIContextConfig rhiConfig{};
    rhiConfig.InstanceRequiredExtensions = extensions;
    rhiConfig.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
    rhiConfig.DeviceRequiredExtensions = {"VK_KHR_swapchain"};

    RHIContext::Instance().InitInstance(rhiConfig);
    RHIContext::Instance().InitContext();

    mAssetManager = std::make_shared<AssetManager>();
    mScene = std::make_shared<Scene>("EditorScene");
    mRenderSystem = std::make_shared<RenderSystem>(mScene, mAssetManager);
    mRenderSystem->Init();
};
Editor::~Editor()
{
}
void Editor::Run()
{
    HelloImGui::Run(mParams);
}
void Editor::ViewPort()
{
    mRenderSystem->Update(0.016);
    
}
void Editor::Hierarchy()
{
}
void Editor::AssetBrowser()
{
}
void Editor::Console()
{
}
void Editor::Inspector()
{
}

} // namespace MEngine::Tool