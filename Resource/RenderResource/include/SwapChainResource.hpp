#pragma once
#include "FrameResource.hpp"
#include "RHIHandler.hpp"
#include "RHISwapChain.hpp"
#include "RHITexture.hpp"
#include "RHITextureView.hpp"
#include "RenderResource.hpp"
#include <vector>


using namespace MEngine::Platform;
namespace MEngine::Resource
{
class SwapChainResource : public RenderResource
{
  private:
    std::vector<RHIHandler<RHITexture>> mSwapChainTextures{};
    std::vector<RHIHandler<RHITextureView>> mSwapChainTextureViews{};
    RHIHandler<RHISwapChain> mRHISwapChainHandler{};

  public:
    SwapChainResource() : RenderResource()
    {
    }
    ~SwapChainResource() override = default;
    virtual void InitRHI() override;
    virtual void ReleaseRHI() override;
    inline const std::vector<RHIHandler<RHITexture>> &GetSwapChainTextures() const
    {
        return mSwapChainTextures;
    }
    inline const std::vector<RHIHandler<RHITextureView>> &GetSwapChainTextureViews() const
    {
        return mSwapChainTextureViews;
    }
    inline RHIHandler<RHISwapChain> GetRHISwapChain() const
    {
        return mRHISwapChainHandler;
    }
    void Present(FrameResource &frameResource);
};
} // namespace MEngine::Resource