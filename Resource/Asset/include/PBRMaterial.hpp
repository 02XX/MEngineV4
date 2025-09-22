#pragma once
#include "Material.hpp"
#include "Math.hpp"
#include "PBRMaterialResource.hpp"
#include "Texture2D.hpp"
#include <format>
#include <memory>
namespace MEngine::Resource
{
struct PBRProperties
{
    alignas(16) Vector3 Albedo = Vector3(1.0f, 1.0f, 1.0f);
    alignas(16) Vector3 Normal = Vector3(1.0f, 1.0f, 1.0f);
    float Metallic = 0.0f;
    float Roughness = 1.0f;
    float AO = 1.0f;
    float EmissiveIntensity = 1.0f;
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

  protected:
    PBRProperties mProperties;
    PBRTextures mTextures;

  protected:
    PBRMaterial() : Material()
    {
        mName = std::format("PBRMaterial_{}", GetID().ToString());
        mResource = std::make_unique<PBRMaterialResource>(this);
    }

  public:
    PBRMaterial(const std::string &name, const std::shared_ptr<GraphicPipeline> &pipeline, const PBRProperties &props,
                const PBRTextures &textures)
        : Material(name, pipeline), mProperties(props), mTextures(textures)
    {
        mResource = std::make_unique<PBRMaterialResource>(this);
    }
    virtual ~PBRMaterial() = default;
    inline const PBRProperties &GetProperties() const
    {
        return mProperties;
    }
    inline const PBRTextures &GetTextures() const
    {
        return mTextures;
    }
};
} // namespace MEngine::Resource