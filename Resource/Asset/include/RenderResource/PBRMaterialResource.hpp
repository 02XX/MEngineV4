#pragma once
#include "Context.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "MaterialResource.hpp"
#include <memory>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class PBRMaterial;
// Abstract class
class PBRMaterialResource : public MaterialResource, public virtual IUpload, public virtual IBind
{
  public:
    vk::Buffer mBuffer;
    VmaAllocation mBufferAllocation;
    VmaAllocationInfo mBufferAllocationInfo;
    vk::DeviceAddress mBufferAddress;

    vk::Buffer mStagingBuffer;
    VmaAllocation mStagingBufferAllocation;
    VmaAllocationInfo mStagingBufferAllocationInfo;

    uint32_t mSetIndex{2};
    vk::DescriptorSet mDescriptorSet{};

  private:
  public:
    PBRMaterialResource(PBRMaterial *material);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void Upload() override;
    virtual void Bind(BindContext bindContext) override;
};
} // namespace MEngine::Resource