#include "IndexResource.hpp"
#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
#include "RHIHandler.hpp"
#include <cstring>

namespace MEngine::Resource
{
void IndexResource::InitRHI()
{
}
void IndexResource::ReleaseRHI()
{
    mRHIVertexBufferHandler.SafeRelease();
}
void IndexResource::UpdateData(const std::vector<uint32_t> &indices)
{
    // Staging buffer
    vk::DeviceSize bufferSize = sizeof(uint32_t) * indices.size();
    VmaAllocationCreateInfo stagingAllocCreateInfo = {};
    stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    stagingAllocCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    vk::BufferCreateInfo stagingBufferDesc = {};
    stagingBufferDesc.size = bufferSize;
    stagingBufferDesc.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stagingBufferDesc.sharingMode = vk::SharingMode::eExclusive;
    // Create staging buffer
    mRHIVertexBufferHandler = RHIHandler<RHIBuffer>(new RHIBuffer(stagingBufferDesc, stagingAllocCreateInfo));
    // Copy vertex data to staging buffer
    memcpy(mRHIVertexBufferHandler->mAllocationInfo.pMappedData, indices.data(), (size_t)bufferSize);
    // Create vertex buffer
    VmaAllocationCreateInfo vertexAllocCreateInfo = {};
    vertexAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vertexAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    vk::BufferCreateInfo vertexBufferDesc = {};
    vertexBufferDesc.size = bufferSize;
    vertexBufferDesc.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    vertexBufferDesc.sharingMode = vk::SharingMode::eExclusive;
    auto vertexBufferHandler = RHIHandler<RHIBuffer>(new RHIBuffer(vertexBufferDesc, vertexAllocCreateInfo));
    // Copy data from staging buffer to vertex buffer

    auto &rhiContext = RHIContext::Instance();
    auto &device = rhiContext.GetDevice();
    vk::UniqueFence fence = device.createFenceUnique(vk::FenceCreateInfo{});
    auto commandBuffer = rhiContext.GetTransferCommandBuffer(vk::CommandBufferLevel::ePrimary);
    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    vk::BufferCopy copyRegion{};
    copyRegion.size = bufferSize;
    commandBuffer->copyBuffer(mRHIVertexBufferHandler->mBuffer, vertexBufferHandler->mBuffer, copyRegion);
    commandBuffer->end();
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(commandBuffer.get());

    rhiContext.GetTransferQueue().submit(submitInfo, fence.get());
    auto result = device.waitForFences(fence.get(), vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to wait for fence");
    }
};
} // namespace MEngine::Resource