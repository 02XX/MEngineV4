#pragma once
#include "Asset.hpp"
#include "AssetURL.hpp"
#include "Context.hpp"
#include "IManager.hpp"
#include "MemberInfo.hpp"
#include "RenderResource.hpp"
#include "UUID.hpp"
#include <any>
#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace MEngine::Resource
{
class AssetManager final : public virtual IManager, public virtual IPendingResourceManager
{
  private:
    // Route map
    std::unordered_map<std::type_index, std::shared_ptr<IManager>> mManagers{};
    std::unordered_map<std::type_index, std::type_index> mResourceTypeToAssetTypeMap{};

  private:
    AssetManager() = default;
    bool mIsShutdown{false};

  public:
    ~AssetManager();
    static AssetManager &Instance();
    void Init(std::shared_ptr<Context> context);
    void Shutdown(std::shared_ptr<Context> context);
    template <std::derived_from<Asset> TAsset, std::derived_from<RenderResource>... TRenderResource>
    void RegisterManager(std::shared_ptr<IManager> manager)
    {
        assert(manager != nullptr && "Cannot register a null manager!");
        mManagers[std::type_index(typeid(TAsset))] = manager;
        (mResourceTypeToAssetTypeMap.insert_or_assign(std::type_index(typeid(TRenderResource)),
                                                      std::type_index(typeid(TAsset))),
         ...);
    }
    template <std::derived_from<Asset> TAsset> std::shared_ptr<IManager> GetManager() const
    {
        auto it = mManagers.find(std::type_index(typeid(TAsset)));
        if (it != mManagers.end())
        {
            return it->second;
        }
        return nullptr;
    }
    void Import(const AssetURL &url) override;
    void Export(std::shared_ptr<Asset> asset, const AssetURL &url) override;
    std::shared_ptr<Asset> Load(const AssetURL &url) override;
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override;
    void Add(std::shared_ptr<Asset> asset) override;
    std::shared_ptr<Asset> Get(const Core::UUID &id) override;
    std::shared_ptr<Asset> GetByName(const std::string &name) override;
    void Remove(const Core::UUID &id) override;
    // TODO: More Perfect Type Identification
    std::type_index GetAssetType(Asset *asset) const;
    std::type_index GetAssetTypeFromRenderResource(RenderResource *resource) const;
    std::type_index GetAssetTypeFromURL(const AssetURL &url) const;
    void ProcessPendingInitResources(RenderContext renderContext) override;
    void ProcessPendingUpdateResources(RenderContext renderContext) override;
    void ProcessPendingDeletionResources(RenderContext renderContext) override;
    void ProcessPendingTasks(RenderContext renderContext) override;
    void PendingInit(RenderResource *resource) override;
    void PendingUpdate(RenderResource *resource) override;
    void PendingDelete(std::unique_ptr<RenderResource> resource) override;
    void PendingTask(Task task) override;
};
} // namespace MEngine::Resource
