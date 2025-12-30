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
    RHISemaphore();
    ~RHISemaphore() override = default;
    vk::Semaphore GetSemaphore() const
    {
        return mSemaphore.get();
    }
};
} // namespace MEngine::Platform