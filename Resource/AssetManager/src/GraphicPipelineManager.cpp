#include "GraphicPipelineManager.hpp"
#include "GraphicPipelineBuilderForwardOpaquePhong.hpp"
#include "GraphicPipelineDirector.hpp"
#include "Logger.hpp"
#include <memory>

namespace MEngine::Resource
{

void GraphicPipelineManager::CreateDefault()
{
    auto forwardOpaquePhong = CreateForwardOpaquePhong();
    // auto gBufferOpaquePBR = CreateGBufferOpaquePBR();
    // auto lightingOpaquePBR = CreateLightingOpaquePBR();
    forwardOpaquePhong->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::ForwardOpaquePhong));
    // gBufferOpaquePBR->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::GBufferOpaquePBR));
    // lightingOpaquePBR->SetID(mDefaultPipelines.at(DefaultGraphicPipelineType::LightingOpaquePBR));
    Add(forwardOpaquePhong);
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
} // namespace MEngine::Resource