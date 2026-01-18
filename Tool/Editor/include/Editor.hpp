#pragma once
#include "CameraSystem.hpp"
#include "ConcurrentQueue.hpp"
#include "ConcurrentRingBuffer.hpp"
#include "Context.hpp"
#include "ECS.hpp"
#include "OffscreenFrameResource.hpp"
#include "RenderSystem.hpp"
#include "SwapChainResource.hpp"
#include "TransformSystem.hpp"
#include <GLFW/glfw3.h>

#include "imgui_threaded_rendering.h"
#include <cstdint>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <memory>
#include <mutex>
#include <taskflow/taskflow.hpp>
#include <thread>
#include <vector>

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
struct Resolution
{
    uint32_t width = 1280;
    uint32_t height = 720;
    const std::string ToString() const
    {
        std::string resolution = std::format("{}x{}", width, height);
        return resolution;
    }
    bool operator==(const Resolution &other) const
    {
        return width == other.width && height == other.height;
    }
};
class Editor
{
  private:
    std::shared_ptr<Context> mContext;
    std::shared_ptr<Scene> mScene;
    std::shared_ptr<AssetManager> mAssetManager;
    std::shared_ptr<TransformSystem> mTransformSystem;
    std::shared_ptr<CameraSystem> mCameraSystem;
    std::shared_ptr<RenderSystem> mRenderSystem;

  private:
    // Window
    GLFWwindow *mWindow{};
    WindowConfig mWindowConfig{};
    inline static const std::vector<Resolution> sResolutions = {{100, 100},   {800, 600},   {1280, 720}, {1920, 1080},
                                                                {2560, 1440}, {3840, 2160}, {5120, 2880}};
    Resolution mCurrentResolution = sResolutions[2];
    bool mNeedReCreateFrameResources = true;
    bool mNeedReCreateSwapChain = true;
    // ImGui
    ImGuiID mDockSpaceID{};

    //====render thread====
    double FPS = 144; // 目标帧率
    vk::SurfaceKHR mSurface{};
    std::unique_ptr<SwapChainResource> mSwapChainResource{};
    std::vector<std::shared_ptr<OffscreenFrameResource>> mOffscreenFrameResources{};
    ImDrawDataSnapshot *mCurrentFrameDrawDataSnapshot{};
    //================
    std::vector<VkDescriptorSet> mFrameDescriptorSets{};
    bool mIsRunning = false;

  private:
    uint32_t mImageIndex = 0;
    uint32_t mCurrentFrameIndex = 0;
    ConcurrentRingBuffer<ImDrawDataSnapshot *> mFrameDrawDataSnapshots{3};

    tf::Taskflow mTaskflow{};
    tf::Executor mExecutor{};

  private:
    // Hierarchy Panel
    Entity mSelectedEntity{NullEntity};

  public:
    Editor();
    ~Editor();
    void Run();

  private:
    std::thread mRenderThread;

  private:
    void InitWindow();
    void InitVulkan();
    void InitImGui();
    void UILayout();

    void ViewPort();

    //===============
    void Hierarchy();
    void ReParentEntity(Entity entity, Entity newParent);
    void RenderEntityNode(Entity entity);
    void DeleteEntityAndChildren(Entity entity);
    //===============

    void AssetBrowser();
    void Console();
    void Toolbar();
    void Inspector();

    void CreateFrameResources();
    void UIAcquireSwapChainImage(OffscreenFrameResource *frameResource);
    void UIRenderPass(OffscreenFrameResource *frameResource);
    void UIPresent(OffscreenFrameResource *frameResource);
};
} // namespace MEngine::Tool