#pragma once
#include "RHIDescriptorSet.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
#include <cstdint>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Material;
class MaterialResource : public RenderResource
{
  protected:
    Material *mMaterial{};
    RHIHandler<RHIDescriptorSet> mDescriptorSetHandler{};

  public:
    MaterialResource(Material *material) : RenderResource(), mMaterial(material)
    {
    }
    ~MaterialResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    inline RHIHandler<RHIDescriptorSet> GetDescriptorSet() const
    {
        return mDescriptorSetHandler;
    }
    virtual void UpdateDescriptorSet(uint32_t set) {};
};
} // namespace MEngine::Resource