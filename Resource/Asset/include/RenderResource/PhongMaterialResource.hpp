#pragma once
#include "Context.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "MaterialResource.hpp"
#include <memory>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class PhongMaterial;
// Abstract class
class PhongMaterialResource : public MaterialResource, public virtual IUpload, public virtual IBind
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

  private:
    vk::BindDescriptorSetsInfo mBindInfo{};

  public:
    PhongMaterialResource(PhongMaterial *material);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void Upload() override;
    virtual void Bind(BindContext bindContext) override;
};
} // namespace MEngine::Resource