#include "ShaderManager.hpp"
#include "VulkanContext.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Function;
class ShaderManagerTest : public ::testing::Test
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

TEST_F(ShaderManagerTest, CreateDefaultShader)
{
    std::shared_ptr<ShaderManager> shaderManager = std::make_shared<ShaderManager>(mVulkanContext);
    auto gbufferVert = shaderManager->GetByName("gbuffer_vert");
    EXPECT_NE(gbufferVert, nullptr);
    auto gbufferFrag = shaderManager->GetByName("gbuffer_frag");
    EXPECT_NE(gbufferFrag, nullptr);
    auto lightingVert = shaderManager->GetByName("lighting_vert");
    EXPECT_NE(lightingVert, nullptr);
    auto lightingFrag = shaderManager->GetByName("lighting_frag");
    EXPECT_NE(lightingFrag, nullptr);
}