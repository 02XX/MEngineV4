#include "RenderResource.hpp"
#include "Logger.hpp"
namespace MEngine::Resource
{
RenderResource::RenderResource(const RenderResource &other)
{
    mState = other.mState;
}
RenderResource &RenderResource::operator=(const RenderResource &other)
{
    if (this != &other)
    {
        mState = other.mState;
    }
    return *this;
}
RenderResource::RenderResource(RenderResource &&other) noexcept
{
    mState = other.mState;
    other.mState = State::Released;
}
RenderResource &RenderResource::operator=(RenderResource &&other) noexcept
{
    if (this != &other)
    {
        mState = other.mState;
        other.mState = State::Released;
    }
    return *this;
}
RenderResource::~RenderResource()
{
}
void RenderResource::ReleaseResource()
{
    if (mState == State::Initialized)
    {
        ReleaseRHI();
        mState = State::Released;
    }
}
void RenderResource::InitResource()
{
    if (mState != State::Initialized)
    {
        InitRHI();
        mState = State::Initialized;
    }
}
} // namespace MEngine::Resource