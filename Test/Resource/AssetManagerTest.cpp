#include "AssetManager.hpp"
#include "Shader.hpp"
#include <gtest/gtest.h>
#include <memory>
using namespace MEngine::Resource;

namespace MEngine::Resource
{
TEST(AssetManagerTest, SaveAndLoadShader)
{
    AssetManager assetManager;
    // Create a Shader asset
    auto shader = std::shared_ptr<Shader>(new Shader());
    shader->mName = "TestShader";
    shader->mStage = vk::ShaderStageFlagBits::eVertex;
    shader->mGLSLFilePath = "shaders/test_shader.vert";
    shader->mGLSLSource = "#version 450\nvoid main() {}";
    shader->mSPIRVFilePath = "shaders/test_shader.spv";
    shader->mSPIRVCode = {0x07230203, 0x00010000, 0x0008000a, 0x0000000b}; // Dummy SPIR-V code

    AssetURL url("asset://test_shader.asset");

    // Save the Shader asset
    assetManager.SaveAsset(shader, url);

    // Load the Shader asset
    auto loadedShader = assetManager.LoadAsset<Shader>(url);
    ASSERT_NE(loadedShader, nullptr);
    EXPECT_EQ(loadedShader->GetName(), "TestShader");
    EXPECT_EQ(loadedShader->GetStage(), vk::ShaderStageFlagBits::eVertex);
    EXPECT_EQ(loadedShader->mGLSLFilePath, "shaders/test_shader.vert");
    EXPECT_EQ(loadedShader->mGLSLSource, "#version 450\nvoid main() {}");
    EXPECT_EQ(loadedShader->mSPIRVFilePath, "shaders/test_shader.spv");
    EXPECT_EQ(loadedShader->mSPIRVCode, (std::vector<uint32_t>{0x07230203, 0x00010000, 0x0008000a, 0x0000000b}));
}
} // namespace MEngine::Resource