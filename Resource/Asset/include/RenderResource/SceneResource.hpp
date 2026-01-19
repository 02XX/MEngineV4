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
  public:
    vk::Buffer mUBOBuffer{}, mUBOStagingBuffer{};
    VmaAllocation mUBOBufferAllocation{}, mUBOStagingBufferAllocation{};
    VmaAllocationInfo mUBOBufferAllocationInfo{}, mUBOStagingBufferAllocationInfo{};
    vk::DeviceAddress mUBOBufferAddress{};

    vk::Buffer mSSBOBuffer{}, mSSBOStagingBuffer{};
    VmaAllocation mSSBOBufferAllocation{}, mSSBOStagingBufferAllocation{};
    VmaAllocationInfo mSSBOBufferAllocationInfo{}, mSSBOStagingBufferAllocationInfo{};
    vk::DeviceAddress mSSBOBufferAddress{};

    vk::DescriptorSet mDescriptorSet{};

  protected:
    Scene *mScene{nullptr};

  public:
    SceneResource(Scene *scene);
    ~SceneResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource
