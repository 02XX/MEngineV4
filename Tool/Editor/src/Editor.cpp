#include "Editor.hpp"
#include "CameraComponent.hpp"
#include "Logger.hpp"
#include "MReflection.hpp"
#include "Material.hpp"
#include "MaterialComponent.hpp"
#include "Math.hpp"
#include "MeshComponent.hpp"
#include "MeshManager.hpp"
#include "PBRMaterial.hpp"
#include "PBRMaterialManager.hpp"
#include "PBRMaterialResource.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "StaticMesh.hpp"
#include "TextureRenderTarget2D.hpp"
#include "TextureRenderTarget2DResource.hpp"
#include "TransformComponent.hpp"
#include <cstddef>
#include <cstdint>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_stdlib.h>
#include <mutex>
#include <vector>

namespace MEngine::Tool
{

Editor::Editor()
{
    Logger::GetInstance().GetLogger()->SetLogLevel(LogLevel::Trace);
    LogInfo("Welcome to MEngine Editor!");
    InitWindow();
    InitVulkan();
    InitImGui();

    mScene = std::make_shared<Scene>("DefaultScene");
    mAssetManager = std::make_shared<AssetManager>(mContext);
    mTransformSystem = std::make_shared<TransformSystem>(mScene, mAssetManager);
    mTransformSystem->Init();
    mCameraSystem = std::make_shared<CameraSystem>(mScene, mAssetManager);
    mCameraSystem->Init();
    mRenderSystem = std::make_shared<RenderSystem>(mContext, mScene, mAssetManager);
    mRenderSystem->Init();
    mRenderSystem->PushPreRecord(
        [this](OffscreenFrameResource *frameResource) { UIAcquireSwapChainImage(frameResource); });
    mRenderSystem->PushRenderPass([this](OffscreenFrameResource *frameResource) { UIRenderPass(frameResource); });
    mRenderSystem->PushPostSubmitPass([this](OffscreenFrameResource *frameResource) { UIPresent(frameResource); });

    auto ecsRegister = mScene->mRegistry;
    auto cubeEntity = ecsRegister->create();
    auto cubeMesh = mAssetManager->GetManager<StaticMesh, MeshManager>()->GetMesh(DefaultMeshType::Cube);
    auto defaultMat = mAssetManager->GetManager<PBRMaterial, PBRMaterialManager>()->GetMaterial(
        DefaultMaterialType::GBufferPBROpaque);
    auto &cubeEntityTransformComponent = ecsRegister->emplace<TransformComponent>(cubeEntity);
    cubeEntityTransformComponent.Rotate(45, Vector3{1.0f, 0.0f, 0.0f});
    auto &cubeEntityMeshComponent = ecsRegister->emplace<MeshComponent>(cubeEntity);
    cubeEntityMeshComponent.Mesh = cubeMesh;
    auto &cubeEntityMaterialComponent = ecsRegister->emplace<MaterialComponent>(cubeEntity);
    cubeEntityMaterialComponent.Material = defaultMat;

    auto cameraEntity = ecsRegister->create();
    auto &cameraTransformComponent = ecsRegister->emplace<TransformComponent>(cameraEntity);
    cameraTransformComponent.name = "EditorCamera";
    cameraTransformComponent.localPosition = Vector3(0.0f, 0.0f, -5.0f);
    auto &cameraEntityCameraComponent = ecsRegister->emplace<CameraComponent>(cameraEntity);
    cameraEntityCameraComponent.isEditorCamera = true;
    cameraEntityCameraComponent.isMainCamera = true;
};
Editor::~Editor()
{
    mContext->Device->waitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (mRenderSystem)
    {
        mRenderSystem->Shutdown();
        mRenderSystem.reset();
    }
    if (mCameraSystem)
    {
        mCameraSystem->Shutdown();
        mCameraSystem.reset();
    }
    if (mTransformSystem)
    {
        mTransformSystem->Shutdown();
        mTransformSystem.reset();
    }
    if (mAssetManager)
    {
        mAssetManager.reset();
    }
    if (mSwapChainResource)
    {
        mSwapChainResource->ReleaseResource(mContext);
        mSwapChainResource.reset();
    }
    for (size_t i = 0; i < mUICommandBuffers.size(); ++i)
    {
        mContext->Device->freeCommandBuffers(mOffscreenFrameResources[i]->PresentCommandPool, {mUICommandBuffers[i]});
    }
    mOffscreenFrameResources.clear();
    mScene.reset();
    std::function<void(std::shared_ptr<Context> context)> item{};
    while (PendingDeletions.TryPop(item))
    {
        item(mContext);
    }
    mContext->Instance.get().destroySurfaceKHR(mSurface);
    LogInfo("Goodbye!");
}
void Editor::InitWindow()
{
    // 读取配置文件
    if (!glfwInit())
    {
        LogError("Failed to initialize GLFW");
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, mWindowConfig.resizable ? GLFW_TRUE : GLFW_FALSE);
    if (mWindowConfig.fullscreen)
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        mWindowConfig.width = mode->width;
        mWindowConfig.height = mode->height;
        mWindow = glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(),
                                   glfwGetPrimaryMonitor(), nullptr);
        glfwWindowHint(GLFW_DECORATED, mWindowConfig.fullscreen ? GLFW_FALSE : GLFW_TRUE);
    }
    else
    {
        mWindow =
            glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(), nullptr, nullptr);
    }
    if (!mWindow)
    {
        LogError("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    if (mWindowConfig.vsync)
    {
        glfwSwapInterval(1); // 启用垂直同步
        LogInfo("VSync enabled");
    }
    else
    {
        glfwSwapInterval(0); // 禁用垂直同步
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwMakeContextCurrent(mWindow);
    LogDebug("GLFW window created successfully: {}x{}", mWindowConfig.width, mWindowConfig.height);
}
void Editor::InitVulkan()
{
    uint32_t vulkanInstanceExtensionCount = 0;
    const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
    std::vector<const char *> extensions(vulkanInstanceExtensions,
                                         vulkanInstanceExtensions + vulkanInstanceExtensionCount);

    ContextConfig config{};
    config.InstanceRequiredExtensions = extensions;
    config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
    config.DeviceRequiredExtensions = {"VK_KHR_swapchain"};
    mContext = std::make_shared<Context>(config);

    glfwCreateWindowSurface(mContext->Instance.get(), mWindow, nullptr, reinterpret_cast<VkSurfaceKHR *>(&mSurface));
    mSwapChainResource = std::make_unique<SwapChainResource>(mSurface);
    mSwapChainResource->InitResource(mContext);
    mFramesInFlight = mSwapChainResource->SwapChainImages.size();
    mOffscreenFrameResources.resize(mFramesInFlight);
    mFrameDescriptorSets.resize(mFramesInFlight);
    mUICommandBuffers.resize(mFramesInFlight);
    for (size_t i = 0; i < mFramesInFlight; i++)
    {
        mOffscreenFrameResources[i] = std::make_shared<OffscreenFrameResource>(mContext, vk::Extent3D{800, 600, 1});
        auto colorAttachment =
            mOffscreenFrameResources[i]->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();

        mUICommandBuffers[i] = mContext->Device->allocateCommandBuffers(
            vk::CommandBufferAllocateInfo()
                .setCommandPool(mOffscreenFrameResources[i]->PresentCommandPool)
                .setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandBufferCount(1))[0];
        auto commandBuffer = mOffscreenFrameResources[i]->GraphicsCommandBuffer;
        vk::ImageMemoryBarrier2 swapChainBarrier{}, colorAttachmentBarrier{};
        swapChainBarrier.setImage(mSwapChainResource->SwapChainImages[i])
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
            .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        colorAttachmentBarrier.setImage(colorAttachment->GetImage())
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
            .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        std::vector<vk::ImageMemoryBarrier2> barriers = {swapChainBarrier, colorAttachmentBarrier};
        commandBuffer.begin(vk::CommandBufferBeginInfo{});
        commandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(barriers));
        commandBuffer.end();
        vk::SubmitInfo2 submitInfo{};
        std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
            vk::CommandBufferSubmitInfo().setCommandBuffer(commandBuffer),
        };
        submitInfo.setCommandBufferInfos(commandBufferInfos);
        mContext->GraphicsQueue.submit2(submitInfo, {});
        mContext->Device->waitIdle();
    }
}
void Editor::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘导航
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 启用Docking

    ImGui::StyleColorsDark(); // 设置主题
    ImGui_ImplGlfw_InitForVulkan(mWindow, true);
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = mContext->Instance.get();
    initInfo.PhysicalDevice = mContext->PhysicalDevice;
    initInfo.Device = mContext->Device.get();
    initInfo.QueueFamily = mContext->QueueFamilyIndicates.graphicsFamily.value();
    initInfo.Queue = mContext->GraphicsQueue;
    initInfo.UseDynamicRendering = true;
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
    std::vector<vk::Format> colorAttachmentFormats = {vk::Format::eR8G8B8A8Srgb};
    pipelineRenderingCreateInfo.setColorAttachmentFormats(colorAttachmentFormats)
        .setDepthAttachmentFormat(vk::Format::eD32SfloatS8Uint)
        .setStencilAttachmentFormat(vk::Format::eD32SfloatS8Uint)
        .setViewMask(0);
    initInfo.PipelineRenderingCreateInfo = pipelineRenderingCreateInfo;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = MAX_FRAMES_IN_FLIGHT;
    initInfo.DescriptorPoolSize = 1000;
    if (!ImGui_ImplVulkan_Init(&initInfo))
    {
        LogError("Failed to initialize ImGui Vulkan backend");
        throw std::runtime_error("ImGui Vulkan init failed");
    }
    ImFontConfig fontConfig{};
    auto notoSansFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/MSYH.TTC", 18.0f, &fontConfig,
                                                     io.Fonts->GetGlyphRangesChineseFull());
    if (!notoSansFont)
    {
        LogError("Failed to load NotoSans font");
        throw std::runtime_error("NotoSans font load failed");
    }
    io.FontDefault = notoSansFont;

    mFrameConsumeCVs = std::vector<std::condition_variable>(mFramesInFlight);
    mFrameProduceCVs = std::vector<std::condition_variable>(mFramesInFlight);
    mFrameMutexes = std::vector<std::mutex>(mFramesInFlight);
    mFrameSnapshots = std::vector<ImDrawDataSnapshot>(mFramesInFlight);
    mHasFrameData = std::vector<bool>(mFramesInFlight, false);
    for (size_t i = 0; i < mFramesInFlight; i++)
    {
        auto currentColorAttachment =
            mOffscreenFrameResources[i]->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();
        mFrameDescriptorSets[i] =
            ImGui_ImplVulkan_AddTexture(static_cast<VkSampler>(currentColorAttachment->GetSampler()),
                                        static_cast<VkImageView>(currentColorAttachment->GetImageView()),
                                        static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
    }
}
void Editor::UIAcquireSwapChainImage(OffscreenFrameResource *frameResource)
{
    auto device = mContext->Device.get();
    vk::AcquireNextImageInfoKHR acquireInfo{};
    acquireInfo.setTimeout(1000000000)
        .setSwapchain(mSwapChainResource->SwapChain)
        .setSemaphore(mOffscreenFrameResources[mCurrentFrame]->ImageAvailableSemaphore.get())
        .setDeviceMask(1);

    auto nextImageResult = device.acquireNextImage2KHR(&acquireInfo, &mImageIndex);
    if (nextImageResult == vk::Result::eErrorOutOfDateKHR)
    {
        HandleSwapchainOutOfDate();
        return;
    }
}
void Editor::UIRenderPass(OffscreenFrameResource *frameResource)
{
    auto commandBuffer = frameResource->GraphicsCommandBuffer;
    std::vector<vk::RenderingAttachmentInfo> colorAttachments{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->ColorClearValue)
            .setImageView(mSwapChainResource->SwapChainImageViews[mImageIndex])
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
    };
    vk::RenderingInfo renderingInfo{};
    renderingInfo
        .setRenderArea(vk::Rect2D{{0, 0},
                                  {mSwapChainResource->SurfaceInfo.Capabilities.currentExtent.width,
                                   mSwapChainResource->SurfaceInfo.Capabilities.currentExtent.height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachments)
        .setPDepthAttachment(nullptr);
    vk::ImageMemoryBarrier2 preBarrier{}, preRenderBarrier{};
    preBarrier.setImage(mSwapChainResource->SwapChainImages[mImageIndex])
        .setOldLayout(vk::ImageLayout::ePresentSrcKHR)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    preRenderBarrier.setImage(frameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    std::vector<vk::ImageMemoryBarrier2> preBarriers = {preBarrier, preRenderBarrier};
    commandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(preBarriers));

    commandBuffer.beginRendering(renderingInfo);
    {
        std::unique_lock<std::mutex> lck(mFrameMutexes[mCurrentFrame]);
        mFrameConsumeCVs[mCurrentFrame].wait(lck, [this]() { return mHasFrameData[mCurrentFrame] || !mIsRunning; });
        if (!mIsRunning)
            return;
        ImGui_ImplVulkan_RenderDrawData(&mFrameSnapshots[mCurrentFrame].DrawData, commandBuffer);
        mHasFrameData[mCurrentFrame] = false;
        mFrameProduceCVs[mCurrentFrame].notify_one();
    }
    commandBuffer.endRendering();

    vk::ImageMemoryBarrier2 postBarrier{}, postRenderBarrier{};
    postBarrier.setImage(mSwapChainResource->SwapChainImages[mImageIndex])
        .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
        .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eNone)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    postRenderBarrier.setImage(frameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    std::vector<vk::ImageMemoryBarrier2> postBarriers = {postBarrier, postRenderBarrier};
    commandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(postBarriers));
}
void Editor::UIPresent(OffscreenFrameResource *frameResource)
{
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setSwapchains({mSwapChainResource->SwapChain})
        .setImageIndices({mImageIndex})
        .setWaitSemaphores({mOffscreenFrameResources[mCurrentFrame]->RenderFinishedSemaphore.get()});
    try
    {
        auto presentResult = mContext->GraphicsQueue.presentKHR(presentInfo);
        if (presentResult != vk::Result::eSuccess && presentResult != vk::Result::eSuboptimalKHR)
        {
            LogError("Failed to present image: {}", vk::to_string(presentResult));
            throw std::runtime_error("Failed to present image");
        }
    }
    catch (vk::OutOfDateKHRError &)
    {
        HandleSwapchainOutOfDate();
    }
}
void Editor::Run()
{
    mIsRunning = true;
    auto device = mContext->Device.get();
    mTaskflow.emplace([this, device]() {
        while (mIsRunning)
        {
            mRenderSystem->SetOffscreenFrameResource(mOffscreenFrameResources[mCurrentFrame].get());
            mRenderSystem->Update(1.0);
            mCurrentFrame = (mCurrentFrame + 1) % mFramesInFlight;
        }
      });
    mExecutor.run(mTaskflow);
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UILayout();
        ImGui::Render();
        {
            std::unique_lock<std::mutex> lck(mFrameMutexes[mUIFrameIndex]);
            mFrameProduceCVs[mUIFrameIndex].wait(lck,
                                                 [this]() { return !mHasFrameData[mUIFrameIndex] || !mIsRunning; });
            if (!mIsRunning)
                break;
            mFrameSnapshots[mUIFrameIndex].SnapUsingSwap(ImGui::GetDrawData(), ImGui::GetTime());
            mHasFrameData[mUIFrameIndex] = true;
            mFrameConsumeCVs[mUIFrameIndex].notify_one();
        }
        mTransformSystem->Update(1.0);
        mCameraSystem->Update(1.0);
        mUIFrameIndex = (mUIFrameIndex + 1) % mFramesInFlight;
    }
    mIsRunning = false;
    for (size_t i = 0; i < mFramesInFlight; i++)
    {
        std::unique_lock<std::mutex> lck(mFrameMutexes[i]);
        mFrameConsumeCVs[i].notify_one();
        mFrameProduceCVs[i].notify_one();
    }
    mExecutor.wait_for_all();
}
void Editor::Render()
{
}
void Editor::UILayout()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    mDockSpaceID = ImGui::DockSpaceOverViewport();
    // 3. 首次运行时初始化布局
    static bool first_run = true;
    if (first_run)
    {
        first_run = false;
        ImGui::DockBuilderRemoveNode(mDockSpaceID);
        ImGui::DockBuilderAddNode(mDockSpaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(mDockSpaceID, viewport->WorkSize);
        // 1. 主区域拆分为底部（30%）和顶部（70%）
        ImGuiID dockBottomID, dockTopID;
        ImGui::DockBuilderSplitNode(mDockSpaceID, ImGuiDir_Down, 0.3, &dockBottomID, &dockTopID);
        // 2. 顶部区域拆分为左（30%）和剩余部分（70%）
        ImGuiID dockLeftID, remainingTop;
        ImGui::DockBuilderSplitNode(dockTopID, ImGuiDir_Left, 0.3, &dockLeftID, &remainingTop);
        // 3. 剩余部分（70%）拆分为中（60%）和右（40%）
        ImGuiID dockCenterID, dockRightID;
        ImGui::DockBuilderSplitNode(remainingTop, ImGuiDir_Right, 0.4, &dockRightID, &dockCenterID);
        // 4. 中部拆分上(20%)和下(80%)
        ImGuiID dockTopCenterID, dockBottomCenterID;
        ImGui::DockBuilderSplitNode(dockCenterID, ImGuiDir_Up, 0.2, &dockTopCenterID, &dockBottomCenterID);
        // 绑定窗口
        ImGui::DockBuilderDockWindow("Viewport", dockBottomCenterID); // 中间
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);        // 左侧
        ImGui::DockBuilderDockWindow("Inspector", dockRightID);       // 右侧
        ImGui::DockBuilderDockWindow("Assets", dockBottomID);         // 底部
        ImGui::DockBuilderDockWindow("Toolbar", dockTopCenterID);     // 顶部
        ImGui::DockBuilderFinish(mDockSpaceID);
    }
    // ImGui::Begin("Console");
    // {
    //     Console();
    // }
    // ImGui::End();
    ImGui::Begin("Inspector");
    {
        Inspector();
    }
    ImGui::End();
    ImGui::Begin("Assets");
    {
        AssetBrowser();
    }
    ImGui::End();
    // RenderToolbarPanel();
    ImGui::Begin("Viewport");
    {
        ViewPort();
    }
    ImGui::End();
    ImGui::Begin("Hierarchy");
    {
        Hierarchy();
    }
    ImGui::End();
}
void Editor::ViewPort()
{
    auto currentOffscreenFrameResource = mOffscreenFrameResources[mUIFrameIndex].get();
    ImGui::Image(reinterpret_cast<ImTextureID>(mFrameDescriptorSets[mUIFrameIndex]),
                 ImVec2(currentOffscreenFrameResource->Extent.width, currentOffscreenFrameResource->Extent.height));
}
void Editor::Hierarchy()
{
    auto &registry = mScene->mRegistry;
    ImGui::BeginChild("HierarchyList");
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (!ImGui::IsAnyItemHovered())
        {
            mSelectedEntity = entt::null;
        }
    }
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImRect windowRect = window->Rect();
    auto view = registry->view<TransformComponent>();
    for (auto entity : view)
    {
        if (view.get<TransformComponent>(entity).parent == NullEntity)
        {
            RenderEntityNode(entity);
        }
    }

    if (ImGui::BeginDragDropTargetCustom(windowRect, ImGui::GetID("HierarchyList")))
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_TREENODE"))
        {
            Entity draggedEntity = *(const Entity *)payload->Data;
            ReParentEntity(draggedEntity, NullEntity); // 设为根节点
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto newEntity = registry->create();
            registry->emplace<TransformComponent>(newEntity);
        }
        if (mSelectedEntity != NullEntity)
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                DeleteEntityAndChildren(mSelectedEntity);
                mSelectedEntity = NullEntity;
            }
        }
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}
void Editor::ReParentEntity(Entity entity, Entity newParent)
{
    auto registry = mScene->mRegistry;
    auto &transform = registry->get<TransformComponent>(entity);

    // 1. 如果新父节点就是旧父节点，或者就是自己，直接返回
    if (transform.parent == newParent || entity == newParent)
        return;

    // 2. 循环引用检测 (防止把父节点拖给子节点)
    Entity curr = newParent;
    while (curr != NullEntity)
    {
        if (curr == entity)
            return;
        curr = registry->get<TransformComponent>(curr).parent;
    }

    // 3. 从旧父节点移除
    if (transform.parent != NullEntity)
    {
        auto &oldParentTransform = registry->get<TransformComponent>(transform.parent);
        auto &children = oldParentTransform.children;
        children.erase(std::remove(children.begin(), children.end(), entity), children.end());
    }

    // 4. 设置新父节点
    transform.parent = newParent;
    if (newParent != NullEntity)
    {
        registry->get<TransformComponent>(newParent).children.push_back(entity);
    }
}
void Editor::RenderEntityNode(Entity entity)
{
    auto &registry = mScene->mRegistry;
    auto &transform = registry->get<TransformComponent>(entity);
    if (registry->any_of<CameraComponent>(entity))
    {
        auto &cameraComp = registry->get<CameraComponent>(entity);
        if (cameraComp.isEditorCamera)
            return;
    }
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

    if (transform.children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;
    if (mSelectedEntity == entity)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)entity, flags, "%s", transform.name.c_str());

    // 点击逻辑
    if (ImGui::IsItemClicked())
        mSelectedEntity = entity;
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        mSelectedEntity = entity;

    // --- 拖拽源 ---
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ENTITY_TREENODE", &entity, sizeof(Entity));
        ImGui::Text("Move %s", transform.name.c_str());
        ImGui::EndDragDropSource();
    }

    // --- 拖拽目标 ---
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_TREENODE"))
        {
            Entity draggedEntity = *(const Entity *)payload->Data;
            ReParentEntity(draggedEntity, entity);
        }
        ImGui::EndDragDropTarget();
    }

    // 递归渲染
    if (opened)
    {
        auto childrenCopy = transform.children;
        for (auto child : childrenCopy)
        {
            RenderEntityNode(child);
        }
        ImGui::TreePop();
    }
}
void Editor::DeleteEntityAndChildren(Entity entity)
{
    auto registry = mScene->mRegistry;
    if (!registry->valid(entity))
        return;
    auto &transform = registry->get<TransformComponent>(entity);
    for (auto child : transform.children)
    {
        DeleteEntityAndChildren(child);
    }
    if (transform.parent != NullEntity)
    {
        auto &parentTransform = registry->get<TransformComponent>(transform.parent);
        parentTransform.children.erase(
            std::remove(parentTransform.children.begin(), parentTransform.children.end(), entity),
            parentTransform.children.end());
    }
    registry->destroy(entity);
}
void Editor::AssetBrowser()
{
}
void Editor::Console()
{
}
void Editor::Inspector()
{
    auto registry = mScene->mRegistry;
    if (mSelectedEntity != NullEntity && registry->valid(mSelectedEntity))
    {
        if (registry->any_of<TransformComponent>(mSelectedEntity))
        {
            auto &transform = registry->get<TransformComponent>(mSelectedEntity);
            if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::InputText("Name", &transform.name);
                ImGui::DragFloat3("Position", &transform.localPosition.x, 0.1f);
                glm::vec3 euler = glm::degrees(glm::eulerAngles(transform.localRotation));
                if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.01f))
                {
                    transform.localRotation = glm::quat(glm::radians(euler));
                }
                ImGui::DragFloat3("Scale", &transform.localScale.x, 0.1f);
            }
        }
        if (registry->any_of<CameraComponent>(mSelectedEntity))
        {
        }
        if (registry->any_of<MeshComponent>(mSelectedEntity))
        {
            if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto &meshComp = registry->get<MeshComponent>(mSelectedEntity);
                auto mesh = meshComp.Mesh;
                if (mesh)
                {
                    ImGui::Text("Mesh: %s", mesh->GetName().c_str());
                }
                else
                {
                    ImGui::Text("Mesh: None");
                }
            }
        }
        if (registry->any_of<MaterialComponent>(mSelectedEntity))
        {
            if (ImGui::CollapsingHeader("MaterialComponent", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto &materialComp = registry->get<MaterialComponent>(mSelectedEntity);
                auto material = materialComp.Material;
                if (auto pbrMat = std::dynamic_pointer_cast<PBRMaterial>(material))
                {
                    auto &pbrProps = pbrMat->mProperties;
                    if (ImGui::ColorEdit4("Albedo", glm::value_ptr(pbrProps.Albedo)))
                    {
                        pbrProps.Albedo = glm::clamp(pbrProps.Albedo, glm::vec4(0.0f), glm::vec4(1.0f));
                        materialComp.dirty = true;
                    }
                }
                else
                {
                    ImGui::Text("Material: None");
                }
            }
        }
    }
}
void Editor::ReflectObject(std::any object, std::string typeName)
{
    if (ImGui::CollapsingHeader(typeName.data(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto &typeRegistry = MReflection::Registry::GetInstance();
        auto typeInfo = typeRegistry.GetType(typeName);
        auto fields = typeInfo->GetFields();
        for (auto field : fields)
        {
        }
    }
}

void Editor::HandleSwapchainOutOfDate()
{
    mContext->Device->waitIdle();
    mSwapChainResource->ReleaseResource(mContext);
    mSwapChainResource->InitResource(mContext);
    for (size_t i = 0; i < mFramesInFlight; i++)
    {
        auto currentOffscreenFrameResource = mOffscreenFrameResources[i].get();
        auto mUICommandBuffer = currentOffscreenFrameResource->GraphicsCommandBuffer;
        vk::ImageMemoryBarrier2 barrier{};
        barrier.setImage(mSwapChainResource->SwapChainImages[i])
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
            .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        mUICommandBuffer.begin(vk::CommandBufferBeginInfo{});
        mUICommandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(barrier));
        mUICommandBuffer.end();
        vk::SubmitInfo2 submitInfo{};
        std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
            vk::CommandBufferSubmitInfo().setCommandBuffer(mUICommandBuffer),
        };
        submitInfo.setCommandBufferInfos(commandBufferInfos);
        mContext->GraphicsQueue.submit2(submitInfo, {});
        mContext->Device->waitIdle();
    }
}
} // namespace MEngine::Tool