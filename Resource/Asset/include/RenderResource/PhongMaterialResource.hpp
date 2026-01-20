#pragma once
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <cstdint>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class PhongMaterial;
class PhongMaterialResource final : public MaterialResource
{
  public:
    vk::DeviceAddress mBufferAddress{};

  public:
    vk::Buffer mBuffer, mStagingBuffer;
    VmaAllocation mBufferAllocation, mStagingBufferAllocation;
    VmaAllocationInfo mBufferAllocationInfo, mStagingBufferAllocationInfo;

    vk::DescriptorSet mDescriptorSet{};

  public:
    PhongMaterialResource(PhongMaterial *material) : MaterialResource((Material *)material)
    {
    }
    ~PhongMaterialResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource