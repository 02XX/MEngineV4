#pragma once
#include "Material.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "PBRMaterialResource.hpp"
#include "Texture2D.hpp"
#include <cstdint>
#include <memory>
namespace MEngine::Resource
{
struct PBRParams
{
    Vector4 Albedo = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
    Vector4 Normal = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    float Metallic = 0.0f;
    float Roughness = 1.0f;
    float AO = 1.0f;
    float EmissiveIntensity = 1.0f;
    // Bindless Descriptor Indices
    uint32_t AlbedoBindlessIndex = 0;
    uint32_t NormalBindlessIndex = 0;
    uint32_t ARMBindlessIndex = 0;
    uint32_t EmissiveBindlessIndex = 0;
};
struct PBRTextures
{
    std::shared_ptr<Texture2D> AlbedoTexture{};
    std::shared_ptr<Texture2D> NormalTexture{};
    std::shared_ptr<Texture2D> ARMTexture{};
    std::shared_ptr<Texture2D> EmissiveTexture{};
};
class PBRMaterial : public Material
{
  public:
    PBRParams mParam{};
    PBRTextures mTextures{};

  public:
    PBRMaterial(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline, PBRParams parms,
                PBRTextures textures, bool dynamic = false)
        : Material(name, pipeline, dynamic), mParam(parms), mTextures(textures)
    {
        mResource = std::make_unique<PBRMaterialResource>(this);
    }
    void PendingUpdate() override
    {
        Material::PendingUpdate();
        mTextures.AlbedoTexture->PendingUpdate();
        mTextures.NormalTexture->PendingUpdate();
        mTextures.ARMTexture->PendingUpdate();
        mTextures.EmissiveTexture->PendingUpdate();
    }
};
} // namespace MEngine::Resource