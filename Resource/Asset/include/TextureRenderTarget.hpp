#pragma once
#include "Texture.hpp"
#include "TextureRenderTargetResource.hpp"
namespace MEngine::Resource
{
using ClearValue = vk::ClearValue;
class TextureRenderTarget : public Texture
{
    friend class TextureRenderTargetResource;

  public:
    // Default black for color attachment
    ClearValue mClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})};

  protected:
  public:
    TextureRenderTarget(const std::string &name, const TextureSetting &textureSetting) : Texture(name, textureSetting)
    {
        if (IsDepthStencil())
        {
            mClearValue = vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0));
            mTextureSettings.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment |
                                      vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc |
                                      vk::ImageUsageFlagBits::eTransferDst);
        }
        mResource = std::make_unique<TextureRenderTargetResource>(this);
    }
};
} // namespace MEngine::Resource