#include "PBRMaterialResource.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include "VMA.hpp"
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_structs.hpp>
namespace MEngine::Resource
{
void PBRMaterialResource::InitRHI(std::shared_ptr<Context> context)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mOwnerAsset);
    pbrMaterial->mTextures.Albedo->GetResource()->InitResource(context);
    pbrMaterial->mTextures.Normal->GetResource()->InitResource(context);
    pbrMaterial->mTextures.ARM->GetResource()->InitResource(context);
    pbrMaterial->mTextures.Emissive->GetResource()->InitResource(context);

    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setSize(sizeof(PBRProperties))
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress |
                  vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
                        &allocCreateInfo, reinterpret_cast<VkBuffer *>(&mSSBO), &mSSBOAllocation,
                        &mSSBOAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create SSBO buffer");
    }
    // Get Device Address
    vk::BufferDeviceAddressInfo bufferDeviceAddressInfo{};
    bufferDeviceAddressInfo.setBuffer(mSSBO);
    mSSBOAddress = context->Device->getBufferAddress(bufferDeviceAddressInfo);

    // Staging Buffer
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(sizeof(PBRProperties))
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo stagingAllocCreateInfo{};
    stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
                        &stagingAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mStagingBuffer),
                        &mStagingBufferAllocation, &mStagingBufferAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create PBRMaterial staging buffer");
        return;
    }
}
void PBRMaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    if (mSSBO && mSSBOAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mSSBO, mSSBOAllocation);
    }
    if (mStagingBuffer && mStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mStagingBuffer, mStagingBufferAllocation);
    }
}
} // namespace MEngine::Resource