#include "AssetManager.hpp"
#include "GraphicPipelineManager.hpp"
#include "MeshManager.hpp"
#include "PBRMaterialManager.hpp"
#include "ShaderManager.hpp"
#include "Texture2DManager.hpp"
#include <memory>

namespace MEngine::Resource
{
AssetManager::AssetManager()
{
    auto shaderManager = std::make_shared<ShaderManager>();
    auto textureManager = std::make_shared<Texture2DManager>();
    auto meshManager = std::make_shared<MeshManager>();
    auto pipelineManager = std::make_shared<GraphicPipelineManager>(shaderManager);
    auto pbrMaterialManager = std::make_shared<PBRMaterialManager>(textureManager, pipelineManager);
    RegisterManager<Shader>(shaderManager);
    RegisterManager<Texture2D>(textureManager);
    RegisterManager<StaticMesh>(meshManager);
    RegisterManager<GraphicPipeline>(pipelineManager);
    RegisterManager<PBRMaterial>(pbrMaterialManager);
}
} // namespace MEngine::Resource