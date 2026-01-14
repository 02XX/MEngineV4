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
void RenderResource::ReleaseResource(std::shared_ptr<Context> context)
{
    if (mState == State::Initialized)
    {
        ReleaseRHI(context);
        mState = State::Released;
    }
}
void RenderResource::InitResource(std::shared_ptr<Context> context)
{
    if (mState != State::Initialized)
    {
        InitRHI(context);
        mState = State::Initialized;
    }
}
} // namespace MEngine::Resource