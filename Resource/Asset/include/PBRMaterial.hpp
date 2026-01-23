#pragma once
#include "Material.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "Texture2D.hpp"
#include <cstdint>
#include <memory>
namespace MEngine::Resource
{
struct PBRParm
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
class PBRMaterial : public Material
{
  public:
    PBRParm mParam{};

  public:
    PBRMaterial(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline)
        : Material(name, pipeline, sizeof(PBRParm))
    {
    }
    virtual void UpdateMaterialData(uint8_t *target)
    {
        size_t offset = 0;
        std::memcpy(target + offset, &mParam, sizeof(PBRParm));
    };
};
} // namespace MEngine::Resource