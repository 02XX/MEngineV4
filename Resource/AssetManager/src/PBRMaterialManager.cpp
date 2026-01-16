#include "PBRMaterialManager.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
namespace MEngine::Resource
{

void PBRMaterialManager::CreateDefault()
{
    auto gBufferMaterial = CreateGBufferOpaqueMaterial();
    auto lightingMaterial = CreateLightMaterial();
    gBufferMaterial->mID = mDefaultMaterials.at(DefaultMaterialType::GBufferPBROpaque);
    lightingMaterial->mID = mDefaultMaterials.at(DefaultMaterialType::GBufferPBRTransparent);
    Add(gBufferMaterial);
    Add(lightingMaterial);
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateGBufferOpaqueMaterial()
{
    auto pipeline = mPipelineManager->GetPipeline(GraphicPipelineType::GBuffer);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto material = std::make_shared<PBRMaterial>("GBufferPBR_Opaque", pipeline, props, textures);
    LogInfo("Created「Default GBufferPBR_Opaque」material");
    return material;
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateLightMaterial()
{
    auto pipeline = mPipelineManager->GetPipeline(GraphicPipelineType::Lighting);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto material = std::make_shared<PBRMaterial>("Lighting_PBR", pipeline, props, textures);
    LogInfo("Created「Default Lighting_PBR」material");
    return material;
}
} // namespace MEngine::Resource