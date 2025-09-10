#pragma once
#include "TextureRenderTarget.hpp"
namespace MEngine::Function
{
class TextureRenderTarget2DBuilder;
class ColorAttachmentBuilder;
class DepthStencilAttachmentBuilder;
} // namespace MEngine::Function
namespace MEngine::Resource
{
class TextureRenderTarget2D : public TextureRenderTarget
{
    friend class Function::TextureRenderTarget2DBuilder;
    friend class Function::ColorAttachmentBuilder;
    friend class Function::DepthStencilAttachmentBuilder;

  protected:
    TextureRenderTarget2D() : TextureRenderTarget()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 1;
    }

  public:
    ~TextureRenderTarget2D() override = default;
};
} // namespace MEngine::Resource