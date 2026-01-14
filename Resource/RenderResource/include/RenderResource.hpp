#pragma once
namespace MEngine::Resource
{
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
    void InitResource();
    void ReleaseResource();
    bool IsInitialized() const
    {
        return mState == State::Initialized;
    }

  protected:
    virtual void InitRHI() = 0;
    virtual void ReleaseRHI() = 0;

  private:
    State mState = State::Uninitialized;
};
} // namespace MEngine::Resource