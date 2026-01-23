#pragma once
#include "Material.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "PhongMaterialResource.hpp"
#include "Texture2D.hpp"
#include <cstdint>
#include <memory>

namespace MEngine::Resource
{
struct PhongParams
{
    Vector4 Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    Vector4 Specular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    float Shininess = 32.0f;
    // Bindless Descriptor Indices
    uint32_t DiffuseBindlessIndex = 0;
    uint32_t SpecularBindlessIndex = 0;
};
struct PhongTextures
{
    std::shared_ptr<Texture2D> DiffuseTexture{};
    std::shared_ptr<Texture2D> SpecularTexture{};
};
class PhongMaterial : public Material
{
  public:
    PhongParams mParam{};
    PhongTextures mTextures{};

  public:
    PhongMaterial(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline, PhongParams params,
                  PhongTextures textures, bool dynamic = false)
        : Material(name, pipeline, dynamic), mParam(params), mTextures(textures)
    {
        mResource = std::make_unique<PhongMaterialResource>(this);
    }
    void PendingUpdate() override
    {
        Material::PendingUpdate();
        mTextures.DiffuseTexture->PendingUpdate();
        mTextures.SpecularTexture->PendingUpdate();
    }
};
} // namespace MEngine::Resource