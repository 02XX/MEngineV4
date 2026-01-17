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
void PBRMaterialResource::UpdateMaterial(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                         vk::CommandBufferInheritanceInfo *inheritanceInfo)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mOwnerAsset);
    pbrMaterial->mProperties.AlbedoIndex =
        pbrMaterial->mTextures.Albedo->GetResourceAs<Texture2DResource>()->mBindlessDescriptorIndex;
    pbrMaterial->mProperties.NormalIndex =
        pbrMaterial->mTextures.Normal->GetResourceAs<Texture2DResource>()->mBindlessDescriptorIndex;
    pbrMaterial->mProperties.ARMIndex =
        pbrMaterial->mTextures.ARM->GetResourceAs<Texture2DResource>()->mBindlessDescriptorIndex;
    pbrMaterial->mProperties.EmissiveIndex =
        pbrMaterial->mTextures.Emissive->GetResourceAs<Texture2DResource>()->mBindlessDescriptorIndex;
    // Copy Data
    void *data;
    uint8_t *mappedData = static_cast<uint8_t *>(mStagingBufferAllocationInfo.pMappedData);
    std::memcpy(mappedData, &pbrMaterial->mProperties, sizeof(PBRProperties));

    // Copy to GPU
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    if (inheritanceInfo)
    {
        beginInfo.setPInheritanceInfo(inheritanceInfo);
    }
    commandBuffer.begin(beginInfo);
    vk::BufferCopy copyRegion{};
    copyRegion.setSize(sizeof(PBRProperties)).setSrcOffset(0).setDstOffset(0);
    commandBuffer.copyBuffer(mStagingBuffer, mSSBO, copyRegion);
    vk::BufferMemoryBarrier2 bufferBarrier{};
    bufferBarrier.setSrcQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value())
        .setDstQueueFamilyIndex(context->QueueFamilyIndicates.graphicsFamily.value())
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setBuffer(mSSBO)
        .setOffset(0)
        .setSize(sizeof(PBRProperties));
    vk::DependencyInfo depInfo{};
    depInfo.setBufferMemoryBarriers(bufferBarrier);
    commandBuffer.pipelineBarrier2(depInfo);
    commandBuffer.end();
}
} // namespace MEngine::Resource