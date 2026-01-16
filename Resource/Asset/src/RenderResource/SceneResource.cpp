#include "SceneResource.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
#include "VMA.hpp"
namespace MEngine::Resource
{
SceneResource::SceneResource(Scene *scene) : RenderResource(scene), mScene(scene)
{
}
void SceneResource::InitRHI(std::shared_ptr<Context> context)
{
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setSize(sizeof(SceneParameter))
        .setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst |
                  vk::BufferUsageFlagBits::eShaderDeviceAddress)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
                        &allocCreateInfo, reinterpret_cast<VkBuffer *>(&mSceneSSBO), &mSceneSSBOAllocation,
                        &mSceneSSBOAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create Scene SSBO");
        return;
    }
    mSceneSSBOAddress = context->Device->getBufferAddress(vk::BufferDeviceAddressInfo().setBuffer(mSceneSSBO));
}
void SceneResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    vmaDestroyBuffer(context->VmaAllocator, mSceneSSBO, mSceneSSBOAllocation);
}
void SceneResource::UpdateSceneUBO(SceneParameter sceneParams)
{
    uint8_t *mappedData = static_cast<uint8_t *>(mSceneSSBOAllocationInfo.pMappedData);
    std::memcpy(mappedData, &sceneParams, sizeof(SceneParameter));
}
} // namespace MEngine::Resource
