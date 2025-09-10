
#include "PipelineLayoutManager.hpp"
#include "VulkanContext.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Function;
class PipelineLayoutManagerTest : public ::testing::Test
{
  protected:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    void SetUp() override
    {
        mVulkanContext = std::make_shared<VulkanContext>();
        VulkanContextConfig config;
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        mVulkanContext->InitContext(config);
        mVulkanContext->Init();
    }
    void TearDown() override
    {
    }
};

TEST_F(PipelineLayoutManagerTest, CreateDefaultPipeline)
{
    auto pipelineLayoutManager = std::make_shared<PipelineLayoutManager>(mVulkanContext);
    auto gBufferPBRPipelineLayout = pipelineLayoutManager->GetByType(PipelineLayoutType::GBufferPBR);
    EXPECT_NE(gBufferPBRPipelineLayout, nullptr);
    auto lightingPBRPipelineLayout = pipelineLayoutManager->GetByType(PipelineLayoutType::LightingPBR);
    EXPECT_NE(lightingPBRPipelineLayout, nullptr);
}