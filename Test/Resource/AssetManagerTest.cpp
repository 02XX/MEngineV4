#include "AssetManager.hpp"
#include "Context.hpp"
#include "GraphicPipelineManager.hpp"
#include "MeshManager.hpp"
#include "ShaderManager.hpp"
#include "Texture2DManager.hpp"
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
    auto shaders = shaderManager->GetAll();
    for (auto shader : shaders)
    {
        shader->GetResource()->InitResource(mContext);
    }
}
// TEST_F(AssetManagerTest, GraphicPipelineManager)
// {
//     auto shaderManager = std::make_shared<ShaderManager>();
//     auto graphicPipelineManager = std::make_shared<GraphicPipelineManager>(mContext, shaderManager);
//     auto pipelines = graphicPipelineManager->GetAll();
//     for (auto pipeline : pipelines)
//     {
//         pipeline->GetResource()->InitResource(mContext);
//     }
// }
TEST_F(AssetManagerTest, Texture2DManager)
{
    auto textureManager = std::make_shared<Texture2DManager>();
    auto textures = textureManager->GetAll();
    for (auto texture : textures)
    {
        texture->GetResource()->InitResource(mContext);
    }
}
TEST_F(AssetManagerTest, MeshManager)
{
    auto meshManager = std::make_shared<MeshManager>();
    auto meshes = meshManager->GetAll();
    for (auto mesh : meshes)
    {
        mesh->GetResource()->InitResource(mContext);
    }
}