#pragma once
#include "RHIResource.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Resource
{
class Texture2DResource;
class Texture2DArrayResource;
class Texture3DResource;
} // namespace MEngine::Resource
namespace MEngine::Platform
{
class RHISemaphore : public RHIResource
{
  protected:
    // sampler
    vk::UniqueSemaphore mSemaphore;

  public:
    RHISemaphore(const RHISeamphoreDesc &desc);
    ~RHISemaphore() override = default;
    inline vk::Sampler GetSampler() const
    {
        return mSampler.get();
    }
};
} // namespace MEngine::Platform