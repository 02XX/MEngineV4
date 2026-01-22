#pragma once
#include "Asset.hpp"
#include "Context.hpp"
#include "TextureResource.hpp"
#include <string>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace MEngine::Resource
{

struct TextureSetting : public vk::ImageCreateInfo, public vk::SamplerCreateInfo
{
};
struct MipMap
{
    std::vector<std::vector<uint8_t>> Datas{}; //[arrayLayer]
    vk::Extent3D Extent{};
};
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UTexture
// Abstract Class
class Texture : public Asset
{
    friend class TextureResource;

  public:
    TextureSetting mTextureSettings{
        vk::ImageCreateInfo()
            .setFormat(vk::Format::eR8G8B8A8Unorm)
            .setExtent(vk::Extent3D{1, 1, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled |
                      vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setInitialLayout(vk::ImageLayout::eUndefined),
        vk::SamplerCreateInfo()
            .setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setMipLodBias(0.0f)
            .setAnisotropyEnable(vk::True)
            .setMaxAnisotropy(16.0f)
            .setCompareEnable(vk::False)
            .setCompareOp(vk::CompareOp::eAlways)
            .setMinLod(0.0f)
            .setMaxLod(10.0f)
            .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
            .setUnnormalizedCoordinates(vk::False)};

  protected:
    Texture() : Asset()
    {
        mAssetType = AssetType::Texture;
    }

  public:
    Texture(const std::string &name, const TextureSetting &textureSetting)
        : Asset(name), mTextureSettings(textureSetting)
    {
    }
    ~Texture() override = 0;
    bool IsDepthStencil() const
    {
        return (mTextureSettings.format == vk::Format::eD16Unorm ||
                mTextureSettings.format == vk::Format::eX8D24UnormPack32 ||
                mTextureSettings.format == vk::Format::eD32Sfloat ||
                mTextureSettings.format == vk::Format::eD16UnormS8Uint ||
                mTextureSettings.format == vk::Format::eD24UnormS8Uint ||
                mTextureSettings.format == vk::Format::eD32SfloatS8Uint);
    }
};
inline Texture::~Texture()
{
}
} // namespace MEngine::Resource
