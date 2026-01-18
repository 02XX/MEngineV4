#pragma once
#include "Texture2D.hpp"
#include "UpdateManager.hpp"
#include <memory>
#include <set>
#include <unordered_map>

namespace MEngine::Resource
{
enum class DefaultTextureType
{
    Magenta,
    White,
    Black
};
class Texture2DManager final : public UpdateManager<Texture2D>, public virtual IManager<Texture2D>
{
  private:
    std::unordered_map<DefaultTextureType, Core::UUID> mDefaultTextures{
        {DefaultTextureType::Magenta, Core::UUID{"00000000-0000-0000-0000-000000000000"}},
        {DefaultTextureType::White, Core::UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultTextureType::Black, Core::UUID{"00000000-0000-0000-0000-000000000002"}},
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
    void CreateDefault() override;
    std::shared_ptr<Texture2D> GetTexture2D(DefaultTextureType type) const;
    void UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                   vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) override;
};

} // namespace MEngine::Resource