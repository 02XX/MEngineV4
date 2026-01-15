#pragma once
#include "MaterialResource.hpp"
#include "RenderResource.hpp"
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class PBRMaterial;
class PBRMaterialResource final : public MaterialResource
{
  private:
    // BDA(Buffer Device Address) Vulkan 1.2+
    vk::DeviceAddress mPropertiesBufferAddress{};

  public:
    PBRMaterialResource(PBRMaterial *material) : MaterialResource((Material *)material)
    {
    }
    ~PBRMaterialResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource