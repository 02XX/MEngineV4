#pragma once
#include "MaterialResource.hpp"
#include "RHIBuffer.hpp"
#include "RHIDescriptorSet.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class PBRMaterial;
class PBRMaterialResource final : public MaterialResource
{
  private:
    RHIHandler<RHIBuffer> mRHIUniformBufferHandler{};

  public:
    PBRMaterialResource(PBRMaterial *material) : MaterialResource((Material *)material)
    {
    }
    ~PBRMaterialResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    void UpdateDescriptorSet(uint32_t set) override;
};
} // namespace MEngine::Resource