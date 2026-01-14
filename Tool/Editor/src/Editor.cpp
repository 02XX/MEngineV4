#include "Editor.hpp"
#include "ILogger.hpp"
#include "Logger.hpp"
#include <GLFW/glfw3.h>
#include <hello_imgui/hello_imgui.h>

namespace MEngine::Tool
{

Editor::Editor()
{
    Logger::GetInstance().GetLogger()->SetLogLevel(LogLevel::Trace);
    mParams.appWindowParams.windowTitle = "MEngine Editor";
    mParams.rendererBackendType = HelloImGui::RendererBackendType::Vulkan;
    mParams.fpsIdling.enableIdling = true;
    mParams.fpsIdling.fpsIdle = 60.0f;
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
        {"Inspector", "RightDockSpace", [this] { Inspector(); }, true, true},
        {"Viewport", "MainDockSpace", [this] { ViewPort(); }, true, true}};
    mParams.dockingParams.layoutCondition = HelloImGui::DockingLayoutCondition::ApplicationStart;
    mParams.dockingParams.mainDockSpaceNodeFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    // uint32_t vulkanInstanceExtensionCount = 0;
    // const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
    // std::vector<const char *> extensions(vulkanInstanceExtensions,
    //                                      vulkanInstanceExtensions + vulkanInstanceExtensionCount);

    // ContextConfig config{};
    // config.InstanceRequiredExtensions = extensions;
    // config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
    // mContext = std::make_shared<Context>(config);
    // mRenderSystem = std::make_shared<RenderSystem>(mContext);
    // mRenderSystem->Init();
};
Editor::~Editor()
{
    // mRenderSystem->Shutdown();
    // mRenderSystem.reset();
    // std::function<void(std::shared_ptr<Context>)> item;
    // auto size = PendingDeletions.Size();
    // while (PendingDeletions.TryPop(item))
    // {
    //     item(mContext);
    //     LogDebug("Processed a pending deletion in RenderSystem destructor");
    // }
    // mContext.reset();
}
void Editor::Run()
{
    HelloImGui::Run(mParams);
}
void Editor::ViewPort()
{
    // mRenderSystem->Update(0.016);
}
void Editor::Hierarchy()
{
}
void Editor::AssetBrowser()
{
}

void Editor::Inspector()
{
}
} // namespace MEngine::Tool