#pragma once
#include "Asset.hpp"
#include "AssetURL.hpp"
#include "IManager.hpp"
#include "Logger.hpp"
#include "UUID.hpp"
#include <concepts>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <typeindex>
#include <unordered_map>

using JSON = nlohmann::json;
using namespace MEngine::Resource;
namespace MEngine::Resource
{
class AssetManager
{
  private:
    std::shared_ptr<Context> mContext;

  private:
    std::unordered_map<std::type_index, std::any> mManagers;

  public:
    AssetManager(std::shared_ptr<Context> context);
    template <std::derived_from<Asset> TAsset> void RegisterManager(std::shared_ptr<IManager<TAsset>> manager)
    {
        mManagers[std::type_index(typeid(TAsset))] = manager;
    }
    template <std::derived_from<Asset> TAsset> void UnregisterManager()
    {
        auto typeIdx = std::type_index(typeid(TAsset));
        if (mManagers.contains(typeIdx))
        {
            mManagers.erase(typeIdx);
        }
        else
        {
            LogWarn("Manager for asset type {} not found", typeid(TAsset).name());
        }
    }
    template <std::derived_from<Asset> TAsset, std::derived_from<IManager<TAsset>> TManager = IManager<TAsset>>
    std::shared_ptr<TManager> GetManager()
    {
        auto type = std::type_index(typeid(TAsset));
        if (!mManagers.contains(type))
        {
            LogError("Manager for asset type {} not found", typeid(TAsset).name());
            return nullptr;
        }
        auto manager = std::any_cast<std::shared_ptr<IManager<TAsset>>>(mManagers.at(type));
        return std::dynamic_pointer_cast<TManager>(manager);
    }
    
    virtual ~AssetManager()
    {
        mManagers.clear();
    }
    template <std::derived_from<Asset> TAsset> void AddAsset(std::shared_ptr<TAsset> asset)
    {
        if (!asset)
        {
            LogError("Cannot add null asset");
            return;
        }
        auto manager = GetManager<TAsset>();
        manager->Add(asset);
    }
    template <std::derived_from<Asset> TAsset> std::unique_ptr<TAsset> LoadAsset(const AssetURL &url)
    {
        auto path = url.GetPath();
        if (!std::filesystem::exists(path))
        {
            LogError("Asset file {} does not exist", path.string());
            return nullptr;
        }
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            LogError("Failed to open asset file {}", path.string());
            return nullptr;
        }
        // JSON j = JSON::from_msgpack(file);
        JSON j = JSON::parse(file);
        file.close();
        auto asset = std::unique_ptr<TAsset>(new TAsset());
        j.get_to(*asset);
        return asset;
    }
    template <std::derived_from<Asset> TAsset> void SaveAsset(std::shared_ptr<TAsset> asset, const AssetURL &url)
    {
        auto path = url.GetPath();
        auto dir = path.parent_path();
        if (!std::filesystem::exists(dir))
        {
            std::filesystem::create_directories(dir);
        }
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            LogError("Failed to open asset file {} for writing", path.string());
            return;
        }
        JSON j = *asset;
        // auto msgpackData = JSON::to_msgpack(j);
        // file.write(reinterpret_cast<const char *>(msgpackData.data()), msgpackData.size());
        file << j;
        file.close();
    }
    template <std::derived_from<Asset> TAsset> std::shared_ptr<TAsset> GetByName(const std::string &name)
    {
        auto manager = GetManager<TAsset>();
        return manager->GetByName(name);
    }
    template <std::derived_from<Asset> TAsset> std::shared_ptr<TAsset> GetByID(const Core::UUID &id)
    {
        auto manager = GetManager<TAsset>();
        return manager->Get(id);
    }
};
} // namespace MEngine::Resource
