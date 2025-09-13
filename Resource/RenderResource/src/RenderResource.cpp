#include "RenderResource.hpp"
#include "Logger.hpp"
namespace MEngine::Resource
{
RenderResource::~RenderResource()
{
    mState = State::Deleted;
    if (mIsInitialized)
    {
        LogError("RenderResource was not released before destruction!");
    }
}
void RenderResource::ReleaseResource()
{
    if (mIsInitialized)
    {
        ReleaseRHI();
        mIsInitialized = false;
    }
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