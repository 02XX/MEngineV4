#pragma once
#include "GraphicPipeline.hpp"
#include "GraphicPipelineManager.hpp"
#include "Manager.hpp"
#include "Material.hpp"
#include "Texture2DManager.hpp"
#include <memory>

namespace MEngine::Resource
{

template <std::derived_from<Material> TAsset>
class MaterialManager : public UpdateManager<TAsset>, public virtual IManager<TAsset>
{
  protected:
    std::shared_ptr<Texture2DManager> mTextureManager;
    std::shared_ptr<GraphicPipelineManager> mPipelineManager;

  public:
    MaterialManager(std::shared_ptr<Texture2DManager> textureManager,
                    std::shared_ptr<GraphicPipelineManager> pipelineManager)
        : mTextureManager(textureManager), mPipelineManager(pipelineManager)
    {
    }
    ~MaterialManager() override = default;
};
} // namespace MEngine::Resource