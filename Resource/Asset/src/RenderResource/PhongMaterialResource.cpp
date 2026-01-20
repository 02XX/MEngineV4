#include "PhongMaterialResource.hpp"
#include "Logger.hpp"
#include "PhongMaterial.hpp"
#include "VMA.hpp"
#include <cstdint>
#include <cstring>
namespace MEngine::Resource
{
void PhongMaterialResource::InitRHI(std::shared_ptr<Context> context)
{
    auto phongMaterial = static_cast<PhongMaterial *>(mOwnerAsset);
    phongMaterial->mTextures.mDiffuseTexture->GetResource()->InitResource(context);
    phongMaterial->mTextures.mSpecularTexture->GetResource()->InitResource(context);

    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setSize(sizeof(PhongParam))
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress |
                  vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
                        &allocCreateInfo, reinterpret_cast<VkBuffer *>(&mBuffer), &mBufferAllocation,
                        &mBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create SSBO buffer");
    }
    // Get Device Address
    vk::BufferDeviceAddressInfo bufferDeviceAddressInfo{};
    bufferDeviceAddressInfo.setBuffer(mBuffer);
    mBufferAddress = context->Device->getBufferAddress(bufferDeviceAddressInfo);

    // Staging Buffer
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(sizeof(PhongParam))
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
        LogError("Failed to create PhongMaterial staging buffer");
        return;
    }
    vk::DescriptorSetAllocateInfo descriptorSetAllocInfo{};
    descriptorSetAllocInfo.setDescriptorPool(context->DescriptorPool.get())
        .setSetLayouts(context->DefaultDescriptorSetLayouts[Context::DefaultDescriptorSetLayoutType::PBR].get())
        .setDescriptorSetCount(1);
    auto descriptorSets = context->Device->allocateDescriptorSets(descriptorSetAllocInfo);
    if (descriptorSets.empty())
    {
        LogError("Failed to allocate PhongMaterial descriptor set");
        return;
    }
    mDescriptorSet = descriptorSets[0];
    vk::WriteDescriptorSet descriptorWrite{};
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.setBuffer(mBuffer).setOffset(0).setRange(sizeof(PhongParam));
    descriptorWrite.setDstSet(mDescriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo({bufferInfo});
    context->Device->updateDescriptorSets({descriptorWrite}, {});
}
void PhongMaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    if (mBuffer && mBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mBuffer, mBufferAllocation);
    }
    if (mStagingBuffer && mStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mStagingBuffer, mStagingBufferAllocation);
    }
}
} // namespace MEngine::Resource