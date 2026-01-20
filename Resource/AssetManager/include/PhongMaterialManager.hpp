#pragma once
#include "MaterialManager.hpp"
#include "PhongMaterial.hpp"

namespace MEngine::Resource
{
struct DefaultPhongMaterialType
{
    static constexpr const char *ForwardOpaque = "Forward_Opaque";
    static constexpr const char *ForwardTransparent = "Forward_Transparent";
};
class PhongMaterialManager final : public MaterialManager<PhongMaterial>, public virtual IManager<PhongMaterial>
{
  private:
    static inline const std::unordered_map<std::string, Core::UUID> mDefaultMaterials{
        {DefaultPhongMaterialType::ForwardOpaque, Core::UUID{"10000000-0000-0000-0000-000000000000"}},
        {DefaultPhongMaterialType::ForwardTransparent, Core::UUID{"10000000-0000-0000-0000-000000000001"}},
    };

  public:
    PhongMaterialManager(std::shared_ptr<Texture2DManager> textureManager,
                         std::shared_ptr<GraphicPipelineManager> pipelineManager)
        : MaterialManager<PhongMaterial>(textureManager, pipelineManager)
    {
        CreateDefault();
    }
    ~PhongMaterialManager() override = default;
    std::shared_ptr<PhongMaterial> CreateForwardOpaque();
    std::shared_ptr<PhongMaterial> CreateForwardTransparent();
    void CreateDefault() override;
    void UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                   vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) override;
};
} // namespace MEngine::Resource