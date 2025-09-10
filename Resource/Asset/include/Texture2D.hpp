#pragma once
#include "Texture.hpp"

using namespace MEngine::Core;
namespace MEngine::Resource
{
class Texture2D : public Texture
{
  private:
  protected:
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

  protected:
    Texture2D() : Texture()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 1;
    }

  public:
    ~Texture2D() override = default;
    inline vk::Sampler GetSampler()
    {
        return mSampler.get();
    }
};
} // namespace MEngine::Resource