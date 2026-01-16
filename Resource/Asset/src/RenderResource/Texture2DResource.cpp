#include "Texture2DResource.hpp"
#include "Logger.hpp"
#include "Texture2D.hpp"
#include "TextureResource.hpp"
#include "VMA.hpp"
#include <algorithm>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
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
    vk::Buffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    VmaAllocationInfo stagingBufferAllocationInfo{};
    result =
        vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
                        &stagingBufferAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&stagingBuffer),
                        &stagingBufferAllocation, reinterpret_cast<VmaAllocationInfo *>(&stagingBufferAllocationInfo));
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create staging buffer with VMA");
    }
    // Copy texture data to staging buffer
    uint8_t *mappedData = reinterpret_cast<uint8_t *>(stagingBufferAllocationInfo.pMappedData);
    size_t offset = 0;
    for (const auto &mip : texture2D->mTextureData)
    {
        std::memcpy(mappedData + offset, mip.Data.data(), mip.Data.size());
        offset += mip.Data.size();
    }
    if (texture2D->mTextureData.empty())
    {
        throw std::runtime_error("Texture data is empty");
    }

    if (texture2D->mTextureSettings.mipLevels != texture2D->mTextureData.size())
    {
        LogWarn("Texture mipmap levels do not match the provided mip data size. Using the smaller mipmap level of the "
                "two.");
    }
    vk::ImageMemoryBarrier2 preBarrier{};
    preBarrier.setImage(mImage)
        .setSrcQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value())
        .setDstQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value())
        .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                                       texture2D->mTextureSettings.mipLevels, 0,
                                                       texture2D->mTextureSettings.arrayLayers})
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
        .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite);
    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandBuffer->pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers({preBarrier}));

    std::vector<vk::BufferImageCopy2> copyRegions(texture2D->mTextureSettings.mipLevels);
    size_t bufferOffset = 0;
    for (size_t i = 0; i < copyRegions.size(); i++)
    {
        auto &mip = texture2D->mTextureData[i];
        copyRegions[i]
            .setBufferOffset(bufferOffset)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageSubresource(
                vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, static_cast<uint32_t>(i), 0, 1})
            .setImageExtent(mip.Extent);
        bufferOffset += mip.Data.size();
    }
    vk::CopyBufferToImageInfo2 copyInfo{};
    copyInfo.setDstImage(mImage)
        .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcBuffer(stagingBuffer)
        .setRegions(copyRegions);
    commandBuffer->copyBufferToImage2(copyInfo);

    vk::ImageMemoryBarrier2 postBarrier{};
    postBarrier.setImage(mImage)
        .setSrcQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value())
        .setDstQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value())
        .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                                       texture2D->mTextureSettings.mipLevels, 0,
                                                       texture2D->mTextureSettings.arrayLayers})
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    commandBuffer->pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers({postBarrier}));
    commandBuffer->end();
    vk::SubmitInfo2 submitInfo{};
    submitInfo.setCommandBufferInfos({vk::CommandBufferSubmitInfo{}.setCommandBuffer(commandBuffer.get())});
    context->TransferQueue.submit2({submitInfo});
    context->TransferQueue.waitIdle();
    // Cleanup staging buffer
    vmaDestroyBuffer(context->VmaAllocator, stagingBuffer, stagingBufferAllocation);

    // 更新bindless descriptor
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
}
} // namespace MEngine::Resource
