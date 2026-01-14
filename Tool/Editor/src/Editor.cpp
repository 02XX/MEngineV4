#include "Editor.hpp"
#include "Logger.hpp"
#include "RenderSystem.hpp"
#include <cstddef>
namespace MEngine::Tool
{

Editor::Editor()
{
    Logger::GetInstance().GetLogger()->SetLogLevel(LogLevel::Trace);
    LogInfo("Welcome to MEngine Editor!");
    InitWindow();
    InitVulkan();
    InitImGui();
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
    if (mSwapChainResource)
    {
        mSwapChainResource->ReleaseResource(mContext);
        mSwapChainResource.reset();
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
    mImageAvailableSemaphores.resize(imageCount);
    mRenderFinishedSemaphores.resize(imageCount);
    mInFlightFences.resize(imageCount);
    mGraphicCommandPools.resize(imageCount);
    mGraphicCommandBuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        mImageAvailableSemaphores[i] = mContext->Device->createSemaphoreUnique(vk::SemaphoreCreateInfo{});
        mRenderFinishedSemaphores[i] = mContext->Device->createSemaphoreUnique(vk::SemaphoreCreateInfo{});
        mInFlightFences[i] =
            mContext->Device->createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
        mGraphicCommandPools[i] = mContext->Device->createCommandPoolUnique(
            vk::CommandPoolCreateInfo()
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                .setQueueFamilyIndex(mContext->QueueFamilyIndicates.graphicsFamily.value()));
        mGraphicCommandBuffers[i] =
            std::move(mContext->Device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo()
                                                                         .setCommandPool(mGraphicCommandPools[i].get())
                                                                         .setLevel(vk::CommandBufferLevel::ePrimary)
                                                                         .setCommandBufferCount(1))[0]);
        vk::ImageMemoryBarrier2 barrier{};
        barrier.setImage(mSwapChainResource->SwapChainImages[i])
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
            .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        mGraphicCommandBuffers[i]->begin(vk::CommandBufferBeginInfo{});
        mGraphicCommandBuffers[i]->pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(barrier));
        mGraphicCommandBuffers[i]->end();
        vk::SubmitInfo2 submitInfo{};
        std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
            vk::CommandBufferSubmitInfo().setCommandBuffer(mGraphicCommandBuffers[i].get()),
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
}
// void Editor::CreateViewPortDescriptorSets()
// {
//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//     {
//         if (mViewPortDescriptorSets[i] != VK_NULL_HANDLE)
//         {
//             ImGui_ImplVulkan_RemoveTexture(mViewPortDescriptorSets[i]);
//         }
//         auto colorTexture = mFrameResources[i]->ColorTextures->GetResourceAs<TextureRenderTarget2DResource>();
//         mViewPortDescriptorSets[i] =
//             ImGui_ImplVulkan_AddTexture(static_cast<VkSampler>(colorTexture->GetSampler()->GetSampler()),
//                                         static_cast<VkImageView>(colorTexture->GetTextureView()->GetImageView()),
//                                         static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
//     }
// }
void Editor::Run()
{
    auto device = mContext->Device.get();
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UILayout();
        ImGui::Render();

        auto result = device.waitForFences(mInFlightFences[mCurrentFrame].get(), vk::True,
                                           1000000000); // 1 second timeout
        if (result != vk::Result::eSuccess)
        {
            LogError("Failed to wait for fence: {}", vk::to_string(result));
            continue;
        }

        vk::AcquireNextImageInfoKHR acquireInfo{};
        acquireInfo.setTimeout(1000000000)
            .setSwapchain(mSwapChainResource->SwapChain)
            .setSemaphore(mImageAvailableSemaphores[mCurrentFrame].get())
            .setDeviceMask(1);
        uint32_t imageIndex;
        auto nextImageResult = device.acquireNextImage2KHR(&acquireInfo, &imageIndex);
        if (nextImageResult == vk::Result::eErrorOutOfDateKHR)
        {
            HandleSwapchainOutOfDate();
            continue;
        }

        device.resetFences(mInFlightFences[mCurrentFrame].get());

        auto currentCommandBuffer = mGraphicCommandBuffers[mCurrentFrame].get();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        currentCommandBuffer.begin(beginInfo);
        std::vector<vk::RenderingAttachmentInfo> colorAttachments{
            // Color
            vk::RenderingAttachmentInfo()
                .setClearValue(mColorClearValue)
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
        vk::ImageMemoryBarrier2 preBarrier{};
        preBarrier.setImage(mSwapChainResource->SwapChainImages[imageIndex])
            .setOldLayout(vk::ImageLayout::ePresentSrcKHR)
            .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        currentCommandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(preBarrier));
        currentCommandBuffer.beginRendering(renderingInfo);
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), currentCommandBuffer);
        }
        currentCommandBuffer.endRendering();
        vk::ImageMemoryBarrier2 postBarrier{};
        postBarrier.setImage(mSwapChainResource->SwapChainImages[imageIndex])
            .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
            .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        currentCommandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(postBarrier));
        currentCommandBuffer.end();
        vk::SubmitInfo2 submitInfo{};
        std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
            vk::CommandBufferSubmitInfo().setCommandBuffer(currentCommandBuffer),
        };
        std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos = {
            vk::SemaphoreSubmitInfo()
                .setSemaphore(mRenderFinishedSemaphores[mCurrentFrame].get())
                .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput),
        };
        std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos = {
            vk::SemaphoreSubmitInfo()
                .setSemaphore(mImageAvailableSemaphores[mCurrentFrame].get())
                .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput),
        };
        submitInfo.setCommandBufferInfos(commandBufferInfos)
            .setSignalSemaphoreInfos(signalSemaphoreInfos)
            .setWaitSemaphoreInfos(waitSemaphoreInfos);
        mContext->GraphicsQueue.submit2(submitInfo, mInFlightFences[mCurrentFrame].get());

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setSwapchains({mSwapChainResource->SwapChain})
            .setImageIndices({imageIndex})
            .setWaitSemaphores({mRenderFinishedSemaphores[mCurrentFrame].get()});
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
        vk::ImageMemoryBarrier2 barrier{};
        barrier.setImage(mSwapChainResource->SwapChainImages[i])
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
            .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        mGraphicCommandBuffers[i]->begin(vk::CommandBufferBeginInfo{});
        mGraphicCommandBuffers[i]->pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(barrier));
        mGraphicCommandBuffers[i]->end();
        vk::SubmitInfo2 submitInfo{};
        std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
            vk::CommandBufferSubmitInfo().setCommandBuffer(mGraphicCommandBuffers[i].get()),
        };
        submitInfo.setCommandBufferInfos(commandBufferInfos);
        mContext->GraphicsQueue.submit2(submitInfo, {});
        mContext->Device->waitIdle();
    }
}
} // namespace MEngine::Tool