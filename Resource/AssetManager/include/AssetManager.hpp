#pragma once
#include "Asset.hpp"
#include "AssetURL.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "UUID.hpp"
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>
using JSON = nlohmann::json;
using namespace MEngine::Resource;
namespace MEngine::Resource
{
class AssetManager
{
  private:
    std::unordered_map<UUID, std::shared_ptr<Asset>> mAssets;

  public:
    virtual ~AssetManager() = default;
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
};
} // namespace MEngine::Resource
