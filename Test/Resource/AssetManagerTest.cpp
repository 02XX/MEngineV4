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

using namespace MEngine::Resource;
using namespace MEngine::Platform;
class AssetManagerTest : public ::testing::Test
{
  protected:
    std::shared_ptr<Context> mContext;
    // std::jthread mRenderThread;
    vk::UniqueCommandPool GraphicsCommandPool{};
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
        AssetManager::Instance().Init(mContext);
    }
    void TearDown() override
    {
        RenderContext renderContext{mContext, vk::CommandBuffer{}};
        AssetManager::Instance().DestroyAll();
        AssetManager::Instance().ProcessPendingDeletionResources(renderContext);
    }
};
TEST_F(AssetManagerTest, All)
{
    auto assetManager = &AssetManager::Instance();
    auto assets = assetManager->GetAll();
    for (auto asset : assets)
    {
        assetManager->PendingInit(asset->GetResource());
    }
    RenderContext renderContext{mContext, vk::CommandBuffer{}};
    AssetManager::Instance().ProcessPendingInitResources(renderContext);
}
TEST_F(AssetManagerTest, GraphicPipeline)
{
    auto assetManager = &AssetManager::Instance();
    auto pipeline = assetManager->GetByNameAs<GraphicPipeline>(DefaultGraphicPipelineType::ForwardOpaquePhong);
    pipeline->PendingInit();
    assetManager->ProcessPendingInitResources(RenderContext{mContext, vk::CommandBuffer{}});
    vk::UniqueCommandBuffer commandBuffer =
        std::move(mContext->Device
                      ->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{}
                                                         .setCommandPool(GraphicsCommandPool.get())
                                                         .setLevel(vk::CommandBufferLevel::ePrimary)
                                                         .setCommandBufferCount(1))
                      .front());
    commandBuffer->begin(vk::CommandBufferBeginInfo{});
    commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
                                pipeline->GetResourceAs<GraphicPipelineResource>()->mPipeline);
    commandBuffer->end();
}