#include "RHITextureView.hpp"
#include "RHIContext.hpp"

namespace MEngine::Platform
{
RHITextureView::RHITextureView(const RHITextureViewDesc &createInfo) : mTextureViewDesc(createInfo)
{
    auto &device = RHIContext::Instance().GetDevice();
    mImageView = device.createImageViewUnique(mTextureViewDesc);
    if (!mImageView)
    {
        throw std::runtime_error("Failed to create image view!");
    }
}
} // namespace MEngine::Platform