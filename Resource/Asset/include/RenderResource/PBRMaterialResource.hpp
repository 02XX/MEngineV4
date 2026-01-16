#pragma once
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <cstdint>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
struct PBRMaterialPushConstants
{
    Matrix4 ModelMatrix;
    vk::DeviceAddress SceneSSBOAddress;
    vk::DeviceAddress MaterialSSBOAddress;
    uint32_t PropertiesOffset;
};
class PBRMaterial;
class PBRMaterialResource final : public MaterialResource
{
  public:
    uint32_t mPropertiesOffset{}; // SSBO offset

  public:
    PBRMaterialResource(PBRMaterial *material) : MaterialResource((Material *)material)
    {
    }
    ~PBRMaterialResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource