#pragma once
#include "Texture.hpp"
#include "Texture2DResource.hpp"
#include <cstdint>
#include <memory>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>
#include <vector>

using namespace MEngine::Core;
namespace MEngine::Function
{
class Texture2DBuilder;
}
namespace MEngine::Resource
{
struct Texture2DMipMap
{
    std::vector<uint8_t> Data{};
    uint32_t SizeX{0};
    uint32_t SizeY{0};
    uint32_t SizeZ{0};
};
class Texture2D : public Texture
{
    friend class Texture2DResource;
    friend class Texture2DManager;
    friend class nlohmann::adl_serializer<Texture2D>;
    FRIEND_TEST(AssetManagerTest, SaveAndLoadTexture2D);

  private:
    std::vector<Texture2DMipMap> mTextureData{};

  protected:
    Texture2D() : Texture()
    {
        mResource = std::make_unique<Texture2DResource>(this);
    }

  public:
    Texture2D(const std::string &name, const TextureSetting &importSetting, const SamplerSetting &samplerSetting)
        : Texture(name, importSetting, samplerSetting)
    {
        mResource = std::make_unique<Texture2DResource>(this);
    }
    ~Texture2D() override = default;
    void SetTextureData(const std::vector<Texture2DMipMap> &data)
    {
        mTextureData = data;
    }
    inline const std::vector<Texture2DMipMap> &GetTextureData() const
    {
        return mTextureData;
    }
};
} // namespace MEngine::Resource

namespace nlohmann
{
using namespace MEngine::Resource;
template <> struct adl_serializer<Texture2DMipMap>
{
    static void to_json(json &j, const Texture2DMipMap &p)
    {
        j["SizeX"] = p.SizeX;
        j["SizeY"] = p.SizeY;
        j["SizeZ"] = p.SizeZ;
        j["Data"] = p.Data;
    };
    static void from_json(const json &j, Texture2DMipMap &p)
    {
        p.SizeX = j.value("SizeX", 0);
        p.SizeY = j.value("SizeY", 0);
        p.SizeZ = j.value("SizeZ", 0);
        p.Data = j.value("Data", std::vector<uint8_t>{});
    }
};
template <> struct adl_serializer<Texture2D>
{
    static void to_json(json &j, const Texture2D &p)
    {
        j = static_cast<const Texture &>(p);
        j["PlatformData"] = p.mTextureData;
    };
    static void from_json(const json &j, Texture2D &p)
    {
        j.get_to<Texture>(p);
        p.mTextureData = j.value("PlatformData", std::vector<Texture2DMipMap>{});
    }
};
} // namespace nlohmann