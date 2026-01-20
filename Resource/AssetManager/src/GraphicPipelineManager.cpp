#include "GraphicPipelineManager.hpp"
#include "GraphicPipelineBuilderForwardOpaquePBR.hpp"
#include "GraphicPipelineBuilderForwardOpaquePhong.hpp"
#include "GraphicPipelineBuilderGBufferOpaquePBR.hpp"
#include "GraphicPipelineBuilderLightingOpaquePBR.hpp"
#include "GraphicPipelineDirector.hpp"
#include "Logger.hpp"
#include <memory>

namespace MEngine::Resource
{

void GraphicPipelineManager::CreateDefault()
{
    auto forwardOpaquePhong = CreateForwardOpaquePhong();
    auto forwardOpaquePBR = CreateForwardOpaquePBR();
    // auto gBufferOpaquePBR = CreateGBufferOpaquePBR();
    // auto lightingOpaquePBR = CreateLightingOpaquePBR();
    forwardOpaquePhong->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::ForwardOpaquePhong));
    forwardOpaquePBR->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::ForwardOpaquePBR));
    // gBufferOpaquePBR->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::GBufferOpaquePBR));
    // lightingOpaquePBR->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::LightingOpaquePBR));
    Add(forwardOpaquePhong);
    Add(forwardOpaquePBR);
    // Add(gBufferOpaquePBR);
    // Add(lightingOpaquePBR);
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateForwardOpaquePhong()
{
    GraphicPipelineBuilderForwardOpaquePhong builder(mContext, mShaderManager);
    GraphicPipelineDirector director;
    auto pipeline = director.Make(builder);
    if (!pipeline)
    {
        LogError("Failed to create Forward_Phong pipeline");
        return nullptr;
    }
    LogInfo("Created「Forward_Phong」pipeline");
    return pipeline;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateForwardTransparentPhong()
{
    LogError("Unimplemented CreateForwardTransparentPhong");
    return nullptr;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateForwardOpaquePBR()
{
    GraphicPipelineBuilderForwardOpaquePBR builder(mContext, mShaderManager);
    GraphicPipelineDirector director;
    auto pipeline = director.Make(builder);
    if (!pipeline)
    {
        LogError("Failed to create Forward pipeline");
        return nullptr;
    }
    LogInfo("Created「Forward」pipeline");
    return pipeline;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateGBufferOpaquePBR()
{
    GraphicPipelineBuilderGBufferOpaquePBR builder(mContext, mShaderManager);
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
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateLightingOpaquePBR()
{
    GraphicPipelineBuilderLightingOpaquePBR builder(mContext, mShaderManager);
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
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateSkyBox()
{
    LogError("Unimplemented CreateSkyBox");
    return nullptr;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreatePostProcess()
{
    LogError("Unimplemented CreatePostProcess");
    return nullptr;
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::CreateUI()
{
    LogError("Unimplemented CreateUI");
    return nullptr;
}
} // namespace MEngine::Resource