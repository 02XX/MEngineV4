#pragma once
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <cstdint>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
struct PBRMaterialPushConstants
{
    Matrix4 ModelMatrix;
    vk::DeviceAddress SceneSSBOAddress;
    vk::DeviceAddress MaterialSSBOAddress;
};
class PBRMaterial;
class PBRMaterialResource final : public MaterialResource
{
  public:
    vk::DeviceAddress mSSBOAddress{};

  protected:
    vk::Buffer mSSBO{};
    VmaAllocation mSSBOAllocation{};
    VmaAllocationInfo mSSBOAllocationInfo{};

    vk::Buffer mStagingBuffer{};
    VmaAllocation mStagingBufferAllocation{};
    VmaAllocationInfo mStagingBufferAllocationInfo{};

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
    inline vk::Buffer GetSSBO()
    {
        return mSSBO;
    }
    inline VmaAllocationInfo GetSSBOAllocationInfo()
    {
        return mSSBOAllocationInfo;
    }
};
} // namespace MEngine::Resource