#include "SceneResource.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"
#include "PipelineManager.hpp"
#include "Scene.hpp"
#include "VMA.hpp"
#include <cstddef>
#include <memory>
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{
SceneResource::SceneResource(Scene *scene) : RenderResource(scene), mScene(scene)
{
}
void SceneResource::InitRHI(std::shared_ptr<Context> context)
{
    // UBO
    size_t bufferSize = sizeof(SceneParam);
    vk::BufferCreateInfo bufferCreateInfo{};
    VmaAllocationCreateInfo bufferAllocCreateInfo{};
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

    // SSBO
    size_t ssboBufferSize = sizeof(LightParam) * MAX_LIGHTS;
    vk::BufferCreateInfo ssboBufferCreateInfo{};
    VmaAllocationCreateInfo ssboBufferAllocCreateInfo{};
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
    InitStaging(context, {});

    auto pipelineManager = std::dynamic_pointer_cast<PipelineManager>(AssetManager::Instance().GetManager<Pipeline>());
    vk::DescriptorSetAllocateInfo sceneDescriptorSetAllocInfo{};
    sceneDescriptorSetAllocInfo.setDescriptorPool(pipelineManager->mDescriptorPool)
        .setSetLayouts(pipelineManager->mDefaultDescriptorSetLayouts.at(DefaultDescriptorSetLayoutType::Global))
        .setDescriptorSetCount(1);
    auto descriptorSets = context->Device->allocateDescriptorSets(sceneDescriptorSetAllocInfo);
    if (descriptorSets.empty())
    {
        LogError("Failed to allocate Scene descriptor set");
        return;
    }
    mGlobalDescriptorSet = descriptorSets[0];
    vk::WriteDescriptorSet uboDescriptorWrite{}, ssboDescriptorWrite{};
    vk::DescriptorBufferInfo uboBufferInfo{}, ssboBufferInfo{};
    uboBufferInfo.setBuffer(mUBOBuffer).setOffset(0).setRange(bufferSize);
    uboDescriptorWrite.setDstSet(mGlobalDescriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo({uboBufferInfo});
    ssboBufferInfo.setBuffer(mSSBOBuffer).setOffset(0).setRange(ssboBufferSize);
    ssboDescriptorWrite.setDstSet(mGlobalDescriptorSet)
        .setDstBinding(1)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setBufferInfo({ssboBufferInfo});
    context->Device->updateDescriptorSets({uboDescriptorWrite, ssboDescriptorWrite}, {});
}
void SceneResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    ReleaseStaging(context);
    if (mUBOBuffer && mUBOBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mUBOBuffer, mUBOBufferAllocation);
    }
    if (mSSBOBuffer && mSSBOBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mSSBOBuffer, mSSBOBufferAllocation);
    }
    if (mGlobalDescriptorSet)
    {
        auto pipelineManager =
            std::dynamic_pointer_cast<PipelineManager>(AssetManager::Instance().GetManager<Pipeline>());
        context->Device->freeDescriptorSets(pipelineManager->mDescriptorPool, {mGlobalDescriptorSet});
        mGlobalDescriptorSet = nullptr;
    }
}
void SceneResource::InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize)
{
    vk::DeviceSize uboBufferSize = sizeof(SceneParam);
    vk::DeviceSize ssboBufferSize = sizeof(LightParam) * MAX_LIGHTS;
    vk::BufferCreateInfo stagingUBOBufferCreateInfo{}, stagingSSBOBufferCreateInfo{};
    VmaAllocationCreateInfo stagingUBOBufferAllocCreateInfo{}, stagingSSBOBufferAllocCreateInfo{};
    stagingUBOBufferCreateInfo.setSize(uboBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    stagingUBOBufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingUBOBufferAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingUBOBufferCreateInfo),
                        &stagingUBOBufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mUBOStagingBuffer),
                        &mUBOStagingBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mUBOStagingBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create UBO staging buffer");
        return;
    }
    stagingSSBOBufferCreateInfo.setSize(ssboBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    stagingSSBOBufferAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    stagingSSBOBufferAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&stagingSSBOBufferCreateInfo),
                        &stagingSSBOBufferAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mSSBOStagingBuffer),
                        &mSSBOStagingBufferAllocation,
                        reinterpret_cast<VmaAllocationInfo *>(&mSSBOStagingBufferAllocationInfo)) != VK_SUCCESS)
    {
        LogError("Failed to create SSBO staging buffer");
        return;
    }
}
void SceneResource::ReleaseStaging(std::shared_ptr<Context> context)
{
    if (mUBOStagingBuffer && mUBOStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mUBOStagingBuffer, mUBOStagingBufferAllocation);
    }
    if (mSSBOStagingBuffer && mSSBOStagingBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mSSBOStagingBuffer, mSSBOStagingBufferAllocation);
    }
}
void SceneResource::Upload()
{
    // UBO
    auto uboTarget = static_cast<uint8_t *>(mUBOStagingBufferAllocationInfo.pMappedData);
    auto uboBufferSize = sizeof(SceneParam);
    std::memcpy(uboTarget, &mScene->mSceneParams, uboBufferSize);
    // SSBO
    auto ssboTarget = static_cast<uint8_t *>(mSSBOStagingBufferAllocationInfo.pMappedData);
    auto ssboBufferSize = sizeof(LightParam) * MAX_LIGHTS;
    for (size_t i = 0; i < MAX_LIGHTS; ++i)
    {
        std::memcpy(ssboTarget + i * sizeof(LightParam), &mScene->mLightParams[i], sizeof(LightParam));
    }
}
void SceneResource::Bind(BindContext bindContext)
{
    bindContext.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bindContext.PipelineLayout,
                                                 mSetIndex, {mGlobalDescriptorSet}, {});
}
} // namespace MEngine::Resource
