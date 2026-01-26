#include "TextureResource.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"
#include "RenderResource.hpp"
#include "Texture.hpp"
#include <memory>
namespace MEngine::Resource
{
TextureResource::TextureResource(Texture *texture) : RenderResource(texture)
{
}
TextureResource::~TextureResource()
{
}
void TextureResource::InitRHI(std::shared_ptr<Context> context)
{
    auto texture = static_cast<Texture *>(mOwnerAsset);
    auto instance = context->Instance.get();
    auto device = context->Device.get();

    texture->mTextureSettings.usage |=
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;

    VmaAllocationCreateInfo imageAllocationCreateInfo{};
    imageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    imageAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    auto result =
        vmaCreateImage(context->VmaAllocator, reinterpret_cast<VkImageCreateInfo *>(&texture->mTextureSettings),
                       &imageAllocationCreateInfo, reinterpret_cast<VkImage *>(&mImage), &mImageAllocation,
                       reinterpret_cast<VmaAllocationInfo *>(&mImageAllocationInfo));
    if (result != VK_SUCCESS)
    {
        LogError("Failed to create image with VMA");
        return;
    }
    mSampler = device.createSampler(static_cast<vk::SamplerCreateInfo>(texture->mTextureSettings));

    vk::ImageViewCreateInfo imageviewCreateInfo{};
    imageviewCreateInfo.setImage(mImage)
        .setComponents({})
        .setFormat(texture->mTextureSettings.format)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(texture->IsDepthStencil() ? vk::ImageAspectFlagBits::eDepth
                                                                          : vk::ImageAspectFlagBits::eColor)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(texture->mTextureSettings.mipLevels)
                                 .setBaseArrayLayer(0)
                                 .setLayerCount(texture->mTextureSettings.arrayLayers));
    switch (texture->mTextureSettings.imageType)
    {
    case vk::ImageType::e1D: {
        imageviewCreateInfo.viewType =
            texture->mTextureSettings.arrayLayers > 1 ? vk::ImageViewType::e1DArray : vk::ImageViewType::e1D;
        break;
    }
    case vk::ImageType::e2D: {
        if (static_cast<vk::ImageCreateInfo &>(texture->mTextureSettings).flags &
            vk::ImageCreateFlagBits::eCubeCompatible)
        {
            imageviewCreateInfo.viewType =
                texture->mTextureSettings.arrayLayers > 6 ? vk::ImageViewType::eCubeArray : vk::ImageViewType::eCube;
        }
        else
        {
            imageviewCreateInfo.viewType =
                texture->mTextureSettings.arrayLayers > 1 ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D;
        }
        break;
    }
    case vk::ImageType::e3D: {
        imageviewCreateInfo.viewType = vk::ImageViewType::e3D;
        break;
    }
    break;
    }
    mImageView = device.createImageView(imageviewCreateInfo);
}
void TextureResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    if (mSampler)
    {
        device.destroySampler(mSampler);
        mSampler = nullptr;
    }
    if (mImageView)
    {
        device.destroyImageView(mImageView);
        mImageView = nullptr;
    }
    if (mImage && mImageAllocation)
    {
        vmaDestroyImage(context->VmaAllocator, mImage, mImageAllocation);
        mImage = nullptr;
        mImageAllocation = nullptr;
        mImageAllocationInfo = {};
    }
}
std::pair<uint32_t, uint32_t> TextureResource::GetPixelSize(vk::Format format) const
{
    switch (format)
    {
    case vk::Format::eR8Unorm:
    case vk::Format::eR8Snorm:
    case vk::Format::eR8Uint:
    case vk::Format::eR8Sint:
    case vk::Format::eR8Srgb:
        return {1, 1};

    case vk::Format::eR8G8Unorm:
    case vk::Format::eR8G8Snorm:
    case vk::Format::eR8G8Uint:
    case vk::Format::eR8G8Sint:
    case vk::Format::eR8G8Srgb:
        return {2, 2};

    case vk::Format::eR8G8B8Unorm:
    case vk::Format::eR8G8B8Snorm:
    case vk::Format::eR8G8B8Uint:
    case vk::Format::eR8G8B8Sint:
    case vk::Format::eR8G8B8Srgb:
        return {3, 3};

    case vk::Format::eR8G8B8A8Unorm:
    case vk::Format::eR8G8B8A8Snorm:
    case vk::Format::eR8G8B8A8Uint:
    case vk::Format::eR8G8B8A8Sint:
    case vk::Format::eR8G8B8A8Srgb:
        return {4, 4};

    case vk::Format::eR16Unorm:
    case vk::Format::eR16Snorm:
    case vk::Format::eR16Uint:
    case vk::Format::eR16Sint:
    case vk::Format::eR16Sfloat:
        return {1, 2};

    case vk::Format::eR16G16Unorm:
    case vk::Format::eR16G16Snorm:
    case vk::Format::eR16G16Uint:
    case vk::Format::eR16G16Sint:
    case vk::Format::eR16G16Sfloat:
        return {2, 4};

    case vk::Format::eR16G16B16Unorm:
    case vk::Format::eR16G16B16Snorm:
    case vk::Format::eR16G16B16Uint:
    case vk::Format::eR16G16B16Sint:
    case vk::Format::eR16G16B16Sfloat:
        return {3, 6};

    case vk::Format::eR16G16B16A16Unorm:
    case vk::Format::eR16G16B16A16Snorm:
    case vk::Format::eR16G16B16A16Uint:
    case vk::Format::eR16G16B16A16Sint:
    case vk::Format::eR16G16B16A16Sfloat:
        return {4, 8};

    case vk::Format::eR32Uint:
    case vk::Format::eR32Sint:
    case vk::Format::eR32Sfloat:
        return {1, 4};

    case vk::Format::eR32G32Uint:
    case vk::Format::eR32G32Sint:
    case vk::Format::eR32G32Sfloat:
        return {2, 8};

    case vk::Format::eR32G32B32Uint:
    case vk::Format::eR32G32B32Sint:
    case vk::Format::eR32G32B32Sfloat:
        return {3, 12};

    case vk::Format::eR32G32B32A32Uint:
    case vk::Format::eR32G32B32A32Sint:
    case vk::Format::eR32G32B32A32Sfloat:
        return {4, 16};

    case vk::Format::eD16Unorm:
        return {1, 2};

    case vk::Format::eD32Sfloat:
        return {1, 4};

    case vk::Format::eD24UnormS8Uint:
        return {1, 4};

    case vk::Format::eD32SfloatS8Uint:
        return {1, 5};

    default:
        LogError("Unknown format: {}", vk::to_string(format));
        return {0, 0};
    }
}

} // namespace MEngine::Resource
