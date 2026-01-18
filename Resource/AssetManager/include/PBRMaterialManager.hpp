#pragma once
#include "MaterialManager.hpp"
#include "PBRMaterial.hpp"

namespace MEngine::Resource
{
struct DefaultPBRMaterialType
{
    static constexpr const char *ForwardOpaque = "Forward_Opaque";
    static constexpr const char *GBufferOpaque = "GBuffer_Opaque";
    static constexpr const char *LightingOpaque = "Lighting_Opaque";
    static constexpr const char *ForwardTransparent = "Forward_Transparent";
};
class PBRMaterialManager final : public MaterialManager<PBRMaterial>, public virtual IManager<PBRMaterial>
{
  private:
    static inline const std::unordered_map<std::string, Core::UUID> mDefaultMaterials{
        {DefaultPBRMaterialType::ForwardOpaque, Core::UUID{"10000000-0000-0000-0000-000000000000"}},
        {DefaultPBRMaterialType::GBufferOpaque, Core::UUID{"10000000-0000-0000-0000-000000000001"}},
        {DefaultPBRMaterialType::LightingOpaque, Core::UUID{"10000000-0000-0000-0000-000000000002"}},
        {DefaultPBRMaterialType::ForwardTransparent, Core::UUID{"10000000-0000-0000-0000-000000000003"}},
    };

  public:
    PBRMaterialManager(std::shared_ptr<Texture2DManager> textureManager,
                       std::shared_ptr<GraphicPipelineManager> pipelineManager)
        : MaterialManager<PBRMaterial>(textureManager, pipelineManager)
    {
        CreateDefault();
    }
    ~PBRMaterialManager() override = default;
    std::shared_ptr<PBRMaterial> CreateForwardOpaque();
    std::shared_ptr<PBRMaterial> CreateLightOpaque();
    std::shared_ptr<PBRMaterial> CreateGBufferOpaque();
    void CreateDefault() override;
    void UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                   vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) override;
};
} // namespace MEngine::Resource