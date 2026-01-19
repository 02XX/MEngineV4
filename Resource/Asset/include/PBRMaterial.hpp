#pragma once
#include "Material.hpp"
#include "Math.hpp"
#include "PBRMaterialResource.hpp"
#include "Texture2D.hpp"
#include <cstdint>
#include <memory>
namespace MEngine::Resource
{
struct PBRProperties
{
    Vector4 Albedo = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
    Vector4 Normal = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    float Metallic = 0.0f;
    float Roughness = 1.0f;
    float AO = 1.0f;
    float EmissiveIntensity = 1.0f;
    uint32_t AlbedoIndex = 0;
    uint32_t NormalIndex = 0;
    uint32_t ARMIndex = 0;
    uint32_t EmissiveIndex = 0;
    // Bindless Descriptor Indices
    uint32_t AlbedoBindlessIndex = 0;
    uint32_t NormalBindlessIndex = 0;
    uint32_t ARMBindlessIndex = 0;
    uint32_t EmissiveBindlessIndex = 0;
};
struct PBRTextures
{
    std::shared_ptr<Texture2D> Albedo;
    std::shared_ptr<Texture2D> Normal;
    std::shared_ptr<Texture2D> ARM;
    std::shared_ptr<Texture2D> Emissive;
};
class PBRMaterial : public Material
{
    friend class PBRMaterialResource;
    friend class PBRMaterialManager;

  public:
    PBRProperties mProperties;
    PBRTextures mTextures;

  public:
    PBRMaterial(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline, PBRProperties props,
                PBRTextures textures)
        : Material(name, pipeline), mProperties(props), mTextures(textures)
    {
        mResource = std::make_unique<PBRMaterialResource>(this);
    }
    virtual ~PBRMaterial() = default;
};
} // namespace MEngine::Resource