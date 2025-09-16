#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHIBuffer::RHIBuffer(const RHIBufferDesc &bufferDesc, VmaAllocationCreateInfo allocCreateInfo)
    : RHIResource(), mBufferDesc(bufferDesc), mAllocationCreateInfo(allocCreateInfo)
{
    if (vmaCreateBuffer(RHIContext::Instance().GetVmaAllocator(),
                        reinterpret_cast<const VkBufferCreateInfo *>(&mBufferDesc), &mAllocationCreateInfo,
                        reinterpret_cast<VkBuffer *>(&mBuffer), &mAllocation, &mAllocationInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer");
    }
}
} // namespace MEngine::Platform