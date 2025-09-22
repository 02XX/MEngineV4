#pragma once
#include "Manager.hpp"
#include "Texture2D.hpp"
#include <memory>
#include <unordered_map>
namespace MEngine::Resource
{
enum class DefaultTextureType
{
    Magenta,
    White,
    Black
};
class Texture2DManager final : public Manager<Texture2D>, public virtual IManager<Texture2D>
{
  private:
    std::unordered_map<DefaultTextureType, UUID> mDefaultTextures{
        {DefaultTextureType::Magenta, UUID{"00000000-0000-0000-0000-000000000000"}},
        {DefaultTextureType::White, UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultTextureType::Black, UUID{"00000000-0000-0000-0000-000000000002"}},
    };

  private:
    std::shared_ptr<Texture2D> CreateWhiteTexture();
    std::shared_ptr<Texture2D> CreateBlackTexture();
    std::shared_ptr<Texture2D> CreateMagentaTexture();

  public:
    Texture2DManager()
    {
        CreateDefault();
    }
    void CreateDefault();
    std::shared_ptr<Texture2D> GetTexture2D(DefaultTextureType type) const;
};

} // namespace MEngine::Resource