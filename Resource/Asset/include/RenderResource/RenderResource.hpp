#pragma once
#include "Context.hpp"
#include <memory>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Asset;
class RenderResource : public std::enable_shared_from_this<RenderResource>
{
    friend class AssetManager;

  public:
    Asset *mOwnerAsset{};

  public:
    RenderResource(Asset *ownerAsset);
    RenderResource(const RenderResource &other);
    RenderResource &operator=(const RenderResource &other);
    RenderResource(RenderResource &&other) noexcept;
    RenderResource &operator=(RenderResource &&other) noexcept;
    virtual ~RenderResource();
    ////////////////////////////////////////////////////////////////////////////////////
    enum class State
    {
        Uninitialized,
        Initialized,
        Released
    };
    void InitResource(std::shared_ptr<Context> context);
    void ReleaseResource(std::shared_ptr<Context> context);
    void InitResourceAsync();
    void ReleaseResourceAsync();

    bool IsInitialized() const
    {
        return mState == State::Initialized;
    }

  protected:
    virtual void InitRHI(std::shared_ptr<Context> context) = 0;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) = 0;

  private:
    State mState = State::Uninitialized;
};
} // namespace MEngine::Resource