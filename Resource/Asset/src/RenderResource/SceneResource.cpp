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
    // UBO
    size_t bufferSize = sizeof(SceneParam);
    vk::BufferCreateInfo bufferCreateInfo{}, stagingBufferCreateInfo{};
    VmaAllocationCreateInfo bufferAllocCreateInfo{}, stagingBufferAllocCreateInfo{};
    bufferCreateInfo.setSize(bufferSize)
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst |
                  vk::BufferUsageFlagBits::eShaderDeviceAddress)
        .setSharingMode(vk::SharingMode::eExclusive);
    bufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
                        &bufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mUBOBuffer), &mUBOBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mUBOBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create UBO buffer");
        return;
    }
    // Get Device Address
    vk::BufferDeviceAddressInfo bufferAddressInfo{};
    bufferAddressInfo.setBuffer(mUBOBuffer);
    mUBOBufferAddress = context->Device->getBufferAddress(bufferAddressInfo);

    stagingBufferCreateInfo.setSize(bufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    stagingBufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingBufferAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingBufferCreateInfo),
                        &stagingBufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mUBOStagingBuffer),
                        &mUBOStagingBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mUBOStagingBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create UBO staging buffer");
        return;
    }
    // SSBO
    size_t ssboBufferSize = sizeof(LightParam) * MAX_LIGHTS;
    vk::BufferCreateInfo ssboBufferCreateInfo{}, ssboStagingBufferCreateInfo{};
    VmaAllocationCreateInfo ssboBufferAllocCreateInfo{}, ssboStagingBufferAllocCreateInfo{};
    ssboBufferCreateInfo.setSize(ssboBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst |
                  vk::BufferUsageFlagBits::eShaderDeviceAddress)
        .setSharingMode(vk::SharingMode::eExclusive);
    ssboBufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&ssboBufferCreateInfo),
                        &ssboBufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mSSBOBuffer), &mSSBOBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mSSBOBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create SSBO buffer");
        return;
    }
    // Get Device Address
    vk::BufferDeviceAddressInfo ssboBufferAddressInfo{};
    ssboBufferAddressInfo.setBuffer(mSSBOBuffer);
    mSSBOBufferAddress = context->Device->getBufferAddress(ssboBufferAddressInfo);
    ssboStagingBufferCreateInfo.setSize(ssboBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    ssboStagingBufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    ssboStagingBufferAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&ssboStagingBufferCreateInfo),
                        &ssboStagingBufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mSSBOStagingBuffer),
                        &mSSBOStagingBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mSSBOStagingBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create SSBO staging buffer");
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
    vk::WriteDescriptorSet uboDescriptorWrite{}, ssboDescriptorWrite{};
    vk::DescriptorBufferInfo uboBufferInfo{}, ssboBufferInfo{};
    uboBufferInfo.setBuffer(mUBOBuffer).setOffset(0).setRange(bufferSize);
    uboDescriptorWrite.setDstSet(mDescriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo({uboBufferInfo});
    ssboBufferInfo.setBuffer(mSSBOBuffer).setOffset(0).setRange(ssboBufferSize);
    ssboDescriptorWrite.setDstSet(mDescriptorSet)
        .setDstBinding(1)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setBufferInfo({ssboBufferInfo});
    context->Device->updateDescriptorSets({uboDescriptorWrite, ssboDescriptorWrite}, {});
}
void SceneResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    if (mUBOBuffer && mUBOBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mUBOBuffer, mUBOBufferAllocation);
    }
    if (mUBOStagingBuffer && mUBOStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mUBOStagingBuffer, mUBOStagingBufferAllocation);
    }
    if (mSSBOBuffer && mSSBOBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mSSBOBuffer, mSSBOBufferAllocation);
    }
    if (mSSBOStagingBuffer && mSSBOStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mSSBOStagingBuffer, mSSBOStagingBufferAllocation);
    }
}
} // namespace MEngine::Resource
