#include "GraphicPipelineManager.hpp"
#include "GraphicPipelineBuilderGBuffer.hpp"
#include "GraphicPipelineBuilderLighting.hpp"
#include "GraphicPipelineDirector.hpp"
#include "Logger.hpp"
#include <memory>

namespace MEngine::Resource
{

void GraphicPipelineManager::CreateDefault()
{
    auto gBufferPipeline = CreateGBufferPipeline();
    auto lightingPipeline = CreateLightingPipeline();
    gBufferPipeline->SetID(mDefaultPipelines[GraphicPipelineType::GBuffer]);
    lightingPipeline->SetID(mDefaultPipelines[GraphicPipelineType::Lighting]);
    Add(gBufferPipeline);
    Add(lightingPipeline);
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateGBufferPipeline()
{
    GraphicPipelineBuilderGBuffer builder(mContext, mShaderManager);
    GraphicPipelineDirector director;
    auto pipeline = director.Make(builder);
    if (!pipeline)
    {
        LogError("Failed to create GBuffer pipeline");
        return nullptr;
    }
    LogInfo("Created「GBuffer」pipeline");
    return pipeline;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateLightingPipeline()
{
    GraphicPipelineBuilderLighting builder(mContext, mShaderManager);
    GraphicPipelineDirector director;
    auto pipeline = director.Make(builder);
    if (!pipeline)
    {
        LogError("Failed to create Lighting pipeline");
        return nullptr;
    }
    LogInfo("Created 「Lighting」 pipeline");
    return std::shared_ptr<GraphicPipeline>(std::move(pipeline));
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateSkyboxPipeline()
{
    LogError("Unimplemented CreateSkyboxPipeline");
    return nullptr;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreatePostProcessPipeline()
{
    LogError("Unimplemented CreatePostProcessPipeline");
    return nullptr;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateUIPipeline()
{
    LogError("Unimplemented CreateUIPipeline");
    return nullptr;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::GetPipeline(GraphicPipelineType type) const
{
    if (mDefaultPipelines.contains(type))
    {
        return Get(mDefaultPipelines.at(type));
    }
    LogError("Default pipeline type {} not found", static_cast<int>(type));
    return nullptr;
}
} // namespace MEngine::Resource