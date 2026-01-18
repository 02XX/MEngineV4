
#pragma once
#include "TextureResource.hpp"
#include <cstdint>

using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Texture2D;
class Texture2DResource : public TextureResource
{
  protected:
    vk::Buffer mStagingBuffer;
    VmaAllocation mStagingBufferAllocation;
    VmaAllocationInfo mStagingBufferAllocationInfo{};

  public:
    uint32_t mBindlessDescriptorIndex{0};
    bool mWroteStagingBuffer{false};

  public:
    Texture2DResource(Texture2D *texture2D);
    ~Texture2DResource() override = default;
    virtual void InitRHI(std::shared_ptr<Context> context) override;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) override;
    inline vk::Buffer GetStagingBuffer() const
    {
        return mStagingBuffer;
    }
    inline VmaAllocationInfo GetStagingBufferAllocationInfo() const
    {
        return mStagingBufferAllocationInfo;
    }
    inline VmaAllocation GetStagingBufferAllocation() const
    {
        return mStagingBufferAllocation;
    }
};
} // namespace MEngine::Resource
