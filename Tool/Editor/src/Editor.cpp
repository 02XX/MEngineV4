#include "Editor.hpp"
#include "Logger.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "TextureRenderTarget2D.hpp"
#include "TextureRenderTarget2DResource.hpp"
#include <cstddef>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
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
    mRenderSystem = std::make_shared<RenderSystem>(mContext, mScene, mAssetManager);
    mRenderSystem->Init();
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
        mContext->Device->freeCommandBuffers(mFrameResources[i]->PresentCommandPool, {mUICommandBuffers[i]});
    }
    if (mFrameResources.size() > 0)
    {
        for (auto &frameResource : mFrameResources)
        {
            frameResource->ReleaseResource(mContext);
        }
        mFrameResources.clear();
    }
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
    auto imageCount = mSwapChainResource->SwapChainImages.size();
    mFrameResources.resize(imageCount);
    mFrameDescriptorSets.resize(imageCount);
    mUICommandBuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        mFrameResources[i] = std::make_shared<FrameResource>(vk::Extent3D{800, 600, 1});
        mFrameResources[i]->InitResource(mContext);
        auto colorAttachment = mFrameResources[i]->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();

        mUICommandBuffers[i] =
            mContext->Device->allocateCommandBuffers(vk::CommandBufferAllocateInfo()
                                                         .setCommandPool(mFrameResources[i]->PresentCommandPool)
                                                         .setLevel(vk::CommandBufferLevel::ePrimary)
                                                         .setCommandBufferCount(1))[0];
        auto commandBuffer = mFrameResources[i]->GraphicsCommandBuffer;
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

    auto imageCount = mSwapChainResource->SwapChainImages.size();
    for (size_t i = 0; i < imageCount; i++)
    {
        auto currentColorAttachment = mFrameResources[i]->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();
        mFrameDescriptorSets[i] =
            ImGui_ImplVulkan_AddTexture(static_cast<VkSampler>(currentColorAttachment->GetSampler()),
                                        static_cast<VkImageView>(currentColorAttachment->GetImageView()),
                                        static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
    }
}
void Editor::Run()
{
    mIsRunning = true;
    auto device = mContext->Device.get();
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UILayout();
        ImGui::Render();
        Render();
    }
    mIsRunning = false;
}
void Editor::Render()
{

    auto device = mContext->Device.get();
    auto currentFrameResource = mFrameResources[mCurrentFrame].get();
    auto mUICommandBuffer = mUICommandBuffers[mCurrentFrame];
    auto currentRenderFinishedSemaphore = currentFrameResource->RenderFinishedSemaphore.get();
    auto currentImageAvailableSemaphore = currentFrameResource->ImageAvailableSemaphore.get();
    auto currentInFlightFence = currentFrameResource->InFlightFence.get();
    auto currentColorClearValue = currentFrameResource->ColorClearValue;

    auto result = device.waitForFences(currentInFlightFence, vk::True,
                                       1000000000); // 1 second timeout
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed to wait for fence: {}", vk::to_string(result));
        return;
    }
    vk::AcquireNextImageInfoKHR acquireInfo{};
    acquireInfo.setTimeout(1000000000)
        .setSwapchain(mSwapChainResource->SwapChain)
        .setSemaphore(currentImageAvailableSemaphore)
        .setDeviceMask(1);
    uint32_t imageIndex;
    auto nextImageResult = device.acquireNextImage2KHR(&acquireInfo, &imageIndex);
    if (nextImageResult == vk::Result::eErrorOutOfDateKHR)
    {
        HandleSwapchainOutOfDate();
        return;
    }
    device.resetFences(currentInFlightFence);

    mRenderSystem->SetFrameResource(currentFrameResource);
    mRenderSystem->Update(1.0);

    // UI Record Command Buffer
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    mUICommandBuffer.begin(beginInfo);
    std::vector<vk::RenderingAttachmentInfo> colorAttachments{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(currentColorClearValue)
            .setImageView(mSwapChainResource->SwapChainImageViews[imageIndex])
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
    preBarrier.setImage(mSwapChainResource->SwapChainImages[imageIndex])
        .setOldLayout(vk::ImageLayout::ePresentSrcKHR)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    preRenderBarrier
        .setImage(currentFrameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    std::vector<vk::ImageMemoryBarrier2> preBarriers = {preBarrier, preRenderBarrier};
    mUICommandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(preBarriers));

    mUICommandBuffer.beginRendering(renderingInfo);
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mUICommandBuffer);
    }
    mUICommandBuffer.endRendering();

    vk::ImageMemoryBarrier2 postBarrier{}, postRenderBarrier{};
    postBarrier.setImage(mSwapChainResource->SwapChainImages[imageIndex])
        .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
        .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eNone)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    postRenderBarrier
        .setImage(currentFrameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    std::vector<vk::ImageMemoryBarrier2> postBarriers = {postBarrier, postRenderBarrier};
    mUICommandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(postBarriers));
    mUICommandBuffer.end();

    vk::SubmitInfo2 submitInfo{};
    std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
        vk::CommandBufferSubmitInfo().setCommandBuffer(mUICommandBuffer),
        vk::CommandBufferSubmitInfo().setCommandBuffer(currentFrameResource->GraphicsCommandBuffer),
    };
    std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos = {
        vk::SemaphoreSubmitInfo()
            .setSemaphore(currentRenderFinishedSemaphore)
            .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)};
    std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos = {
        vk::SemaphoreSubmitInfo()
            .setSemaphore(currentImageAvailableSemaphore)
            .setStageMask(vk::PipelineStageFlagBits2::eTopOfPipe),
    };

    submitInfo.setCommandBufferInfos(commandBufferInfos)
        .setSignalSemaphoreInfos(signalSemaphoreInfos)
        .setWaitSemaphoreInfos(waitSemaphoreInfos);

    mContext->GraphicsQueue.submit2({submitInfo}, currentInFlightFence);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setSwapchains({mSwapChainResource->SwapChain})
        .setImageIndices({imageIndex})
        .setWaitSemaphores({currentRenderFinishedSemaphore});
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
    mCurrentFrame = (mCurrentFrame + 1) % mSwapChainResource->SwapChainImages.size();
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
    auto currentFrameResource = mFrameResources[mCurrentFrame].get();
    ImGui::Image(reinterpret_cast<ImTextureID>(mFrameDescriptorSets[mCurrentFrame]),
                 ImVec2(currentFrameResource->Extent.width, currentFrameResource->Extent.height));
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

void Editor::HandleSwapchainOutOfDate()
{
    mContext->Device->waitIdle();
    mSwapChainResource->ReleaseResource(mContext);
    mSwapChainResource->InitResource(mContext);
    auto imageCount = mSwapChainResource->SwapChainImages.size();
    for (size_t i = 0; i < imageCount; i++)
    {
        auto currentFrameResource = mFrameResources[i].get();
        auto mUICommandBuffer = currentFrameResource->GraphicsCommandBuffer;
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