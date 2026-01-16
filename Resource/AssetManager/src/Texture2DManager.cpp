#include "Texture2DManager.hpp"
#include "Texture.hpp"

namespace MEngine::Resource
{

void Texture2DManager::CreateDefault()
{
    auto whiteTexture = CreateWhiteTexture();
    auto blackTexture = CreateBlackTexture();
    auto magentaTexture = CreateMagentaTexture();
    whiteTexture->SetID(mDefaultTextures[DefaultTextureType::White]);
    blackTexture->SetID(mDefaultTextures[DefaultTextureType::Black]);
    magentaTexture->SetID(mDefaultTextures[DefaultTextureType::Magenta]);
    Add(whiteTexture);
    Add(blackTexture);
    Add(magentaTexture);
}
std::shared_ptr<Texture2D> Texture2DManager::CreateWhiteTexture()
{
    TextureSetting textureSetting{};
    SamplerSetting samplerSetting{};
    textureSetting.extent = vk::Extent3D{1, 1, 1};
    textureSetting.mipLevels = 1;
    textureSetting.format = vk::Format::eR8G8B8A8Unorm;
    textureSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    textureSetting.samples = vk::SampleCountFlagBits::e1;
    auto texture = std::make_shared<Texture2D>("White", textureSetting, samplerSetting);
    texture->SetTextureData({
        Texture2DMipMap{
            .Data = {255, 255, 255, 255}, // White pixel
            .Extent = {1, 1, 1},
        },
    });
    return texture;
}
std::shared_ptr<Texture2D> Texture2DManager::CreateMagentaTexture()
{
    TextureSetting textureSetting{};
    SamplerSetting samplerSetting{};
    textureSetting.extent = vk::Extent3D{1, 1, 1};
    textureSetting.mipLevels = 1;
    textureSetting.format = vk::Format::eR8G8B8A8Unorm;
    textureSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    textureSetting.samples = vk::SampleCountFlagBits::e1;
    auto texture = std::make_shared<Texture2D>("Magenta", textureSetting, samplerSetting);
    texture->SetTextureData({
        Texture2DMipMap{
            .Data = {255, 0, 255, 255}, // Magenta pixel
            .Extent = {1, 1, 1},
        },
    });
    return texture;
}
std::shared_ptr<Texture2D> Texture2DManager::CreateBlackTexture()
{
    TextureSetting textureSetting{};
    SamplerSetting samplerSetting{};
    textureSetting.extent = vk::Extent3D{1, 1, 1};
    textureSetting.mipLevels = 1;
    textureSetting.format = vk::Format::eR8G8B8A8Unorm;
    textureSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    textureSetting.samples = vk::SampleCountFlagBits::e1;
    auto texture = std::make_shared<Texture2D>("Black", textureSetting, samplerSetting);
    texture->SetTextureData({
        Texture2DMipMap{
            .Data = {0, 0, 0, 255}, // Black pixel
            .Extent = {1, 1, 1},
        },
    });
    return texture;
}
std::shared_ptr<Texture2D> Texture2DManager::GetTexture2D(DefaultTextureType type) const
{
    if (mDefaultTextures.contains(type))
    {
        return Get(mDefaultTextures.at(type));
    }
    LogError("Default texture type {} not found", static_cast<int>(type));
    return nullptr;
}
} // namespace MEngine::Resource
