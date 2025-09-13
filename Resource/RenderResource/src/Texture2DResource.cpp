#include "Texture2DResource.hpp"
#include "Logger.hpp"
#include "RHIContext.hpp"
namespace MEngine::Resource
{
void Texture2DResource::InitRHI()
{
    auto &rhiContext = Platform::RHIContext::Instance();
    auto device = rhiContext.GetDevice();

    mRHITextureHandler->mImageCreateInfo.setImageType(mRHITextureHandler->mType)
        .setExtent(mRHITextureHandler->mExtent)
        .setMipLevels(mRHITextureHandler->mMipmapLevels)
        .setArrayLayers(mRHITextureHandler->mArrayLevel)
        .setFormat(mRHITextureHandler->mFormat)
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(mRHITextureHandler->mUsages)
        .setSamples(mRHITextureHandler->mSampleCount)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setFlags(mRHITextureHandler->mImageCreateFlags);
    mRHITextureHandler->mAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateImage(
            rhiContext.GetVmaAllocator(), &static_cast<VkImageCreateInfo &>(mRHITextureHandler->mImageCreateInfo),
            &mRHITextureHandler->mAllocationCreateInfo, reinterpret_cast<VkImage *>(&mRHITextureHandler->mImage),
            &mRHITextureHandler->mAllocation, &mRHITextureHandler->mAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create image for texture resource");
        return;
    }
    if (!mTexture->GetMipDatas().empty())
    {
        if (mRHITextureHandler->mMipmapLevels != mTexture->GetMipDatas().size())
        {
            LogWarn("Texture mipmap levels do not match the provided mip data size. Using the smaller mipmap level of "
                    "the two.");
        }
        std::vector<vk::MemoryToImageCopyEXT> copyRegions(
            std::min(mRHITextureHandler->mMipmapLevels, static_cast<unsigned int>(mTexture->GetMipDatas().size())));
        for (size_t i = 0; i < copyRegions.size(); i++)
        {
            copyRegions[i]
                .setMemoryImageHeight(mTexture->GetHeight() >> i)
                .setMemoryRowLength((mTexture->GetWidth() >> i) * mTexture->GetChannels())
                .setImageSubresource(vk::ImageSubresourceLayers{
                    vk::ImageAspectFlagBits::eColor, static_cast<uint32_t>(i), 0, mRHITextureHandler->mArrayLevel})
                .setImageOffset(vk::Offset3D{0, 0, 0})
                .setImageExtent(
                    vk::Extent3D{std::max(1u, mTexture->GetWidth() >> i), std::max(1u, mTexture->GetHeight() >> i), 1})
                .setPHostPointer(mTexture->GetMipDatas()[i].data());
        }
        vk::CopyMemoryToImageInfoEXT copyInfo{};
        copyInfo.setDstImage(mRHITextureHandler->mImage)
            .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
            .setRegions(copyRegions);
        device.copyMemoryToImageEXT(copyInfo);
    }
    mRHISamplerHandler->mSamplerCreateInfo.setMagFilter(mRHISamplerHandler->magFilter)
        .setMinFilter(mRHISamplerHandler->minFilter)
        .setMipmapMode(mRHISamplerHandler->mipmapMode)
        .setAddressModeU(mRHISamplerHandler->addressModeU)
        .setAddressModeV(mRHISamplerHandler->addressModeV)
        .setAddressModeW(mRHISamplerHandler->addressModeW)
        .setMipLodBias(mRHISamplerHandler->mipLodBias)
        .setAnisotropyEnable(mRHISamplerHandler->anisotropyEnable)
        .setMaxAnisotropy(mRHISamplerHandler->maxAnisotropy)
        .setCompareEnable(mRHISamplerHandler->compareEnable)
        .setCompareOp(mRHISamplerHandler->compareOp)
        .setMinLod(mRHISamplerHandler->minLod)
        .setMaxLod(mRHISamplerHandler->maxLod)
        .setBorderColor(mRHISamplerHandler->borderColor)
        .setUnnormalizedCoordinates(mRHISamplerHandler->unnormalizedCoordinates);
    mRHISamplerHandler->mSampler = device.createSamplerUnique(mRHISamplerHandler->mSamplerCreateInfo);
    if (!mRHISamplerHandler->mSampler)
    {
        LogError("Failed to create sampler for texture resource");
        return;
    }
}

void Texture2DResource::ReleaseRHI()
{
    mRHITextureHandler.Release();
    mRHISamplerHandler.Release();
}
} // namespace MEngine::Resource
