#include "PBRMaterialResource.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include "VMA.hpp"
#include <cstdint>
#include <cstring>
namespace MEngine::Resource
{
void PBRMaterialResource::InitRHI(std::shared_ptr<Context> context)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mOwnerAsset);
    pbrMaterial->mTextures.Albedo->GetResource()->InitResource(context);
    pbrMaterial->mTextures.Normal->GetResource()->InitResource(context);
    pbrMaterial->mTextures.ARM->GetResource()->InitResource(context);
    pbrMaterial->mTextures.Emissive->GetResource()->InitResource(context);

    // Staging Buffer
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(sizeof(PBRProperties))
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo stagingAllocCreateInfo{};
    stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    vk::Buffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    VmaAllocationInfo stagingBufferAllocationInfo;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
                        &stagingAllocCreateInfo, reinterpret_cast<VkBuffer *>(&stagingBuffer), &stagingBufferAllocation,
                        &stagingBufferAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create PBRMaterial staging buffer");
        return;
    }
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
    uint8_t *mappedData = static_cast<uint8_t *>(stagingBufferAllocationInfo.pMappedData);
    std::memcpy(mappedData, &pbrMaterial->mProperties, sizeof(PBRProperties));

    mPropertiesOffset = context->NextSSBOOffset;
    // Copy to GPU
    vk::CommandBufferAllocateInfo cmdBufAllocInfo{};
    cmdBufAllocInfo.setCommandPool(context->TransferCommandPool.get())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    vk::UniqueCommandBuffer cmdBuffer = std::move(context->Device->allocateCommandBuffersUnique(cmdBufAllocInfo)[0]);
    vk::CommandBufferBeginInfo beginInfo{};
    cmdBuffer->begin(beginInfo);
    vk::BufferCopy copyRegion{};
    copyRegion.setSize(sizeof(PBRProperties)).setSrcOffset(0).setDstOffset(context->NextSSBOOffset);
    cmdBuffer->copyBuffer(stagingBuffer, context->SSBO, copyRegion);
    vk::BufferMemoryBarrier2 bufferBarrier{};
    bufferBarrier.setSrcQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value())
        .setDstQueueFamilyIndex(context->QueueFamilyIndicates.graphicsFamily.value())
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setBuffer(context->SSBO)
        .setOffset(context->NextSSBOOffset)
        .setSize(sizeof(PBRProperties));
    vk::DependencyInfo depInfo{};
    depInfo.setBufferMemoryBarriers(bufferBarrier);
    cmdBuffer->pipelineBarrier2(depInfo);
    cmdBuffer->end();

    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBufferCount(1).setPCommandBuffers(&cmdBuffer.get());
    context->TransferQueue.submit(submitInfo, nullptr);
    context->TransferQueue.waitIdle();
    context->NextSSBOOffset += sizeof(PBRProperties);
    // Cleanup Staging Buffer
    vmaDestroyBuffer(context->VmaAllocator, stagingBuffer, stagingBufferAllocation);
}
void PBRMaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
}

} // namespace MEngine::Resource