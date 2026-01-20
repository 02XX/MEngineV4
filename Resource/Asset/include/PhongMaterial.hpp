#pragma once
#include "Material.hpp"
#include "Math.hpp"
#include "PhongMaterialResource.hpp"
#include "Texture2D.hpp"
#include <cstdint>
#include <memory>
namespace MEngine::Resource
{
struct PhongParam
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
    std::shared_ptr<Texture2D> mDiffuseTexture{};
    std::shared_ptr<Texture2D> mSpecularTexture{};
};
class PhongMaterial : public Material
{
    friend class PhongMaterialResource;
    friend class PhongMaterialManager;

  public:
    PhongParam mParam{};
    PhongTextures mTextures{};

  public:
    PhongMaterial(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline, PhongParam param,
                  PhongTextures textures)
        : Material(name, pipeline), mParam(param), mTextures(textures)
    {
        mResource = std::make_unique<PhongMaterialResource>(this);
    }
    virtual ~PhongMaterial() = default;
};
} // namespace MEngine::Resource