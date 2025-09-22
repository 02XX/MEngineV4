#include "GraphicPipelineManager.hpp"
#include "GBufferPipelineBuilder.hpp"
#include "GraphicPipelineDirector.hpp"
#include "LightingPipelineBuilder.hpp"
#include "Logger.hpp"

namespace MEngine::Resource
{

void GraphicPipelineManager::CreateDefault()
{
    auto gBufferPipeline = CreateGBufferPipeline();
    auto lightingPipeline = CreateLightingPipeline();
    gBufferPipeline->mID = mDefaultPipelines[GraphicPipelineType::GBuffer];
    lightingPipeline->mID = mDefaultPipelines[GraphicPipelineType::Lighting];
    Add(gBufferPipeline);
    Add(lightingPipeline);
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateGBufferPipeline()
{
    GBufferPipelineBuilder builder(mShaderManager);
    GraphicPipelineDirector director;
    auto pipeline = director.Make(builder);
    if (!pipeline)
    {
        LogError("Failed to create GBuffer pipeline");
        return nullptr;
    }
    LogInfo("GBuffer pipeline created");
    return std::shared_ptr<GraphicPipeline>(std::move(pipeline));
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateLightingPipeline()
{
    LightingPipelineBuilder builder(mShaderManager);
    GraphicPipelineDirector director;
    auto pipeline = director.Make(builder);
    if (!pipeline)
    {
        LogError("Failed to create Lighting pipeline");
        return nullptr;
    }
    LogInfo("Lighting pipeline created");
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