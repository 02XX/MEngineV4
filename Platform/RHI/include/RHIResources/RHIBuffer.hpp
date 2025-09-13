#pragma once
#include "RHIResource.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
class RHIBuffer : public RHIResource
{
  protected:
    vk::Buffer mBuffer{};
    vk::BufferCreateInfo mBufferCreateInfo{};
    vk::BufferUsageFlags mUsages{};

    VmaAllocationCreateFlags mAllocationCreateFlags{VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT};
    VmaAllocation mAllocation{};
    VmaAllocationInfo mAllocationInfo{};

  protected:
    RHIBuffer() : RHIResource()
    {
    }

  public:
    ~RHIBuffer() override = default;
};
} // namespace MEngine::Platform