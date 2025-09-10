#pragma once
#include "Material.hpp"
#include "Math.hpp"
namespace MEngine::Resource
{
struct PBRProperties
{
    alignas(16) glm::vec3 Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
    alignas(16) glm::vec3 Normal = glm::vec3(1.0f, 1.0f, 1.0f);
    float Metallic = 0.0f;
    float Roughness = 1.0f;
    float AO = 1.0f;
    float EmissiveIntensity = 1.0f;
};
struct PBRTextures
{
    UUID AlbedoID{};
    UUID NormalID{};
    UUID ARMID{}; // Ambient Occlusion, Roughness, Metallic
    UUID EmissiveID{};
};
class PBRMaterial : public Material
{
  protected:
    PBRProperties mProperties;
    PBRTextures mTextures;

  protected:
    PBRMaterial() : Material()
    {
    }

  public:
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