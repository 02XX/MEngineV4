#include "AssetManager.hpp"
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "IPendingResourceManager.hpp"
#include "Logger.hpp"
#include "PipelineManager.hpp"
#include "ShaderManager.hpp"
#include "ShaderResource.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <stop_token>
#include <thread>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

using namespace MEngine::Resource;
using namespace MEngine::Platform;
class AssetManagerTest : public ::testing::Test
{
  protected:
    std::shared_ptr<Context> mContext;
    // std::jthread mRenderThread;
    vk::UniqueCommandPool GraphicsCommandPool{};
    vk::UniqueCommandBuffer GraphicsCommandBuffer{};
    void SetUp() override
    {
        Logger::GetInstance().GetLogger()->SetLogLevel(LogLevel::Trace);
        ContextConfig config{};
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        mContext = std::make_shared<Context>(config);
        // mRenderThread = std::jthread([&](std::stop_token stoken) {
        //     while (!stoken.stop_requested())
        //     {
        //         RenderContext renderContext{mContext, vk::CommandBuffer{}};
        //         AssetManager::Instance().ProcessPendingInitResources(renderContext);
        //         AssetManager::Instance().ProcessPendingUpdateResources(renderContext);
        //         AssetManager::Instance().ProcessPendingDeletionResources(renderContext);
        //         std::this_thread::sleep_for(std::chrono::milliseconds(16));
        //     }
        // });
        vk::CommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.setQueueFamilyIndex(mContext->QueueFamilyIndicates.graphicsFamily.value())
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        GraphicsCommandPool = mContext->Device->createCommandPoolUnique(commandPoolCreateInfo);
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.setCommandPool(GraphicsCommandPool.get())
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        auto commandBuffers = mContext->Device->allocateCommandBuffersUnique(commandBufferAllocateInfo);
        GraphicsCommandBuffer = std::move(commandBuffers.front());
        AssetManager::Instance().Init(mContext);
    }
    void TearDown() override
    {
        RenderContext renderContext{mContext, vk::CommandBuffer{}};
        AssetManager::Instance().ProcessPendingDeletionResources(renderContext);
    }
};
TEST_F(AssetManagerTest, All)
{
}
