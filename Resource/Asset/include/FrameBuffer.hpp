#pragma once
#include "Asset.hpp"
#include "TextureRenderTarget2D.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace MEngine::Resource
{
struct FrameBufferSettings
{
    uint32_t width{800};
    uint32_t height{600};
    std::vector<std::shared_ptr<TextureRenderTarget2D>> mColorAttachments{};
    std::shared_ptr<TextureRenderTarget2D> mDepthStencilAttachment{};
};
class FrameBuffer : public Asset
{
  private:
    // FrameBufferSettings mSettings{};
    uint32_t mWidth{800};
    uint32_t mHeight{600};
    std::vector<std::shared_ptr<TextureRenderTarget2D>> mColorAttachments{};
    std::shared_ptr<TextureRenderTarget2D> mDepthStencilAttachment{};

  public:
    FrameBuffer(const std::string &name, uint32_t width, uint32_t height);
    FrameBuffer(const FrameBufferSettings &settings)
        : Asset("FrameBuffer"), mWidth(settings.width), mHeight(settings.height),
          mColorAttachments(settings.mColorAttachments), mDepthStencilAttachment(settings.mDepthStencilAttachment)
    {
    }
    ~FrameBuffer() override;
    inline const std::vector<std::shared_ptr<TextureRenderTarget2D>> &GetColorAttachments() const
    {
        return mColorAttachments;
    }
    inline const std::shared_ptr<TextureRenderTarget2D> &GetDepthStencilAttachment() const
    {
        return mDepthStencilAttachment;
    }
    inline void ReplaceDepthStencilAttachment(const std::shared_ptr<TextureRenderTarget2D> &depthStencil)
    {
        mDepthStencilAttachment = depthStencil;
    }
    inline void ReplaceColorAttachment(uint32_t index, const std::shared_ptr<TextureRenderTarget2D> &colorAttachment)
    {
        if (index >= mColorAttachments.size())
        {
            throw std::out_of_range("Color attachment index out of range");
        }
        mColorAttachments[index] = colorAttachment;
    }
};
} // namespace MEngine::Resource
