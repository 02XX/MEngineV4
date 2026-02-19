#pragma once
#include "Asset.hpp"
#include "AssetURL.hpp"
#include "ISerizalization.hpp"
#include "RflEntity.hpp"
#include <fstream>

namespace MEngine::Resource
{
/**
 * @brief Abstract class for serialization and deserialization of assets.
 *
 */

class Serialization : public virtual ISerialization
{

  protected:
    template <std::derived_from<Asset> TAsset, typename TAssetEntity>
    std::shared_ptr<TAsset> LoadHelper(const AssetURL &url,
                                       std::function<std::shared_ptr<TAsset>(TAssetEntity)> createAssetFromEntity)
    {
        auto path = url.GetPath();
        // std::ifstream file(path, std::ios::binary);
        std::ifstream file(path);
        if (!file.is_open())
        {
            LogError("Failed to open file: {}", path.string());
            return nullptr;
        }
        auto result = rfl::json::read<TAssetEntity>(file);
        if (!result)
        {
            LogError("Failed to deserialize: {}, {}", path.string(), result.error().what());
            return nullptr;
        }
        auto asset = createAssetFromEntity(result.value());
        return asset;
    }
    template <std::derived_from<Asset> TAsset, typename TAssetEntity>
    void SaveHelper(std::shared_ptr<TAsset> asset,
                    std::function<TAssetEntity(std::shared_ptr<TAsset>)> createEntityFromAsset, const AssetURL &url)
    {
        auto path = url.GetPath();
        std::ofstream file(path);
        if (!file.is_open())
        {
            LogError("Failed to open file for writing: {}", path.string());
            return;
        }
        auto entity = createEntityFromAsset(std::dynamic_pointer_cast<TAsset>(asset));
        rfl::json::write(entity, file);
        file.close();
    }
};
} // namespace MEngine::Resource