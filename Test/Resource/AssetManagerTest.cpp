#include "AssetManager.hpp"
#include "Context.hpp"
#include "IPendingResourceManager.hpp"
#include "Logger.hpp"
#include "ShaderManager.hpp"
#include "ShaderResource.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <stop_token>
#include <thread>

using namespace MEngine::Resource;
using namespace MEngine::Platform;
class AssetManagerTest : public ::testing::Test
{
  protected:
    std::shared_ptr<Context> mContext;
    // std::jthread mRenderThread;
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
