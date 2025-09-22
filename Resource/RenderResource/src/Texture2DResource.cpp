#include "Texture2DResource.hpp"
#include "Logger.hpp"
#include "RHIContext.hpp"
#include "RHIHandler.hpp"
#include "RHITexture.hpp"
#include "Texture2D.hpp"
namespace MEngine::Resource
{
void Texture2DResource::InitRHI()
{
    auto &rhiContext = RHIContext::Instance();
    auto &device = rhiContext.GetDevice();
    auto &instance = rhiContext.GetInstance();
    vk::DispatchLoaderDynamic dld(instance, vkGetInstanceProcAddr, device, vkGetDeviceProcAddr);
    mTexture->mTextureSettings.imageType = vk::ImageType::e2D;
    mTexture->mTextureSettings.arrayLayers = 1;
    mTexture->mTextureSettings.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled |
                                       vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eHostTransferEXT;
    mRHITextureHandler = RHIHandler<RHITexture>(new RHITexture(mTexture->mTextureSettings));
    if (!mTexture->mTextureData.empty())
    {
        if (mRHITextureHandler->mTextureDesc.mipLevels != mTexture->mTextureData.size())
        {
            LogWarn("Texture mipmap levels do not match the provided mip data size. Using the smaller mipmap level of "
                    "the two.");
        }
        mRHITextureHandler->TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);
        std::vector<vk::MemoryToImageCopyEXT> copyRegions(mTexture->mTextureSettings.mipLevels);
        auto [channel, pixSize] = GetPixelSize(mTexture->mTextureSettings.format);
        for (size_t i = 0; i < copyRegions.size(); i++)
        {
            auto &mip = mTexture->mTextureData[i];
            copyRegions[i]
                .setMemoryImageHeight(mip.SizeY)
                .setMemoryRowLength(mip.SizeX * channel)
                .setImageSubresource(vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor,
                                                                static_cast<uint32_t>(i), 0,
                                                                mRHITextureHandler->mTextureDesc.arrayLayers})
                .setImageOffset(vk::Offset3D{0, 0, 0})
                .setImageExtent(vk::Extent3D{std::max(1u, mip.SizeX), std::max(1u, mip.SizeY), 1})
                .setPHostPointer(mip.Data.data());
        }
        vk::CopyMemoryToImageInfoEXT copyInfo{};
        copyInfo.setDstImage(mRHITextureHandler->mImage)
            .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
            .setRegions(copyRegions);
        device.copyMemoryToImageEXT(copyInfo, dld);
        mRHITextureHandler->TransitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    }
    mRHISamplerHandler = RHIHandler<RHISampler>(new RHISampler(mTexture->mSamplerSettings));
    RHITextureViewDesc viewDesc{};
    viewDesc.image = mRHITextureHandler->GetImage();
    viewDesc.viewType = vk::ImageViewType::e2D;
    viewDesc.format = mRHITextureHandler->mTextureDesc.format;
    viewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewDesc.subresourceRange.baseMipLevel = 0;
    viewDesc.subresourceRange.levelCount = mRHITextureHandler->mTextureDesc.mipLevels;
    viewDesc.subresourceRange.baseArrayLayer = 0;
    viewDesc.subresourceRange.layerCount = 1;
    mRHITextureViewHandler = RHIHandler<RHITextureView>(new RHITextureView(viewDesc));
}

void Texture2DResource::ReleaseRHI()
{
    mRHITextureHandler.SafeRelease();
    mRHISamplerHandler.SafeRelease();
}
} // namespace MEngine::Resource
