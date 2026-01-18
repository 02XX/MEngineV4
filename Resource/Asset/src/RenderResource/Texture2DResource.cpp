#include "Texture2DResource.hpp"
#include "Logger.hpp"
#include "Texture2D.hpp"
#include "TextureResource.hpp"
#include "VMA.hpp"
#include <algorithm>

namespace MEngine::Resource
{
Texture2DResource::Texture2DResource(Texture2D *texture2D) : TextureResource(texture2D)
{
}
void Texture2DResource::InitRHI(std::shared_ptr<Context> context)
{
    auto texture2D = static_cast<Texture2D *>(mOwnerAsset);
    auto instance = context->Instance.get();
    auto device = context->Device.get();
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.setCommandPool(context->TransferCommandPool.get())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    vk::UniqueCommandBuffer commandBuffer = std::move(device.allocateCommandBuffersUnique(allocateInfo).front());
    // vk::DispatchLoaderDynamic dld(instance, vkGetInstanceProcAddr, device, vkGetDeviceProcAddr);
    texture2D->mTextureSettings.imageType = vk::ImageType::e2D;

    texture2D->mTextureSettings.arrayLayers = 1;
    texture2D->mTextureSettings.usage =
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;

    VmaAllocationCreateInfo imageAllocationCreateInfo{};
    imageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    imageAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    auto result =
        vmaCreateImage(context->VmaAllocator, reinterpret_cast<VkImageCreateInfo *>(&texture2D->mTextureSettings),
                       &imageAllocationCreateInfo, reinterpret_cast<VkImage *>(&mImage), &mImageAllocation,
                       reinterpret_cast<VmaAllocationInfo *>(&mImageAllocationInfo));
    if (result != VK_SUCCESS)
    {
        LogError("Failed to create image with VMA");
        return;
    }
    mSampler = device.createSampler(texture2D->mSamplerSettings);

    vk::ImageViewCreateInfo viewDesc{};
    viewDesc.image = mImage;
    viewDesc.viewType = vk::ImageViewType::e2D;
    viewDesc.format = texture2D->mTextureSettings.format;
    viewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewDesc.subresourceRange.baseMipLevel = 0;
    viewDesc.subresourceRange.levelCount = texture2D->mTextureSettings.mipLevels;
    viewDesc.subresourceRange.baseArrayLayer = 0;
    viewDesc.subresourceRange.layerCount = 1;
    mImageView = device.createImageView(viewDesc);

    // Staging Buffer
    size_t totalSize = 0;
    for (const auto &mip : texture2D->mTextureData)
    {
        totalSize += mip.Data.size();
    }
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(totalSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo stagingBufferAllocationCreateInfo{};
    stagingBufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingBufferAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    result = vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
                             &stagingBufferAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&mStagingBuffer),
                             &mStagingBufferAllocation,
                             reinterpret_cast<VmaAllocationInfo *>(&mStagingBufferAllocationInfo));
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create staging buffer with VMA");
    }
    vk::DescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(mImageView)
        .setSampler(mSampler);
    mBindlessDescriptorIndex = context->AllocateDescriptorIndex();
    vk::WriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.setDstSet(context->DescriptorSet.get())
        .setDstBinding(0)
        .setDstArrayElement(mBindlessDescriptorIndex)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setImageInfo(descriptorImageInfo);
    device.updateDescriptorSets(writeDescriptorSet, nullptr);
}
void Texture2DResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    TextureResource::ReleaseRHI(context);
    context->FreeDescriptorIndex(mBindlessDescriptorIndex);
    if (mStagingBuffer && mStagingBufferAllocation)
        vmaDestroyBuffer(context->VmaAllocator, mStagingBuffer, mStagingBufferAllocation);
}
} // namespace MEngine::Resource
