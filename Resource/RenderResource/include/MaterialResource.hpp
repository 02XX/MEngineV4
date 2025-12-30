#pragma once
#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
#include "RHIDescriptorSet.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
#include <array>
#include <cstdint>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Material;
class MaterialResource : public RenderResource
{
  protected:
    Material *mMaterial{};
    std::array<RHIHandler<RHIDescriptorSet>, MAX_FRAMES_IN_FLIGHT> mDescriptorSetHandlers{};

  public:
    MaterialResource(Material *material) : RenderResource(), mMaterial(material)
    {
    }
    ~MaterialResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    inline RHIHandler<RHIDescriptorSet> GetDescriptorSet(int frameIndex) const
    {
        return mDescriptorSetHandlers[frameIndex];
    }
    virtual void UpdateDescriptorSet(int frameIndex)
    {
        throw std::runtime_error("MaterialResource::UpdateDescriptorSet not implemented");
    };
};
} // namespace MEngine::Resource