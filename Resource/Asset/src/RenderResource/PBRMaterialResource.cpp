#include "PBRMaterialResource.hpp"
#include "AssetManager.hpp"
#include "Material.hpp"
#include "PBRMaterial.hpp"
#include "PipelineManager.hpp"
#include "Texture2D.hpp"
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"

namespace MEngine::Resource
{
PBRMaterialResource::PBRMaterialResource(PBRMaterial *material) : MaterialResource(material)
{
}
void PBRMaterialResource::InitRHI(std::shared_ptr<Context> context)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mOwnerAsset);
    pbrMaterial->mPipeline->GetResource()->InitResource(context);
    pbrMaterial->mTextures.AlbedoTexture->GetResource()->InitResource(context);
    pbrMaterial->mTextures.NormalTexture->GetResource()->InitResource(context);
    pbrMaterial->mTextures.ARMTexture->GetResource()->InitResource(context);
    pbrMaterial->mTextures.EmissiveTexture->GetResource()->InitResource(context);

    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setSize(sizeof(PBRParams))
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress |
                  vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
                        &allocCreateInfo, reinterpret_cast<VkBuffer *>(&mBuffer), &mBufferAllocation,
                        &mBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create UBO buffer");
    }
    // Get Device Address
    vk::BufferDeviceAddressInfo bufferDeviceAddressInfo{};
    bufferDeviceAddressInfo.setBuffer(mBuffer);
    mBufferAddress = context->Device->getBufferAddress(bufferDeviceAddressInfo);
    auto pipelineManager = std::dynamic_pointer_cast<PipelineManager>(AssetManager::Instance().GetManager<Pipeline>());
    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.setDescriptorPool(pipelineManager->mDescriptorPool)
        .setSetLayouts(pipelineManager->mDefaultDescriptorSetLayouts.at(DefaultDescriptorSetLayoutType::Material))
        .setDescriptorSetCount(1);
    mDescriptorSet = context->Device->allocateDescriptorSets(allocateInfo).front();
    vk::WriteDescriptorSet descriptorWrite{};
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.setBuffer(mBuffer).setOffset(0).setRange(sizeof(PBRParams));
    descriptorWrite.setDstSet(mDescriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo(bufferInfo);
    context->Device->updateDescriptorSets({descriptorWrite}, {});
}
void PBRMaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    ReleaseStaging(context);
    if (mBuffer && mBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mBuffer, mBufferAllocation);
    }
}
void PBRMaterialResource::InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize)
{
    // Staging Buffer
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(bufferSize)
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
void PBRMaterialResource::ReleaseStaging(std::shared_ptr<Context> context)
{
    if (mStagingBuffer && mStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mStagingBuffer, mStagingBufferAllocation);
    }
}
void PBRMaterialResource::Upload()
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mOwnerAsset);
    pbrMaterial->mParam.AlbedoBindlessIndex =
        pbrMaterial->mTextures.AlbedoTexture->GetResourceAs<UploadableTextureResource>()->mBindlessDescriptorIndex;
    pbrMaterial->mParam.NormalBindlessIndex =
        pbrMaterial->mTextures.NormalTexture->GetResourceAs<UploadableTextureResource>()->mBindlessDescriptorIndex;
    pbrMaterial->mParam.ARMBindlessIndex =
        pbrMaterial->mTextures.ARMTexture->GetResourceAs<UploadableTextureResource>()->mBindlessDescriptorIndex;
    pbrMaterial->mParam.EmissiveBindlessIndex =
        pbrMaterial->mTextures.EmissiveTexture->GetResourceAs<UploadableTextureResource>()->mBindlessDescriptorIndex;
    uint8_t *target = reinterpret_cast<uint8_t *>(mStagingBufferAllocationInfo.pMappedData);
    std::memcpy(target, &pbrMaterial->mParam, sizeof(PBRParams));
}
void PBRMaterialResource::Bind(BindContext bindContext)
{
    MaterialResource::Bind(bindContext);
    bindContext.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bindContext.PipelineLayout,
                                                 mSetIndex, mDescriptorSet, {});
}
} // namespace MEngine::Resource