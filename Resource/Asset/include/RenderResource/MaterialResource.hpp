#pragma once
#include "Context.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "RenderResource.hpp"
#include <array>
#include <cstdint>
#include <memory>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Material;
// Abstract class
class MaterialResource : public RenderResource, public virtual IUpload, public virtual IBind
{
  public:
    vk::Buffer mBuffer;
    VmaAllocation mBufferAllocation;
    VmaAllocationInfo mBufferAllocationInfo;
    vk::DeviceAddress mBufferAddress;

    vk::Buffer mStagingBuffer;
    VmaAllocation mStagingBufferAllocation;
    VmaAllocationInfo mStagingBufferAllocationInfo;

    vk::DescriptorSet mDescriptorSet{};

  public:
    MaterialResource(Material *material);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void UploadData() override;
    virtual void Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout = {},
                      vk::Pipeline pipeline = {}) override;
};
} // namespace MEngine::Resource