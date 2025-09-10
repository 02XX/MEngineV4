#include "GraphicPipelineManager.hpp"
#include "RenderPassManager.hpp"
#include "ShaderManager.hpp"
#include "VulkanContext.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Function;
class PipelineManagerTest : public ::testing::Test
{
  protected:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager{};
    std::shared_ptr<RenderPassManager> renderPassManager{};
    std::shared_ptr<ShaderManager> shaderManager{};
    void SetUp() override
    {
        mVulkanContext = std::make_shared<VulkanContext>();
        VulkanContextConfig config;
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        mVulkanContext->InitContext(config);
        mVulkanContext->Init();
        pipelineLayoutManager = std::make_shared<PipelineLayoutManager>(mVulkanContext);
        renderPassManager = std::make_shared<RenderPassManager>(mVulkanContext);
        shaderManager = std::make_shared<ShaderManager>(mVulkanContext);
    }
    void TearDown() override
    {
    }
};

TEST_F(PipelineManagerTest, CreateDefaultPipeline)
{
    auto pipelineManager = std::make_shared<GraphicPipelineManager>(mVulkanContext, pipelineLayoutManager,
                                                                    renderPassManager, shaderManager);
    auto gBufferPipeline = pipelineManager->GetByName("GBuffer");
    EXPECT_NE(gBufferPipeline, nullptr);
    auto lightingPipeline = pipelineManager->GetByName("Lighting");
    EXPECT_NE(lightingPipeline, nullptr);
}