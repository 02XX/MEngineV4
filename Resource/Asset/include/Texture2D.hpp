#pragma once
#include "Texture.hpp"
#include "Texture2DResource.hpp"
#include <cstdint>
#include <memory>
#include <vector>

using namespace MEngine::Core;
namespace MEngine::Function
{
class Texture2DBuilder;
}
namespace MEngine::Resource
{

class Texture2D : public Texture
{
    friend class Texture2DResource;

  private:
    std::vector<Texture2DMipMap> mTextureData{};

  protected:
    Texture2D() : Texture()
    {
    }

  public:
    Texture2D(const std::string &name, TextureSetting importSetting, SamplerSetting samplerSetting)
        : Texture(name, importSetting, samplerSetting)
    {
        mResource = std::make_unique<Texture2DResource>(importSetting, samplerSetting, &mTextureData);
    }
    ~Texture2D() override = default;
    inline void SetTextureData(std::vector<Texture2DMipMap> data)
    {
        mTextureData = std::move(data);
    }
    inline const std::vector<Texture2DMipMap> &GetTextureData() const
    {
        return mTextureData;
    }
};
} // namespace MEngine::Resource
