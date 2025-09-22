#include "FrameBuffer.hpp"
#include "Texture.hpp"

namespace MEngine::Resource
{
FrameBuffer::~FrameBuffer()
{
    for (const auto &colorAttachment : mColorAttachments)
    {
        if (colorAttachment && colorAttachment->GetResource())
        {
            colorAttachment->GetResource()->ReleaseResource();
        }
    }
    if (mDepthStencilAttachment && mDepthStencilAttachment->GetResource())
    {
        mDepthStencilAttachment->GetResource()->ReleaseResource();
    }
}
FrameBuffer::FrameBuffer(const std::string &name, uint32_t width, uint32_t height)
    : Asset(name), mWidth(width), mHeight(height)
{
    mColorAttachments.reserve(4);
    // Color
    TextureSetting colorSetting{};
    colorSetting.extent = vk::Extent3D{mWidth, mHeight, 1};
    colorSetting.mipLevels = 1;
    colorSetting.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst |
                         vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled;
    colorSetting.samples = vk::SampleCountFlagBits::e1;
    colorSetting.format = vk::Format::eR8G8B8A8Srgb;
    SamplerSetting samplerSetting{};
    // Color
    mColorAttachments.push_back(std::make_shared<TextureRenderTarget2D>("Color", colorSetting, samplerSetting));

    colorSetting.format = vk::Format::eR32G32B32A32Sfloat;
    // Albedo
    mColorAttachments.push_back(std::make_shared<TextureRenderTarget2D>("Albedo", colorSetting, samplerSetting));
    // Normal
    mColorAttachments.push_back(std::make_shared<TextureRenderTarget2D>("Normal", colorSetting, samplerSetting));
    // ARM
    mColorAttachments.push_back(std::make_shared<TextureRenderTarget2D>("ARM", colorSetting, samplerSetting));
    // Position
    mColorAttachments.push_back(std::make_shared<TextureRenderTarget2D>("Position", colorSetting, samplerSetting));
    // Emissive
    mColorAttachments.push_back(std::make_shared<TextureRenderTarget2D>("Emissive", colorSetting, samplerSetting));
    // DepthStencil
    TextureSetting depthStencilSetting{};
    depthStencilSetting.extent = vk::Extent3D{mWidth, mHeight, 1};
    depthStencilSetting.mipLevels = 1;
    depthStencilSetting.format = vk::Format::eD32SfloatS8Uint;
    depthStencilSetting.samples = vk::SampleCountFlagBits::e1;
    mDepthStencilAttachment =
        std::make_shared<TextureRenderTarget2D>("DepthStencil", depthStencilSetting, samplerSetting);
}
} // namespace MEngine::Resource