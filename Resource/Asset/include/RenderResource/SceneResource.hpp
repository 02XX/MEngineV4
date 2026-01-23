#pragma once
#include "ECS.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <array>
#include <memory>
#include <vector>

using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Scene;

class SceneResource final : public RenderResource, public virtual IUpload, public virtual IBind
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
    vk::DescriptorSet mGlobalDescriptorSet{};

  protected:
    Scene *mScene{nullptr};

  public:
    SceneResource(Scene *scene);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;

    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void UploadData() override;
    void Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout = {},
              vk::Pipeline pipeline = {}) override;
};
} // namespace MEngine::Resource
