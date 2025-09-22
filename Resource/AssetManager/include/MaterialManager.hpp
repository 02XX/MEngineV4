#pragma once
#include "GraphicPipeline.hpp"
#include "GraphicPipelineManager.hpp"
#include "Manager.hpp"
#include "Material.hpp"
#include "Texture2DManager.hpp"
#include <memory>

namespace MEngine::Resource
{
enum class DefaultMaterialType
{
    GBufferPBROpaque,
    GBufferPBRTransparent,
};
template <std::derived_from<Material> TAsset>
class MaterialManager : public Manager<TAsset>, public virtual IManager<TAsset>
{
  protected:
    std::shared_ptr<Texture2DManager> mTextureManager;
    std::shared_ptr<GraphicPipelineManager> mPipelineManager;
    std::unordered_map<DefaultMaterialType, UUID> mDefaultMaterials{
        {DefaultMaterialType::GBufferPBROpaque, UUID{"00000000-0000-0000-0000-000000000000"}},
        {DefaultMaterialType::GBufferPBRTransparent, UUID{"00000000-0000-0000-0000-000000000001"}},
    };

  public:
    MaterialManager(std::shared_ptr<Texture2DManager> textureManager,
                    std::shared_ptr<GraphicPipelineManager> pipelineManager)
        : mTextureManager(textureManager), mPipelineManager(pipelineManager)
    {
    }
    ~MaterialManager() override = default;
    inline std::shared_ptr<Material> GetMaterial(DefaultMaterialType type) const
    {
        if (mDefaultMaterials.contains(type))
        {
            return this->Get(mDefaultMaterials.at(type));
        }
        LogError("Default material type {} not found", static_cast<int>(type));
        return nullptr;
    }
};
} // namespace MEngine::Resource