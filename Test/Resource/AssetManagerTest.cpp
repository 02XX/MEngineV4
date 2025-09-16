#include "AssetManager.hpp"
#include "RHIContext.hpp"
#include <Texture2D.hpp>
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Resource;
// class AssetManagerTest : public ::testing::Test
// {
//   protected:
//     void SetUp() override
//     {
//         auto &rhiContext = MEngine::Platform::RHIContext::Instance();
//         RHIContextConfig config;
//         config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
//         config.DeviceRequiredExtensions = {"VK_EXT_host_image_copy"};
//         rhiContext.InitInstance(config);
//         rhiContext.InitContext();
//     }
//     void TearDown() override
//     {
//     }
// };
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
TEST(AssetManagerTest, SaveAndLoadTexture2D)
{
    auto &rhiContext = MEngine::Platform::RHIContext::Instance();
    RHIContextConfig config;
    config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
    rhiContext.InitInstance(config);
    rhiContext.InitContext();
    AssetManager assetManager;
    // Create a Texture2D asset
    auto texture = std::shared_ptr<Texture2D>(new Texture2D());
    texture->mTextureImportSettings.setExtent({2, 2, 1});
    texture->mTextureImportSettings.format = vk::Format::eR8G8B8A8Unorm;
    texture->mTextureImportSettings.mipLevels = 2;
    texture->mTextureData = {
        Texture2DMipMap{
            .Data = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255}, // Red, Green, Blue, Yellow
            .SizeX = 2,
            .SizeY = 2,
            .SizeZ = 1,
        },
        Texture2DMipMap{
            .Data = {128, 128, 128, 255}, // Gray
            .SizeX = 1,
            .SizeY = 1,
            .SizeZ = 1,
        },
    };
    assetManager.SaveAsset(texture, AssetURL("asset://test_texture.asset"));
    // Load the Texture2D asset
    auto loadedTexture = assetManager.LoadAsset<Texture2D>(AssetURL("asset://test_texture.asset"));

    // GPU
    loadedTexture->GetResource()->InitRHI();
    loadedTexture->GetResource()->ReleaseRHI();
    while (PendingDeletes.Size() > 0)
    {
        auto resource = PendingDeletes.Consume();
        delete resource;
    }
}

} // namespace MEngine::Resource