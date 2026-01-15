#include "TextureRenderTarget2DResource.hpp"
#include "Logger.hpp"
#include "RenderResource.hpp"
namespace MEngine::Resource
{
TextureRenderTarget2DResource::TextureRenderTarget2DResource(TextureRenderTarget2D *textureRenderTarget2D)
    : TextureResource(textureRenderTarget2D)
{
}
void TextureRenderTarget2DResource::InitRHI(std::shared_ptr<Context> context)
{

    mImageCreateInfo.imageType = vk::ImageType::e2D;
    mImageCreateInfo.arrayLayers = 1;

    VmaAllocationCreateInfo imageAllocationCreateInfo{};
    imageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    imageAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    auto result = vmaCreateImage(context->VmaAllocator, reinterpret_cast<VkImageCreateInfo *>(&mImageCreateInfo),
                                 &imageAllocationCreateInfo, reinterpret_cast<VkImage *>(&mImage), &mImageAllocation,
                                 reinterpret_cast<VmaAllocationInfo *>(&mImageAllocationInfo));
    if (result != VK_SUCCESS)
    {
        LogError("Failed to create image with VMA");
        return;
    }
    mSampler = context->Device->createSampler(mSamplerCreateInfo);
    vk::ImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.image = mImage;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = mImageCreateInfo.format;
    if (mImageCreateInfo.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        imageViewCreateInfo.subresourceRange.aspectMask =
            vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }
    else
    {
        imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = mImageCreateInfo.mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    mImageView = context->Device->createImageView(imageViewCreateInfo);
}
void TextureRenderTarget2DResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    device.destroySampler(mSampler);
    device.destroyImageView(mImageView);
    vmaDestroyImage(context->VmaAllocator, mImage, mImageAllocation);
}
} // namespace MEngine::Resource