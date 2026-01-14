#include "Texture2DResource.hpp"
#include "Logger.hpp"
#include "RenderResource.hpp"

namespace MEngine::Resource
{
void Texture2DResource::InitRHI(std::shared_ptr<Context> context)
{

    auto instance = context->Instance.get();
    auto device = context->Device.get();
    vk::DispatchLoaderDynamic dld(instance, vkGetInstanceProcAddr, device, vkGetDeviceProcAddr);
    mImageCreateInfo.imageType = vk::ImageType::e2D;

    mImageCreateInfo.arrayLayers = 1;
    mImageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled |
                             vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eHostTransferEXT;

    VmaAllocationCreateInfo imageAllocationCreateInfo{};
    imageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    imageAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    auto result = vmaCreateImage(context->VmaAllocator, reinterpret_cast<VkImageCreateInfo *>(&mImageCreateInfo),
                                 &imageAllocationCreateInfo, reinterpret_cast<VkImage *>(&mImage), &mImageAllocation,
                                 reinterpret_cast<VmaAllocationInfo *>(&mImageAllocationInfo));
    if (result != VK_SUCCESS)
    {
        LogError("Failed to create image with VMA");
        return;
    }
    mSampler = device.createSampler(mSamplerCreateInfo);

    vk::ImageViewCreateInfo viewDesc{};
    viewDesc.image = mImage;
    viewDesc.viewType = vk::ImageViewType::e2D;
    viewDesc.format = mImageCreateInfo.format;
    viewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewDesc.subresourceRange.baseMipLevel = 0;
    viewDesc.subresourceRange.levelCount = mImageCreateInfo.mipLevels;
    viewDesc.subresourceRange.baseArrayLayer = 0;
    viewDesc.subresourceRange.layerCount = 1;
    mImageView = device.createImageView(viewDesc);

    // if (!mTextureData.empty())
    // {
    //     if (mImageCreateInfo.mipLevels != mTextureData.size())
    //     {
    //         LogWarn("Texture mipmap levels do not match the provided mip data size. Using the smaller mipmap level of
    //         "
    //                 "the two.");
    //     }
    //     mRHITextureHandler->TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

    //     std::vector<vk::MemoryToImageCopyEXT> copyRegions(mImageCreateInfo.mipLevels);
    //     auto [channel, pixSize] = GetPixelSize(mImageCreateInfo.format);
    //     for (size_t i = 0; i < copyRegions.size(); i++)
    //     {
    //         auto &mip = mTextureData[i];
    //         copyRegions[i]
    //             .setMemoryImageHeight(mip.SizeY)
    //             .setMemoryRowLength(mip.SizeX * channel)
    //             .setImageSubresource(vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor,
    //                                                             static_cast<uint32_t>(i), 0,
    //                                                             mRHITextureHandler->mTextureDesc.arrayLayers})
    //             .setImageOffset(vk::Offset3D{0, 0, 0})
    //             .setImageExtent(vk::Extent3D{std::max(1u, mip.SizeX), std::max(1u, mip.SizeY), 1})
    //             .setPHostPointer(mip.Data.data());
    //     }
    //     vk::CopyMemoryToImageInfoEXT copyInfo{};
    //     copyInfo.setDstImage(mRHITextureHandler->mImage)
    //         .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
    //         .setRegions(copyRegions);
    //     device.copyMemoryToImageEXT(copyInfo, dld);
    //     mRHITextureHandler->TransitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    // }
}

void Texture2DResource::ReleaseRHI(std::shared_ptr<Context> context)
{

    auto device = context->Device.get();
    device.destroySampler(mSampler);
    device.destroyImageView(mImageView);
    vmaDestroyImage(context->VmaAllocator, mImage, mImageAllocation);
}
} // namespace MEngine::Resource
