#pragma once
#include "MaterialManager.hpp"
#include "PBRMaterial.hpp"

namespace MEngine::Resource
{
class PBRMaterialManager final : public MaterialManager<PBRMaterial>, public virtual IManager<PBRMaterial>
{
  public:
  public:
    PBRMaterialManager(std::shared_ptr<Texture2DManager> textureManager,
                       std::shared_ptr<GraphicPipelineManager> pipelineManager)
        : MaterialManager<PBRMaterial>(textureManager, pipelineManager)
    {
        CreateDefault();
    }
    ~PBRMaterialManager() override = default;
    std::shared_ptr<PBRMaterial> CreateLightMaterial();
    std::shared_ptr<PBRMaterial> CreateGBufferOpaqueMaterial();
    void CreateDefault() override;
    void UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                   vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) override;
};
} // namespace MEngine::Resource