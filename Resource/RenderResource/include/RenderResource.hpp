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

    /** Controls initialization order of render resources. Early engine resources utilize the 'Pre' phase to avoid
     * static init ordering issues. */
    enum class InitPhase
    {
        Pre,
        Default, 
        MAX
    };
    enum class State
    {
        Default,
        Deleted
    };
    /** Release all render resources that are currently initialized. */
    static void ReleaseRHIForAllResources();
    /** Initialize all resources initialized before the RHI was initialized. */
    static void InitPreRHIResources();
    ////////////////////////////////////////////////////////////////////////////////////

    /**
     * Initializes the RHI resources used by this resource.
     * Called when entering the state where both the resource and the RHI have been initialized.
     * This is only called by the rendering thread.
     */
    virtual void InitRHI() = 0;
    virtual void ReleaseRHI() = 0;
    /**
     * Initializes the resource.
     * This is only called by the rendering thread.
     */
    void InitResource();
    /**
     * Prepares the resource for deletion.
     * This is only called by the rendering thread.
     */
    void ReleaseResource();
    /**
     * If the resource's RHI resources have been initialized, then release and reinitialize it.  Otherwise, do nothing.
     * This is only called by the rendering thread.
     */
    void UpdateRHI();

    // Accessors.
    bool IsInitialized() const
    {
        return mIsInitialized;
    }

  private:
    bool mIsInitialized = false;
    InitPhase mPhase = InitPhase::Default;
    State mState = State::Default;
};
} // namespace MEngine::Resource