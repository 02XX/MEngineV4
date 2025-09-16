#pragma once
#include "Asset.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace MEngine::Resource
{

using TextureImportSetting = vk::ImageCreateInfo;
using SamplerImportSetting = vk::SamplerCreateInfo;
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Engine/UTexture
class Texture : public Asset
{
    friend struct nlohmann::adl_serializer<Texture>;

  private:
  protected:
    TextureImportSetting mTextureImportSettings{};
    SamplerImportSetting mSamplerImportSettings{};

  protected:
    Texture() : Asset()
    {
    }

  public:
    ~Texture() override = default;
};
} // namespace MEngine::Resource

namespace nlohmann
{
using namespace MEngine::Resource;
template <> struct adl_serializer<TextureImportSetting>
{
    static void to_json(json &j, const TextureImportSetting &p)
    {
        j["extent"] = {{"width", p.extent.width}, {"height", p.extent.height}, {"depth", p.extent.depth}};
        j["mipLevels"] = p.mipLevels;
        j["format"] = static_cast<uint32_t>(p.format);
        j["sampleCount"] = static_cast<uint32_t>(p.samples);
        j["imageCreateFlags"] = static_cast<uint32_t>(p.flags);
        j["usage"] = static_cast<uint32_t>(p.usage);
        j["tiling"] = static_cast<uint32_t>(p.tiling);
    }
    static void from_json(const json &j, TextureImportSetting &p)
    {
        auto extentObj = j.value("extent", json::object());
        p.extent.width = extentObj.value("width", 0);
        p.extent.height = extentObj.value("height", 0);
        p.extent.depth = extentObj.value("depth", 1);
        p.mipLevels = j.value("mipLevels", 1);
        p.format = static_cast<vk::Format>(j.value("format", static_cast<uint32_t>(vk::Format::eR8G8B8A8Unorm)));
        p.samples = static_cast<vk::SampleCountFlagBits>(
            j.value("sampleCount", static_cast<uint32_t>(vk::SampleCountFlagBits::e1)));
        p.flags = static_cast<vk::ImageCreateFlags>(j.value("imageCreateFlags", 0));
        p.usage =
            static_cast<vk::ImageUsageFlags>(j.value("usage", static_cast<uint32_t>(vk::ImageUsageFlagBits::eSampled)));
        p.tiling = static_cast<vk::ImageTiling>(j.value("tiling", static_cast<uint32_t>(vk::ImageTiling::eOptimal)));
    }
};
template <> struct adl_serializer<SamplerImportSetting>
{
    static void to_json(json &j, const SamplerImportSetting &p)
    {
        j["magFilter"] = static_cast<uint32_t>(p.magFilter);
        j["minFilter"] = static_cast<uint32_t>(p.minFilter);
        j["mipmapMode"] = static_cast<uint32_t>(p.mipmapMode);
        j["addressModeU"] = static_cast<uint32_t>(p.addressModeU);
        j["addressModeV"] = static_cast<uint32_t>(p.addressModeV);
        j["addressModeW"] = static_cast<uint32_t>(p.addressModeW);
        j["mipLodBias"] = p.mipLodBias;
        j["anisotropyEnable"] = p.anisotropyEnable;
        j["maxAnisotropy"] = p.maxAnisotropy;
        j["compareEnable"] = p.compareEnable;
        j["compareOp"] = static_cast<uint32_t>(p.compareOp);
        j["minLod"] = p.minLod;
        j["maxLod"] = p.maxLod;
        j["borderColor"] = static_cast<uint32_t>(p.borderColor);
        j["unnormalizedCoordinates"] = p.unnormalizedCoordinates;
    }
    static void from_json(const json &j, SamplerImportSetting &p)
    {
        p.magFilter = static_cast<vk::Filter>(j.value("magFilter", static_cast<uint32_t>(vk::Filter::eLinear)));
        p.minFilter = static_cast<vk::Filter>(j.value("minFilter", static_cast<uint32_t>(vk::Filter::eLinear)));
        p.mipmapMode = static_cast<vk::SamplerMipmapMode>(
            j.value("mipmapMode", static_cast<uint32_t>(vk::SamplerMipmapMode::eLinear)));
        p.addressModeU = static_cast<vk::SamplerAddressMode>(
            j.value("addressModeU", static_cast<uint32_t>(vk::SamplerAddressMode::eRepeat)));
        p.addressModeV = static_cast<vk::SamplerAddressMode>(
            j.value("addressModeV", static_cast<uint32_t>(vk::SamplerAddressMode::eRepeat)));
        p.addressModeW = static_cast<vk::SamplerAddressMode>(
            j.value("addressModeW", static_cast<uint32_t>(vk::SamplerAddressMode::eRepeat)));
        p.mipLodBias = j.value("mipLodBias", 0.0f);
        p.anisotropyEnable = j.value("anisotropyEnable", vk::False);
        p.maxAnisotropy = j.value("maxAnisotropy", 1.0f);
        p.compareEnable = j.value("compareEnable", vk::False);
        p.compareOp = static_cast<vk::CompareOp>(j.value("compareOp", static_cast<uint32_t>(vk::CompareOp::eAlways)));
        p.minLod = j.value("minLod", 0.0f);
        p.maxLod = j.value("maxLod", 0.0f);
        p.borderColor = static_cast<vk::BorderColor>(
            j.value("borderColor", static_cast<uint32_t>(vk::BorderColor::eIntOpaqueBlack)));
        p.unnormalizedCoordinates = j.value("unnormalizedCoordinates", vk::False);
    }
};
template <> struct adl_serializer<Texture>
{
    static void to_json(json &j, const Texture &p)
    {
        j = static_cast<const Asset &>(p);
        j["ImportSettings"]["Texture"] = p.mTextureImportSettings;
        j["ImportSettings"]["Sampler"] = p.mSamplerImportSettings;
    };
    static void from_json(const json &j, Texture &p)
    {
        j.get_to<Asset>(p);
        p.mTextureImportSettings = j.value("ImportSettings", json::object()).value("Texture", TextureImportSetting{});
        p.mSamplerImportSettings = j.value("ImportSettings", json::object()).value("Sampler", SamplerImportSetting{});
    }
};
} // namespace nlohmann