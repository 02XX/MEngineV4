#include "RenderResource.hpp"
#include "Logger.hpp"
namespace MEngine::Resource
{
RenderResource::RenderResource(const RenderResource &other)
{
    mIsInitialized = other.mIsInitialized;
    mState = other.mState;
}
RenderResource &RenderResource::operator=(const RenderResource &other)
{
    if (this != &other)
    {
        mIsInitialized = other.mIsInitialized;
        mState = other.mState;
    }
    return *this;
}
RenderResource::RenderResource(RenderResource &&other) noexcept
{
    mIsInitialized = other.mIsInitialized;
    mState = other.mState;
    other.mIsInitialized = false;
    other.mState = State::Deleted;
}
RenderResource &RenderResource::operator=(RenderResource &&other) noexcept
{
    if (this != &other)
    {
        mIsInitialized = other.mIsInitialized;
        mState = other.mState;
        other.mIsInitialized = false;
        other.mState = State::Deleted;
    }
    return *this;
}
RenderResource::~RenderResource()
{
    // if (mState != State::Deleted)
    // {
    //     ReleaseResource();
    // }
}
void RenderResource::ReleaseResource()
{
    if (mIsInitialized)
    {
        ReleaseRHI();
        mIsInitialized = false;
        mState = State::Deleted;
    }
    // else
    // {
    //     LogWarn("RenderResource::ReleaseResource called on uninitialized resource");
    // }
}
void RenderResource::InitResource()
{
    if (!mIsInitialized)
    {
        InitRHI();
        mIsInitialized = true;
    }
}
void RenderResource::UpdateRHI()
{
    if (mIsInitialized)
    {
        ReleaseRHI();
        InitRHI();
    }
}
} // namespace MEngine::Resource