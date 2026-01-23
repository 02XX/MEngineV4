#pragma once
#include "GraphicPipeline.hpp"
#include "Manager.hpp"
#include "Material.hpp"
#include "PipelineManager.hpp"
#include "TextureManager.hpp"
#include <memory>

namespace MEngine::Resource
{
struct DefaultMaterialType
{
    static constexpr const char *ForwardOpaquePBR = "MaterialType_Forward_Opaque_PBR";
    static constexpr const char *GBufferOpaquePBR = "MaterialType_GBuffer_Opaque_PBR";
    static constexpr const char *LightingOpaquePBR = "MaterialType_Lighting_Opaque_PBR";
    static constexpr const char *ForwardTransparentPBR = "MaterialType_Forward_Transparent_PBR";
    static constexpr const char *ForwardOpaquePhong = "MaterialType_Forward_Opaque_Phong";
    static constexpr const char *ForwardTransparentPhong = "MaterialType_Forward_Transparent_Phong";
};
class MaterialManager final : public Manager<Material, MaterialResource>
{
  protected:
    std::shared_ptr<TextureManager> mTextureManager;
    std::shared_ptr<PipelineManager> mPipelineManager;

  private:
    static inline const std::unordered_map<std::string, Core::UUID> mDefaultMaterials{
        {DefaultMaterialType::ForwardOpaquePBR, Core::UUID{"10000000-0000-0000-0000-000000000000"}},
        {DefaultMaterialType::GBufferOpaquePBR, Core::UUID{"10000000-0000-0000-0000-000000000001"}},
        {DefaultMaterialType::LightingOpaquePBR, Core::UUID{"10000000-0000-0000-0000-000000000002"}},
        {DefaultMaterialType::ForwardTransparentPBR, Core::UUID{"10000000-0000-0000-0000-000000000003"}},
        {DefaultMaterialType::ForwardOpaquePhong, Core::UUID{"10000000-0000-0000-0000-000000000004"}},
        {DefaultMaterialType::ForwardTransparentPhong, Core::UUID{"10000000-0000-0000-0000-000000000005"}},
    };

  public:
    MaterialManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager,
                    std::shared_ptr<TextureManager> textureManager);
    ~MaterialManager() override = default;
    std::shared_ptr<Asset> Load(const AssetURL &url) override
    {
    }
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override
    {
    }
    void ProcessPendingInitResources(RenderContext renderContext) override;
    void ProcessPendingUpdateResources(RenderContext renderContext) override;
};
} // namespace MEngine::Resource