#include "GraphicPipelineManager.hpp"
#include "GBufferPipelineBuilder.hpp"
#include "GraphicPipelineDirector.hpp"
#include "LightingPipelineBuilder.hpp"

namespace MEngine::Function
{
GraphicPipelineManager::GraphicPipelineManager(std::shared_ptr<VulkanContext> vulkanContext,
                                               std::shared_ptr<IPipelineLayoutManager> pipelineLayoutManager,
                                               std::shared_ptr<RenderPassManager> renderPassManager,
                                               std::shared_ptr<IShaderManager> shaderManager)
    : Manager<GraphicPipeline>(vulkanContext), mPipelineLayoutManager(pipelineLayoutManager),
      mRenderPassManager(renderPassManager), mShaderManager(shaderManager)
{
    GraphicPipelineDirector director;
    GBufferPipelineBuilder builder(mVulkanContext, mPipelineLayoutManager, mShaderManager, mRenderPassManager);
    Add(director.Make(builder));
    LightingPipelineBuilder lightingBuilder(mVulkanContext, mPipelineLayoutManager, mShaderManager, mRenderPassManager);
    Add(director.Make(lightingBuilder));
}
void GraphicPipelineManager::Add(std::shared_ptr<GraphicPipeline> asset)
{
    Manager<GraphicPipeline>::Add(asset);
    mNameToID[asset->GetName()] = asset->GetID();
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::GetByName(const std::string &name) const
{
    if (mNameToID.contains(name))
    {
        return Get(mNameToID.at(name));
    }
    LogError("Graphic Pipeline with name {} not found", name);
    return nullptr;
}
void GraphicPipelineManager::Update(const UUID &id, std::shared_ptr<GraphicPipeline> asset)
{
}   
} // namespace MEngine::Function