#include "TextureRenderTargetResource.hpp"
#include "Logger.hpp"
#include "RenderResource.hpp"
#include "TextureRenderTarget.hpp"
namespace MEngine::Resource
{
TextureRenderTargetResource::TextureRenderTargetResource(TextureRenderTarget *textureRenderTarget)
    : TextureResource(textureRenderTarget)
{
}
void TextureRenderTargetResource::InitRHI(std::shared_ptr<Context> context)
{
    TextureResource::InitRHI(context);
}
void TextureRenderTargetResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    TextureResource::ReleaseRHI(context);
}
} // namespace MEngine::Resource