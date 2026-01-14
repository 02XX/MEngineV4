#pragma once
#include "Context.hpp"
#include "RenderSystem.hpp"
#include "SwapChainResource.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <memory>
#include <taskflow/taskflow.hpp>

using namespace MEngine::Function;
namespace MEngine::Tool
{
struct WindowConfig
{
    int32_t width = 1280;
    int32_t height = 720;
    std::string title = "MEngine Editor";
    bool fullscreen = false;
    bool resizable = true;
    bool vsync = true;
};
class Editor
{
  private:
    std::shared_ptr<Context> mContext;
    std::shared_ptr<RenderSystem> mRenderSystem;

  private:
    // Window
    GLFWwindow *mWindow{};
    WindowConfig mWindowConfig{};
    // ImGui
    ImGuiID mDockSpaceID{};
    std::unique_ptr<SwapChainResource> mSwapChainResource{};
    vk::SurfaceKHR mSurface{};
    std::vector<vk::UniqueSemaphore> mImageAvailableSemaphores{};
    std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores{};
    std::vector<vk::UniqueFence> mInFlightFences{};
    std::vector<vk::UniqueCommandPool> mGraphicCommandPools{};
    std::vector<vk::UniqueCommandBuffer> mGraphicCommandBuffers{};
    vk::ClearValue mColorClearValue = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});
    vk::ClearValue mDepthClearValue = vk::ClearDepthStencilValue(1.0f, 0);
    uint32_t mCurrentFrame = 0;
    bool mIsRunning = false;

  private:
    tf::Taskflow mTaskflow{};
    tf::Executor mExecutor{};

  public:
    Editor();
    ~Editor();
    void Run();

  private:
    void InitWindow();
    void InitVulkan();
    void InitImGui();
    void UILayout();

    void ViewPort();
    void Hierarchy();
    void AssetBrowser();
    void Console();
    void Inspector();

  private:
    void HandleSwapchainOutOfDate();
};
} // namespace MEngine::Tool