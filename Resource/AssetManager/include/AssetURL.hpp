#pragma once
#include "Asset.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace MEngine::Resource
{
enum class Protocol
{
    ASSET,
    SHADER,
    TEXTURE,
    HTTP,
    HTTPS,
    FILE,
    INVALID
};
class AssetURL final
{
  private:
    Protocol mProtocol{Protocol::INVALID};
    std::filesystem::path mPath;
    static std::unordered_map<Protocol, std::filesystem::path> mProtocolToBasePath;

  public:
    ~AssetURL() = default;
    AssetURL(const std::string &url);
    Protocol GetProtocol() const;
    const std::filesystem::path &GetPath() const;
    static const std::filesystem::path &GetBasePath(Protocol protocol);

  private:
    void Parse(const std::string &url);
};
} // namespace MEngine::Resource