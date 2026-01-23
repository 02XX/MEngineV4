#include "MeshResource.hpp"
#include "Mesh.hpp"

namespace MEngine::Resource
{
MeshResource::MeshResource(Mesh *staticMesh) : RenderResource(staticMesh)
{
}
void MeshResource::InitRHI(std::shared_ptr<Context> context)
{
    auto staticMesh = static_cast<Mesh *>(mOwnerAsset);
    auto device = context->Device.get();
    //=====================================Vertex Buffer=====================================
    vk::BufferCreateInfo vertexBufferCreateInfo{};
    vk::DeviceSize vertexBufferSize = sizeof(Vertex) * staticMesh->mVertices.size();
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
    //=====================================Index Buffer=====================================
    vk::BufferCreateInfo indexBufferCreateInfo{};
    vk::DeviceSize indexBufferSize = sizeof(uint32_t) * staticMesh->mIndices.size();
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
}
void MeshResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    if (mVertexBuffer)
    {
        vmaDestroyBuffer(context->VmaAllocator, mVertexBuffer, mVertexBufferAllocation);
    }
    if (mIndexBuffer)
    {
        vmaDestroyBuffer(context->VmaAllocator, mIndexBuffer, mIndexBufferAllocation);
    }
    ReleaseStaging(context);
}
void MeshResource::InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize)
{
    auto staticMesh = static_cast<Mesh *>(mOwnerAsset);
    vk::DeviceSize vertexBufferSize = sizeof(Vertex) * staticMesh->mVertices.size();
    vk::DeviceSize indexBufferSize = sizeof(uint32_t) * staticMesh->mIndices.size();
    // Vertex Staging Buffer
    vk::BufferCreateInfo vertexStagingBufferCreateInfo{};
    vertexStagingBufferCreateInfo.setSize(vertexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo vertexStagingAllocCreateInfo{};
    vertexStagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    vertexStagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    if (vmaCreateBuffer(context->VmaAllocator,
                        reinterpret_cast<const VkBufferCreateInfo *>(&vertexStagingBufferCreateInfo),
                        &vertexStagingAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mStagingVertexBuffer),
                        &mStagingVertexBufferAllocation, &mStagingVertexBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex staging buffer");
    }
    // Index Staging Buffer
    vk::BufferCreateInfo indexStagingBufferCreateInfo{};
    indexStagingBufferCreateInfo.setSize(indexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo indexStagingAllocCreateInfo{};
    indexStagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    indexStagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    if (vmaCreateBuffer(context->VmaAllocator,
                        reinterpret_cast<const VkBufferCreateInfo *>(&indexStagingBufferCreateInfo),
                        &indexStagingAllocCreateInfo, reinterpret_cast<VkBuffer *>(&mStagingIndexBuffer),
                        &mStagingIndexBufferAllocation, &mStagingIndexBufferAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create index staging buffer");
    }
}
void MeshResource::ReleaseStaging(std::shared_ptr<Context> context)
{
    if (mStagingVertexBuffer && mStagingVertexBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mStagingVertexBuffer, mStagingVertexBufferAllocation);
    }
    if (mStagingIndexBuffer && mStagingIndexBufferAllocation)
    {
        vmaDestroyBuffer(context->VmaAllocator, mStagingIndexBuffer, mStagingIndexBufferAllocation);
    }
}
void MeshResource::UploadData()
{
    auto staticMesh = static_cast<Mesh *>(mOwnerAsset);
    size_t vertexBufferOffset = 0, indexBufferOffset = 0;
    uint8_t *vertexBufferTarget = reinterpret_cast<uint8_t *>(mStagingVertexBufferAllocationInfo.pMappedData);
    uint8_t *indexBufferTarget = reinterpret_cast<uint8_t *>(mStagingIndexBufferAllocationInfo.pMappedData);
    // Copy Vertex Data
    size_t vertexDataSize = sizeof(Vertex) * staticMesh->mVertices.size();
    std::memcpy(vertexBufferTarget + vertexBufferOffset, staticMesh->mVertices.data(), vertexDataSize);
    vertexBufferOffset += vertexDataSize;
    // Copy Index Data
    size_t indexDataSize = sizeof(uint32_t) * staticMesh->mIndices.size();
    std::memcpy(indexBufferTarget + indexBufferOffset, staticMesh->mIndices.data(), indexDataSize);
    indexBufferOffset += indexDataSize;
}
void MeshResource::Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, vk::Pipeline pipeline)
{
    commandBuffer.bindVertexBuffers(0, mVertexBuffer, {0});
    commandBuffer.bindIndexBuffer(mIndexBuffer, 0, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(static_cast<uint32_t>(static_cast<Mesh *>(mOwnerAsset)->mIndices.size()), 1, 0, 0, 0);
}
} // namespace MEngine::Resource