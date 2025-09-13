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
class RHISampler : public RHIResource
{
    friend class MEngine::Resource::Texture2DResource;
    friend class MEngine::Resource::Texture2DArrayResource;
    friend class MEngine::Resource::Texture3DResource;

  protected:
    // sampler
    vk::UniqueSampler mSampler;

    vk::SamplerCreateInfo mSamplerCreateInfo{};
    vk::SamplerAddressMode addressModeU{vk::SamplerAddressMode::eRepeat};
    vk::SamplerAddressMode addressModeV{vk::SamplerAddressMode::eRepeat};
    vk::SamplerAddressMode addressModeW{vk::SamplerAddressMode::eRepeat};
    vk::Filter minFilter{vk::Filter::eLinear};
    vk::Filter magFilter{vk::Filter::eLinear};
    vk::SamplerMipmapMode mipmapMode{vk::SamplerMipmapMode::eLinear};
    float mipLodBias{0.0f};
    float minLod{0.0f};
    float maxLod{0.0f};
    vk::BorderColor borderColor{vk::BorderColor::eFloatOpaqueBlack};
    bool compareEnable{false};
    vk::CompareOp compareOp{vk::CompareOp::eAlways};
    bool anisotropyEnable{false};
    float maxAnisotropy{1.0f};
    vk::Bool32 unnormalizedCoordinates{vk::False};

  public:
    RHISampler() : RHIResource()
    {
    }
    ~RHISampler() override = default;
    inline vk::Sampler GetSampler() const
    {
        return mSampler.get();
    }
};
} // namespace MEngine::Platform