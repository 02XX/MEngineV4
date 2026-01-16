#include "AssetManager.hpp"
#include "GraphicPipelineManager.hpp"
#include "MeshManager.hpp"
#include "PBRMaterialManager.hpp"
#include "ShaderManager.hpp"
#include "Texture2DManager.hpp"
#include <memory>

namespace MEngine::Resource
{
AssetManager::AssetManager(std::shared_ptr<Context> context) : mContext(context)
{
    auto shaderManager = std::make_shared<ShaderManager>();
    auto textureManager = std::make_shared<Texture2DManager>();
    auto graphicPipelineManager = std::make_shared<GraphicPipelineManager>(mContext, shaderManager);
    auto meshManager = std::make_shared<MeshManager>();
    auto pbrMaterialManager = std::make_shared<PBRMaterialManager>(textureManager, graphicPipelineManager);
    RegisterManager<Shader>(shaderManager);
    RegisterManager<Texture2D>(textureManager);
    RegisterManager<GraphicPipeline>(graphicPipelineManager);
    RegisterManager<StaticMesh>(meshManager);
    RegisterManager<PBRMaterial>(pbrMaterialManager);
}
} // namespace MEngine::Resource