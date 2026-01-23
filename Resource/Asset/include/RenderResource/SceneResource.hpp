#pragma once
#include "ECS.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <array>
#include <cstdint>
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
    uint32_t mSetIndex{2};
    vk::DescriptorSet mGlobalDescriptorSet{};

  private:
    vk::BindDescriptorSetsInfo mBindInfo{};

  protected:
    Scene *mScene{nullptr};

  public:
    SceneResource(Scene *scene);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;

    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void Upload() override;
    void Bind(BindContext bindContext) override;
};
} // namespace MEngine::Resource
