#pragma once
#include "Material.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "Texture2D.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.hpp>

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

class PhongMaterial : public Material
{
  public:
    PhongParam mParam{};

  public:
    PhongMaterial(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline)
        : Material(name, pipeline, sizeof(PhongParam))
    {
    }
    virtual void UpdateMaterialData(uint8_t *target)
    {
        size_t offset = 0;
        std::memcpy(target + offset, &mParam, sizeof(PhongParam));
    };
};
} // namespace MEngine::Resource