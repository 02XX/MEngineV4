#pragma once
#include "ConcurrentQueue.hpp"
#include "Context.hpp"
#include <functional>
#include <memory>

using namespace MEngine::Platform;
using namespace MEngine::Core;
namespace MEngine::Resource
{
inline ConcurrentQueue<std::function<void()>> PendingDeletions{};

class RenderResource
{
  public:
    RenderResource() = default;
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