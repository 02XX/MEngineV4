#pragma once
#include "Asset.hpp"
#include "AssetURL.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "UUID.hpp"
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;
using namespace MEngine::Resource;
namespace MEngine::Resource
{
class AssetManager
{
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

// serialize and deserialize
namespace nlohmann
{
template <> struct adl_serializer<UUID>
{
    static void to_json(json &j, const UUID &p)
    {
        j = p.ToString();
    }
    static void from_json(const json &j, UUID &p)
    {
        p = UUID(j.get<std::string>());
    }
};
template <> struct adl_serializer<Asset>
{
    static void to_json(json &j, const Asset &asset)
    {
        j["ID"] = asset.mID;
        j["Name"] = asset.mName;
    }
    static void from_json(const json &j, Asset &asset)
    {
        asset.mID = j.at("ID").get<UUID>();
        asset.mName = j.at("Name").get<std::string>();
    }
};
template <> struct adl_serializer<Shader>
{
    static void to_json(json &j, const Shader &shader)
    {
        j = static_cast<const Asset &>(shader);
        j["GLSLFilePath"] = shader.mGLSLFilePath;
        j["GLSLSource"] = shader.mGLSLSource;
        j["SPIRVFilePath"] = shader.mSPIRVFilePath;

        // TODO: binary data serialization
        j["SPIRVCode"] = shader.mSPIRVCode;
    }
    static void from_json(const json &j, Shader &shader)
    {
        j.get_to<Asset>(shader);
        shader.mGLSLFilePath = j.at("GLSLFilePath").get<std::filesystem::path>();
        shader.mGLSLSource = j.at("GLSLSource").get<std::string>();
        shader.mSPIRVFilePath = j.at("SPIRVFilePath").get<std::filesystem::path>();

        // TODO: binary data deserialization
        shader.mSPIRVCode = j.at("SPIRVCode").get<std::vector<uint32_t>>();
    }
};
} // namespace nlohmann