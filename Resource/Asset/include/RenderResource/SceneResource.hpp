#pragma once
#include "ECS.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Scene;

class SceneResource final : public RenderResource
{
  private:
    vk::Buffer mBuffer, mStagingBuffer;
    VmaAllocation mBufferAllocation, mStagingBufferAllocation;
    VmaAllocationInfo mBufferAllocationInfo, mStagingBufferAllocationInfo;
    vk::DescriptorSet mDescriptorSet{};
    vk::DeviceAddress mBufferAddress{};

  protected:
    Scene *mScene{nullptr};

  public:
    SceneResource(Scene *scene);
    ~SceneResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    inline vk::Buffer GetBuffer()
    {
        return mBuffer;
    }
    inline VmaAllocationInfo GetBufferAllocationInfo()
    {
        return mBufferAllocationInfo;
    }
    inline vk::Buffer GetStagingBuffer()
    {
        return mStagingBuffer;
    }
    inline VmaAllocationInfo GetStagingBufferAllocationInfo()
    {
        return mStagingBufferAllocationInfo;
    }
    inline vk::DescriptorSet GetDescriptorSet()
    {
        return mDescriptorSet;
    }
    inline vk::DeviceAddress GetBufferAddress()
    {
        return mBufferAddress;
    }
};
} // namespace MEngine::Resource
