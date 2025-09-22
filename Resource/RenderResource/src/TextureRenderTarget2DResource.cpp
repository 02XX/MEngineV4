#include "TextureRenderTarget2DResource.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "TextureRenderTarget2D.hpp"
namespace MEngine::Resource
{
void TextureRenderTarget2DResource::InitRHI()
{
    RHITextureDesc desc{};
    desc.imageType = vk::ImageType::e2D;
    desc.extent = mTextureRenderTarget2D->mTextureSettings.extent;
    desc.mipLevels = mTextureRenderTarget2D->mTextureSettings.mipLevels;
    desc.format = mTextureRenderTarget2D->mTextureSettings.format;
    desc.arrayLayers = 1;
    desc.samples = mTextureRenderTarget2D->mTextureSettings.samples;

    desc.tiling = mTextureRenderTarget2D->mTextureSettings.tiling;
    desc.flags = mTextureRenderTarget2D->mTextureSettings.flags;
    switch (desc.format)
    {
    case vk::Format::eD16Unorm:
    case vk::Format::eX8D24UnormPack32:
    case vk::Format::eD32Sfloat:
    case vk::Format::eS8Uint:
    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        desc.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled |
                     vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
        break;
    default:
        desc.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled |
                     vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
        break;
    }
    RHISamplerDesc samplerDesc{};
    samplerDesc.magFilter = mTextureRenderTarget2D->mSamplerSettings.magFilter;
    samplerDesc.minFilter = mTextureRenderTarget2D->mSamplerSettings.minFilter;
    samplerDesc.mipmapMode = mTextureRenderTarget2D->mSamplerSettings.mipmapMode;
    samplerDesc.addressModeU = mTextureRenderTarget2D->mSamplerSettings.addressModeU;
    samplerDesc.addressModeV = mTextureRenderTarget2D->mSamplerSettings.addressModeV;
    samplerDesc.addressModeW = mTextureRenderTarget2D->mSamplerSettings.addressModeW;
    mTexture = RHIHandler<RHITexture>(new RHITexture(desc));
    switch (desc.format)
    {
    case vk::Format::eD16Unorm:
    case vk::Format::eX8D24UnormPack32:
    case vk::Format::eD32Sfloat:
    case vk::Format::eS8Uint:
    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        mTexture->TransitionImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        break;
    default:
        mTexture->TransitionImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
        break;
    }
    RHITextureViewDesc viewDesc{};
    viewDesc.image = mTexture->GetImage();
    viewDesc.viewType = vk::ImageViewType::e2D;
    viewDesc.format = mTexture->GetTextureDesc().format;
    if (mTexture->GetTextureDesc().usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        viewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }
    else
    {
        viewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    viewDesc.subresourceRange.baseMipLevel = 0;
    viewDesc.subresourceRange.levelCount = mTexture->GetTextureDesc().mipLevels;
    viewDesc.subresourceRange.baseArrayLayer = 0;
    viewDesc.subresourceRange.layerCount = 1;
    mSampler = RHIHandler<RHISampler>(new RHISampler(samplerDesc));
    mTextureView = RHIHandler<RHITextureView>(new RHITextureView(viewDesc));
}
void TextureRenderTarget2DResource::ReleaseRHI()
{
    mTexture.SafeRelease();
    mSampler.SafeRelease();
}
} // namespace MEngine::Resource