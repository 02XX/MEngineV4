#pragma once
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "Manager.hpp"
#include "PipelineManager.hpp"
#include <memory>
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
  private:
    std::shared_ptr<Context> mContext;

  protected:
    std::unordered_map<GraphicPipelineType, Core::UUID> mDefaultPipelines{
        {GraphicPipelineType::GBuffer, Core::UUID{"00000000-0000-0000-0000-000000000000"}},
        {GraphicPipelineType::Lighting, Core::UUID{"00000000-0000-0000-0000-000000000001"}},
        {GraphicPipelineType::Skybox, Core::UUID{"00000000-0000-0000-0000-000000000002"}},
        {GraphicPipelineType::PostProcess, Core::UUID{"00000000-0000-0000-0000-000000000003"}},
        {GraphicPipelineType::UI, Core::UUID{"00000000-0000-0000-0000-000000000004"}},
    };

  public:
    GraphicPipelineManager(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : PipelineManager<GraphicPipeline>(shaderManager), mContext(context)
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