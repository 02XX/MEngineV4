#pragma once
#include "GraphicPipeline.hpp"
#include "Manager.hpp"
#include "PipelineManager.hpp"
namespace MEngine::Resource
{
enum class GraphicPipelineType
{
    GBuffer,
    Lighting,
    Skybox,
    PostProcess,
    UI
};
class GraphicPipelineManager final : public PipelineManager<GraphicPipeline>
{
  protected:
    std::unordered_map<GraphicPipelineType, UUID> mDefaultPipelines{
        {GraphicPipelineType::GBuffer, UUID{"00000000-0000-0000-0000-000000000000"}},
        {GraphicPipelineType::Lighting, UUID{"00000000-0000-0000-0000-000000000001"}},
        {GraphicPipelineType::Skybox, UUID{"00000000-0000-0000-0000-000000000002"}},
        {GraphicPipelineType::PostProcess, UUID{"00000000-0000-0000-0000-000000000003"}},
        {GraphicPipelineType::UI, UUID{"00000000-0000-0000-0000-000000000004"}},
    };

  public:
    GraphicPipelineManager(std::shared_ptr<IManager<Shader>> shaderManager)
        : PipelineManager<GraphicPipeline>(shaderManager)
    {
        CreateDefault();
    }
    ~GraphicPipelineManager() override = default;
    void CreateDefault() override;
    std::shared_ptr<GraphicPipeline> CreateGBufferPipeline();
    std::shared_ptr<GraphicPipeline> CreateLightingPipeline();
    std::shared_ptr<GraphicPipeline> CreateSkyboxPipeline();
    std::shared_ptr<GraphicPipeline> CreatePostProcessPipeline();
    std::shared_ptr<GraphicPipeline> CreateUIPipeline();
    std::shared_ptr<GraphicPipeline> GetPipeline(GraphicPipelineType type) const;
};
} // namespace MEngine::Resource