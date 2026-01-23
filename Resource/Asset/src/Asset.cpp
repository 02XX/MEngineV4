#include "Asset.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"

namespace MEngine::Resource
{
void Asset::PendingInit()
{
    if (!mResource)
        return;
    if (mResource->IsInitialized())
    {
        LogTrace("Asset Resource for「{}」 is already initialized, cannot init again.", mName);
        return;
    }
    AssetManager::Instance().PendingInit(mResource.get());
}
void Asset::PendingUpdate()
{
    if (!mResource)
        return;
    if (!mResource->IsInitialized())
    {
        LogTrace("Asset Resource for「{}」is not initialized, cannot update.", mName);
        return;
    }
    AssetManager::Instance().PendingUpdate(mResource.get());
}
void Asset::PendingDeletion()
{
    if (!mResource)
        return;
    if (!mResource->IsInitialized())
    {
        LogTrace("Asset Resource for「{}」is not initialized, cannot deletion.", mName);
        return;
    }
    AssetManager::Instance().PendingDelete(std::move(mResource));
}
} // namespace MEngine::Resource