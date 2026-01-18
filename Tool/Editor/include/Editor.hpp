#pragma once
#include "CameraSystem.hpp"
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
    // ImGui
    ImGuiID mDockSpaceID{};

    //====render thread====
    uint32_t mCurrentFrame = 0;
    vk::SurfaceKHR mSurface{};
    std::unique_ptr<SwapChainResource> mSwapChainResource{};
    std::vector<std::shared_ptr<OffscreenFrameResource>> mOffscreenFrameResources{};
    std::vector<vk::CommandBuffer> mUICommandBuffers{};
    //================
    std::vector<VkDescriptorSet> mFrameDescriptorSets{};

    bool mIsRunning = false;

    ImDrawData *mFrameDrawData;
    std::mutex mFrameDrawDataMutex;

  private:
    uint64_t mUIFrameIndex = 0;
    uint32_t mImageIndex = 0;
    std::vector<std::mutex> mFrameMutexes{};
    std::vector<std::condition_variable> mFrameProduceCVs{}, mFrameConsumeCVs{};
    std::vector<ImDrawDataSnapshot> mFrameSnapshots{};
    std::vector<bool> mHasFrameData{};

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

    void Inspector();
    void ReflectObject(std::any object, std::string typeName);

    void Render();

    void UIAcquireSwapChainImage(OffscreenFrameResource *frameResource);
    void UIRenderPass(OffscreenFrameResource *frameResource);
    void UIPresent(OffscreenFrameResource *frameResource);

  private:
    void HandleSwapchainOutOfDate();
};
} // namespace MEngine::Tool