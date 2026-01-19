#include "SceneResource.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
#include "VMA.hpp"
#include <cstddef>
namespace MEngine::Resource
{
SceneResource::SceneResource(Scene *scene) : RenderResource(scene), mScene(scene)
{
}
void SceneResource::InitRHI(std::shared_ptr<Context> context)
{
    size_t bufferSize = sizeof(SceneParam);
    // Scene SSBO
    vk::BufferCreateInfo bufferCreateInfo{}, stagingBufferCreateInfo{};
    VmaAllocationCreateInfo bufferAllocCreateInfo{}, stagingBufferAllocCreateInfo{};
    bufferCreateInfo.setSize(bufferSize)
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst |
                  vk::BufferUsageFlagBits::eShaderDeviceAddress)
        .setSharingMode(vk::SharingMode::eExclusive);
    bufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
                        &bufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mBuffer), &mBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create Scene SSBO buffer");
        return;
    }
    // Get Device Address
    vk::BufferDeviceAddressInfo bufferAddressInfo{};
    bufferAddressInfo.setBuffer(mBuffer);
    mBufferAddress = context->Device->getBufferAddress(bufferAddressInfo);

    stagingBufferCreateInfo.setSize(bufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    stagingBufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingBufferAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
                        &stagingBufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mStagingBuffer),
                        &mStagingBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mStagingBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create Scene staging buffer");
        return;
    }
    vk::DescriptorSetAllocateInfo sceneDescriptorSetAllocInfo{};
    sceneDescriptorSetAllocInfo.setDescriptorPool(context->DescriptorPool.get())
        .setSetLayouts(
            context->DefaultDescriptorSetLayouts[Context::DefaultDescriptorSetLayoutType::GlobalStorage].get())
        .setDescriptorSetCount(1);
    auto descriptorSets = context->Device->allocateDescriptorSets(sceneDescriptorSetAllocInfo);
    if (descriptorSets.empty())
    {
        LogError("Failed to allocate Scene descriptor set");
        return;
    }
    mDescriptorSet = descriptorSets[0];
    vk::WriteDescriptorSet sceneDescriptorWrite{};
    vk::DescriptorBufferInfo sceneBufferInfo{};
    sceneBufferInfo.setBuffer(mBuffer).setOffset(0).setRange(bufferSize);
    sceneDescriptorWrite.setDstSet(mDescriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setBufferInfo({sceneBufferInfo});
    context->Device->updateDescriptorSets({sceneDescriptorWrite}, {});
}
void SceneResource::ReleaseRHI(std::shared_ptr<Context> context)
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
