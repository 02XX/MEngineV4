#pragma once
#include "RHIResource.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{
class VertexResource;
class IndexResource;
} // namespace MEngine::Resource
namespace MEngine::Platform
{
using RHIBufferDesc = vk::BufferCreateInfo;
class RHIBuffer : public RHIResource
{
    friend class MEngine::Resource::VertexResource;
    friend class MEngine::Resource::IndexResource;

  protected:
    vk::Buffer mBuffer{};
    RHIBufferDesc mBufferDesc{};

    VmaAllocation mAllocation{};
    VmaAllocationInfo mAllocationInfo{};
    VmaAllocationCreateInfo mAllocationCreateInfo{};

  public:
    RHIBuffer(const RHIBufferDesc &bufferDesc, VmaAllocationCreateInfo allocCreateInfo);

    ~RHIBuffer() override = default;
    inline vk::Buffer GetBuffer() const
    {
        return mBuffer;
    }
    inline VmaAllocationInfo GetAllocationInfo() const
    {
        return mAllocationInfo;
    }
};
} // namespace MEngine::Platform