#pragma once
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <cstdint>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class PBRMaterial;
class PBRMaterialResource final : public MaterialResource
{
  public:
    vk::DeviceAddress mBufferAddress{};

  protected:
    vk::Buffer mBuffer{};
    VmaAllocation mBufferAllocation{};
    VmaAllocationInfo mBufferAllocationInfo{};

    vk::Buffer mStagingBuffer{};
    VmaAllocation mStagingBufferAllocation{};
    VmaAllocationInfo mStagingBufferAllocationInfo{};

    vk::DescriptorSet mDescriptorSet{};

  public:
    PBRMaterialResource(PBRMaterial *material) : MaterialResource((Material *)material)
    {
    }
    ~PBRMaterialResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    inline vk::Buffer GetStagingBuffer()
    {
        return mStagingBuffer;
    }
    inline VmaAllocationInfo GetStagingBufferAllocationInfo()
    {
        return mStagingBufferAllocationInfo;
    }
    inline vk::Buffer GetBuffer()
    {
        return mBuffer;
    }
    inline VmaAllocationInfo GetBufferAllocationInfo()
    {
        return mBufferAllocationInfo;
    }
    inline vk::DescriptorSet GetDescriptorSet()
    {
        return mDescriptorSet;
    }
};
} // namespace MEngine::Resource