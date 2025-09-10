#include "Shader.hpp"
#include "ShaderBuilder.hpp"
#include "ShaderDirector.hpp"
#include "ShaderUtils.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Function;
class ShaderTest : public ::testing::Test
{
  protected:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    std::string shaderSource = R"(#version 450
void main() {
})";
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
TEST_F(ShaderTest, BuildFromGLSLFile)
{
    std::ofstream shaderFile("test_shader.vert", std::ios::out);
    shaderFile << shaderSource;
    shaderFile.close();
    ShaderDirector director("TestShader", std::filesystem::path("test_shader.vert"), vk::ShaderStageFlagBits::eVertex);
    ShaderBuilder builder(mVulkanContext);
    auto shader = director.Make(builder);
    EXPECT_EQ(shader->GetName(), "TestShader");
    EXPECT_EQ(shader->GetStage(), vk::ShaderStageFlagBits::eVertex);
    EXPECT_TRUE(shader->GetShaderModule() != nullptr);
    std::filesystem::remove("test_shader.vert");
}
TEST_F(ShaderTest, BuildFromGLSLSource)
{
    ShaderDirector director("TestShader", shaderSource, vk::ShaderStageFlagBits::eVertex);
    ShaderBuilder builder(mVulkanContext);
    auto shader = director.Make(builder);
    EXPECT_EQ(shader->GetName(), "TestShader");
    EXPECT_EQ(shader->GetStage(), vk::ShaderStageFlagBits::eVertex);
    EXPECT_TRUE(shader->GetShaderModule() != nullptr);
}
TEST_F(ShaderTest, BuildFromSPIRVCode)
{
    // // Dummy SPIR-V code for testing
    auto sprivResult = ShaderUtils::CompileShader(shaderSource, shaderc_vertex_shader, "TestShader");
    std::vector<uint32_t> spirvCode(sprivResult.begin(), sprivResult.end());
    ShaderDirector director("TestShader", spirvCode, vk::ShaderStageFlagBits::eVertex);
    ShaderBuilder builder(mVulkanContext);
    auto shader = director.Make(builder);
    EXPECT_EQ(shader->GetName(), "TestShader");
    EXPECT_EQ(shader->GetStage(), vk::ShaderStageFlagBits::eVertex);
    EXPECT_TRUE(shader->GetShaderModule() != nullptr);
}