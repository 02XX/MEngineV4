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
using RHISamplerDesc = vk::SamplerCreateInfo;
class RHISampler : public RHIResource
{
    friend class MEngine::Resource::Texture2DResource;
    friend class MEngine::Resource::Texture2DArrayResource;
    friend class MEngine::Resource::Texture3DResource;

  protected:
    // sampler
    vk::UniqueSampler mSampler;
    RHISamplerDesc mSamplerDesc{};

  public:
    RHISampler(const RHISamplerDesc &desc);
    ~RHISampler() override = default;
    inline vk::Sampler GetSampler() const
    {
        return mSampler.get();
    }
};
} // namespace MEngine::Platform