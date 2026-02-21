#include "AssetManager.hpp"
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "IPendingResourceManager.hpp"
#include "Logger.hpp"
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
        AssetManager::Instance().Init(mContext);
    }
    void TearDown() override
    {
        RenderContext renderContext{mContext, vk::CommandBuffer{}};
        AssetManager::Instance().Shutdown(mContext);
    }
};
TEST_F(AssetManagerTest, All)
{
}
