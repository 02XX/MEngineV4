#include "Editor.hpp"
#include "Logger.hpp"
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

    Logger::GetInstance().GetLogger()->SetLogCallback(
        [this](LogLevel level, const std::source_location &loc, const std::string &message) {
            this->LogCallback(level, loc, message);
        });

    uint32_t vulkanInstanceExtensionCount = 0;
    const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
    std::vector<const char *> extensions(vulkanInstanceExtensions,
                                         vulkanInstanceExtensions + vulkanInstanceExtensionCount);

    // RHIContextConfig rhiConfig{};
    // rhiConfig.InstanceRequiredExtensions = extensions;
    // rhiConfig.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
    // rhiConfig.DeviceRequiredExtensions = {"VK_KHR_swapchain"};

    // RHIContext::Instance().InitInstance(rhiConfig);
    // RHIContext::Instance().InitContext();

    // mAssetManager = std::make_shared<AssetManager>();
    // mScene = std::make_shared<Scene>("EditorScene");
    // mRenderSystem = std::make_shared<RenderSystem>(mScene, mAssetManager);
    // mRenderSystem->Init();
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
    // mRenderSystem->Update(0.016);
    LogError("Current Time: {}", glfwGetTime());
}
void Editor::Hierarchy()
{
}
void Editor::AssetBrowser()
{
}
void Editor::Console()
{
    if (ImGui::Button("Clear"))
    {
        mLogEntries.clear();
    }
    ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(mLogEntries.size()));
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const auto &entry = mLogEntries[i];
                ImVec4 color = {1, 1, 1, 1};
                if (entry.Level == Core::LogLevel::Error)
                    color = {1.0f, 0.4f, 0.4f, 1.0f};
                else if (entry.Level == Core::LogLevel::Warn)
                    color = {1.0f, 0.8f, 0.0f, 1.0f};
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("[%s:%d] %s", entry.Loc.file_name(), entry.Loc.line(), entry.Message.c_str());
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    // TODO: 在这里实现跳转逻辑
                }
                ImGui::PopStyleColor();
            }
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}
void Editor::Inspector()
{
}

// Callbacks
void Editor::LogCallback(LogLevel level, const std::source_location &loc, const std::string &message)
{
    if (mLogEntries.size() >= mMaxLogEntries)
    {
        mLogEntries.erase(mLogEntries.begin());
    }
    mLogEntries.push_back({level, message, loc});
}
} // namespace MEngine::Tool