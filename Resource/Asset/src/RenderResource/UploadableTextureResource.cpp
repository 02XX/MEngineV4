#include "UploadableTextureResource.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include "TextureResource.hpp"
#include "UploadableTexture.hpp"
#include "VMA.hpp"
#include <algorithm>
#include <cstddef>

namespace MEngine::Resource
{
UploadableTextureResource::UploadableTextureResource(UploadableTexture *texture) : TextureResource(texture)
{
}
void UploadableTextureResource::InitRHI(std::shared_ptr<Context> context)
{
    TextureResource::InitRHI(context);
    auto instance = context->Instance.get();
    auto device = context->Device.get();
    auto texture = static_cast<Texture *>(mOwnerAsset);
    auto textureManager = std::dynamic_pointer_cast<TextureManager>(AssetManager::Instance().GetManager<Texture>());
    mBindlessDescriptorIndex = textureManager->AllocateDescriptorIndex();
    mBindlessTextureDescriptorSet = textureManager->mTextureBindlessDescriptorSet;
    vk::DescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(mImageView)
        .setSampler(mSampler);
    vk::WriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.setDstSet(mBindlessTextureDescriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(mBindlessDescriptorIndex)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setImageInfo(descriptorImageInfo);
    context->Device->updateDescriptorSets(writeDescriptorSet, nullptr);
}
void UploadableTextureResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    TextureResource::ReleaseRHI(context);
    ReleaseStaging(context);
}
void UploadableTextureResource::Upload()
{
    auto texture = static_cast<UploadableTexture *>(mOwnerAsset);
    size_t offset = 0;
    uint8_t *target = reinterpret_cast<uint8_t *>(mStagingBufferAllocationInfo.pMappedData);
    for (size_t mipLevel = 0; mipLevel < texture->mTextureDatas.size(); ++mipLevel)
    {
        const auto &mipData = texture->mTextureDatas[mipLevel];
        for (size_t arrayLayer = 0; arrayLayer < mipData.Datas.size(); ++arrayLayer)
        {
            const auto &layerData = mipData.Datas[arrayLayer];
            size_t dataSize = layerData.size();
            std::memcpy(target + offset, layerData.data(), dataSize);
            offset += dataSize;
        }
    }
}
void UploadableTextureResource::InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize)
{
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(bufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo stagingBufferAllocationCreateInfo{};
    stagingBufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingBufferAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    auto result = vmaCreateBuffer(
        context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
        &stagingBufferAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&mStagingBuffer), &mStagingBufferAllocation,
        reinterpret_cast<VmaAllocationInfo *>(&mStagingBufferAllocationInfo));
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create staging buffer with VMA");
    }
}
void UploadableTextureResource::ReleaseStaging(std::shared_ptr<Context> context)
{
    if (mStagingBuffer && mStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mStagingBuffer, mStagingBufferAllocation);
        mStagingBuffer = nullptr;
        mStagingBufferAllocation = nullptr;
        mStagingBufferAllocationInfo = {};
    }
}
} // namespace MEngine::Resource