#pragma once
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "Manager.hpp"
#include "PipelineManager.hpp"
#include <memory>
namespace MEngine::Resource
{
struct DefaultGraphicPipelineType
{
    static constexpr const char *ForwardOpaquePBR = "Forward_Opaque_PBR";
    static constexpr const char *GBufferOpaquePBR = "GBuffer_Opaque_PBR";
    static constexpr const char *LightingOpaquePBR = "Lighting_Opaque_PBR";
    static constexpr const char *Skybox = "Skybox";
    static constexpr const char *PostProcess = "PostProcess";
    static constexpr const char *UI = "UI";
};
class GraphicPipelineManager final : public PipelineManager<GraphicPipeline>
{
  private:
    std::shared_ptr<Context> mContext;

  protected:
    static inline const std::unordered_map<std::string, Core::UUID> mDefaultPipelines{
        {DefaultGraphicPipelineType::ForwardOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000000"}},
        {DefaultGraphicPipelineType::GBufferOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000001"}},
        {DefaultGraphicPipelineType::LightingOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000002"}},
        {DefaultGraphicPipelineType::Skybox, Core::UUID{"20000000-0000-0000-0000-000000000003"}},
        {DefaultGraphicPipelineType::PostProcess, Core::UUID{"20000000-0000-0000-0000-000000000004"}},
        {DefaultGraphicPipelineType::UI, Core::UUID{"20000000-0000-0000-0000-000000000005"}},
    };

  public:
    GraphicPipelineManager(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : PipelineManager<GraphicPipeline>(shaderManager), mContext(context)
    {
        CreateDefault();
    }
    ~GraphicPipelineManager() override = default;
    void CreateDefault() override;
    std::shared_ptr<GraphicPipeline> CreateForwardOpaquePBR();
    std::shared_ptr<GraphicPipeline> CreateGBufferOpaquePBR();
    std::shared_ptr<GraphicPipeline> CreateLightingOpaquePBR();
    std::shared_ptr<GraphicPipeline> CreateSkyBox();
    std::shared_ptr<GraphicPipeline> CreatePostProcess();
    std::shared_ptr<GraphicPipeline> CreateUI();
};
} // namespace MEngine::Resource