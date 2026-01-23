
#pragma once
#include "IUpload.hpp"
#include "TextureResource.hpp"
#include <cstddef>
#include <cstdint>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class UploadableTexture;
class UploadableTextureResource : public TextureResource, IUpload
{
  public:
    // TODO: 是否一直持有？而不释放？
    vk::Buffer mStagingBuffer{};
    VmaAllocation mStagingBufferAllocation{};
    VmaAllocationInfo mStagingBufferAllocationInfo{};

    uint32_t mBindlessDescriptorIndex{0};
    vk::DescriptorSet mBindlessTextureDescriptorSet{};

  public:
    UploadableTextureResource(UploadableTexture *texture);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void Upload() override;
};
} // namespace MEngine::Resource
