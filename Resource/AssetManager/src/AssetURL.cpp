#include "AssetURL.hpp"

namespace MEngine::Resource
{
std::unordered_map<Protocol, std::filesystem::path> AssetURL::mProtocolToBasePath = {
    {Protocol::ASSET, std::filesystem::current_path() / "Assets/"},
    {Protocol::HTTP, ""},
    {Protocol::HTTPS, ""},
    {Protocol::FILE, std::filesystem::current_path() / "Files/"},
    {Protocol::SHADER, std::filesystem::current_path() / "Assets/Shaders/"},
    {Protocol::TEXTURE, std::filesystem::current_path() / "Assets/Textures/"},
    {Protocol::INVALID, std::filesystem::current_path()}};
const std::filesystem::path &AssetURL::GetBasePath(Protocol protocol)
{
    return mProtocolToBasePath.at(protocol);
}
AssetURL::AssetURL(const std::string &url)
{
    Parse(url);
}
Protocol AssetURL::GetProtocol() const
{
    return mProtocol;
}
const std::filesystem::path &AssetURL::GetPath() const
{
    return mPath;
}
// eg. asset://path/to/asset
void AssetURL::Parse(const std::string &url)
{
    auto pos = url.find("://");
    if (pos == std::string::npos)
    {
        mProtocol = Protocol::INVALID;
        mPath = url;
        return;
    }
    auto protocolStr = url.substr(0, pos);
    if (protocolStr == "asset")
    {
        mProtocol = Protocol::ASSET;
    }
    else if (protocolStr == "shader")
    {
        mProtocol = Protocol::SHADER;
    }
    else if (protocolStr == "texture")
    {
        mProtocol = Protocol::TEXTURE;
    }
    else if (protocolStr == "http")
    {
        mProtocol = Protocol::HTTP;
    }
    else if (protocolStr == "https")
    {
        mProtocol = Protocol::HTTPS;
    }
    else if (protocolStr == "file")
    {
        mProtocol = Protocol::FILE;
    }
    else
    {
        mProtocol = Protocol::INVALID;
    }
    mPath = mProtocolToBasePath[mProtocol] / url.substr(pos + 3);
}
} // namespace MEngine::Resource