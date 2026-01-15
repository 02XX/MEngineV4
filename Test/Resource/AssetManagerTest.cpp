#include "AssetManager.hpp"
#include "Context.hpp"
#include "GraphicPipelineManager.hpp"
#include "ShaderManager.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Resource;
using namespace MEngine::Platform;
class AssetManagerTest : public ::testing::Test
{
  protected:
    std::shared_ptr<Context> mContext;
    void SetUp() override
    {
        ContextConfig config{};
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        mContext = std::make_shared<Context>(config);
    }
    void TearDown() override
    {
        std::function<void(std::shared_ptr<Context> context)> item{};
        while (PendingDeletions.TryPop(item))
        {
            item(mContext);
        }
    }
};
TEST_F(AssetManagerTest, ShaderManager)
{
    auto shaderManager = std::make_shared<ShaderManager>();
}
TEST_F(AssetManagerTest, GraphicPipelineManager)
{
    auto shaderManager = std::make_shared<ShaderManager>();
    auto graphicPipelineManager = std::make_shared<GraphicPipelineManager>(mContext, shaderManager);
}
