
#pragma once
#include "TextureResource.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
struct Texture2DMipMap
{
    std::vector<uint8_t> Data{};
    uint32_t SizeX{0};
    uint32_t SizeY{0};
    uint32_t SizeZ{0};
};
class Texture2DResource : public TextureResource
{

  protected:
    std::vector<Texture2DMipMap> mTextureData{};

  public:
    Texture2DResource(vk::ImageCreateInfo imageCreateInfo, vk::SamplerCreateInfo samplerCreateInfo,
                      std::vector<Texture2DMipMap> textureData)
        : TextureResource(imageCreateInfo, samplerCreateInfo), mTextureData(textureData) {};
    ~Texture2DResource() override = default;
    virtual void InitRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource
