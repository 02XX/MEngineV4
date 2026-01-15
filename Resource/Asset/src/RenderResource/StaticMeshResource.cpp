#include "StaticMeshResource.hpp"
#include "StaticMesh.hpp"
namespace MEngine::Resource
{
StaticMeshResource::StaticMeshResource(StaticMesh *staticMesh) : RenderResource(staticMesh)
{
}
void StaticMeshResource::InitRHI(std::shared_ptr<Context> context)
{
    auto staticMesh = static_cast<StaticMesh *>(mOwnerAsset);
    auto device = context->Device.get();
    vk::UniqueCommandBuffer commandBuffer =
        std::move(device
                      .allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{}
                                                        .setCommandPool(context->TransferCommandPool.get())
                                                        .setLevel(vk::CommandBufferLevel::ePrimary)
                                                        .setCommandBufferCount(1))
                      .front());
    vk::UniqueFence copyFence = device.createFenceUnique(vk::FenceCreateInfo{});
    //=====================================Vertex Buffer=====================================
    vk::BufferCreateInfo vertexBufferCreateInfo{};
    vk::DeviceSize vertexBufferSize = sizeof(Vertex) * staticMesh->GetVertices().size();
    vertexBufferCreateInfo.setSize(vertexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo vertexAllocCreateInfo{};
    vertexAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<const VkBufferCreateInfo *>(&vertexBufferCreateInfo),
                        &vertexAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mVertexBuffer), &mVertexBufferAllocation,
                        &mVertexBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer");
    }
    // Staging Buffer
    vk::BufferCreateInfo vertexStagingBufferCreateInfo{};
    vertexStagingBufferCreateInfo.setSize(vertexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo vertexStagingAllocCreateInfo{};
    vertexStagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    vertexStagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    vk::Buffer vertexStagingBuffer;
    VmaAllocation vertexStagingBufferAllocation;
    VmaAllocationInfo vertexStagingBufferAllocationInfo;
    if (vmaCreateBuffer(context->VmaAllocator,
                        reinterpret_cast<const VkBufferCreateInfo *>(&vertexStagingBufferCreateInfo),
                        &vertexStagingAllocCreateInfo, reinterpret_cast<VkBuffer *>(&vertexStagingBuffer),
                        &vertexStagingBufferAllocation, &vertexStagingBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex staging buffer");
    }
    // Copy vertex data to staging buffer
    memcpy(vertexStagingBufferAllocationInfo.pMappedData, staticMesh->GetVertices().data(), (size_t)vertexBufferSize);
    // Copy data from staging buffer to vertex buffer
    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    vk::BufferCopy vertexCopyRegion{};
    vertexCopyRegion.size = vertexBufferSize;
    commandBuffer->copyBuffer(vertexStagingBuffer, mVertexBuffer, vertexCopyRegion);
    commandBuffer->end();
    vk::SubmitInfo vertexSubmitInfo{};
    vertexSubmitInfo.setCommandBuffers(commandBuffer.get());
    context->TransferQueue.submit(vertexSubmitInfo, copyFence.get());
    auto result = device.waitForFences(copyFence.get(), vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to wait for fence");
    }
    // Clean up staging buffer
    vmaDestroyBuffer(context->VmaAllocator, vertexStagingBuffer, vertexStagingBufferAllocation);
    commandBuffer->reset({});
    device.resetFences(copyFence.get());
    //=====================================Index Buffer=====================================
    vk::BufferCreateInfo indexBufferCreateInfo{};
    vk::DeviceSize indexBufferSize = sizeof(uint32_t) * staticMesh->GetIndices().size();
    indexBufferCreateInfo.setSize(indexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo indexAllocCreateInfo{};
    indexAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(context->VmaAllocator, reinterpret_cast<const VkBufferCreateInfo *>(&indexBufferCreateInfo),
                        &indexAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mIndexBuffer), &mIndexBufferAllocation,
                        &mIndexBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create index buffer");
    }
    // Staging Buffer
    vk::BufferCreateInfo indexStagingBufferCreateInfo{};
    indexStagingBufferCreateInfo.setSize(indexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo indexStagingAllocCreateInfo{};
    indexStagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    indexStagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    vk::Buffer indexStagingBuffer;
    VmaAllocation indexStagingBufferAllocation;
    VmaAllocationInfo indexStagingBufferAllocationInfo;
    if (vmaCreateBuffer(context->VmaAllocator,
                        reinterpret_cast<const VkBufferCreateInfo *>(&indexStagingBufferCreateInfo),
                        &indexStagingAllocCreateInfo, reinterpret_cast<VkBuffer *>(&indexStagingBuffer),
                        &indexStagingBufferAllocation, &indexStagingBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create index staging buffer");
    }
    // Copy index data to staging buffer
    memcpy(indexStagingBufferAllocationInfo.pMappedData, staticMesh->GetIndices().data(), (size_t)indexBufferSize);
    // Copy data from staging buffer to index buffer
    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    vk::BufferCopy indexCopyRegion{};
    indexCopyRegion.size = indexBufferSize;
    commandBuffer->copyBuffer(indexStagingBuffer, mIndexBuffer, indexCopyRegion);
    commandBuffer->end();
    vk::SubmitInfo indexSubmitInfo{};
    indexSubmitInfo.setCommandBuffers(commandBuffer.get());
    context->TransferQueue.submit(indexSubmitInfo, copyFence.get());
    result = device.waitForFences(copyFence.get(), vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to wait for fence");
    }
    // Clean up staging buffer
    vmaDestroyBuffer(context->VmaAllocator, indexStagingBuffer, indexStagingBufferAllocation);
}
void StaticMeshResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    if (mVertexBuffer)
    {
        vmaDestroyBuffer(context->VmaAllocator, mVertexBuffer, mVertexBufferAllocation);
    }
    if (mIndexBuffer)
    {
        vmaDestroyBuffer(context->VmaAllocator, mIndexBuffer, mIndexBufferAllocation);
    }
}
} // namespace MEngine::Resource