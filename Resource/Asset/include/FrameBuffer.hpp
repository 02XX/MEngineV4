#pragma once
#include "Asset.hpp"
#include "TextureRenderTarget2D.hpp"
#include <memory>
#include <vector>
namespace MEngine::Function
{
class MRTFrameBufferBuilder;
} // namespace MEngine::Function
namespace MEngine::Resource
{
class FrameBuffer : public Asset
{
    friend class Function::MRTFrameBufferBuilder;

  protected:
    vk::Extent3D mExtent{0, 0, 1};
    std::vector<std::shared_ptr<TextureRenderTarget2D>> mAttachments{};
    std::vector<vk::ClearValue> mClearValues{};
    vk::UniqueFramebuffer mFrameBuffer{nullptr};
    vk::FramebufferCreateInfo mCreateInfo{};

  protected:
    FrameBuffer() : Asset()
    {
        mName = "FrameBuffer";
    }

  public:
    ~FrameBuffer() override = default;
    inline const std::vector<std::shared_ptr<TextureRenderTarget2D>> &GetAttachments() const
    {
        return mAttachments;
    }
};
} // namespace MEngine::Resource