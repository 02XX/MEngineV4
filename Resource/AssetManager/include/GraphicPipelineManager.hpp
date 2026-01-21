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
    static constexpr const char *ForwardOpaquePhong = "Forward_Opaque_Phong";
    static constexpr const char *ForwardTransparentPhong = "Forward_Transparent_Phong";
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
        {DefaultGraphicPipelineType::ForwardOpaquePhong, Core::UUID{"20000000-0000-0000-0000-000000000000"}},
        {DefaultGraphicPipelineType::ForwardTransparentPhong, Core::UUID{"20000000-0000-0000-0000-000000000001"}},
        {DefaultGraphicPipelineType::ForwardOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000002"}},
        {DefaultGraphicPipelineType::GBufferOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000003"}},
        {DefaultGraphicPipelineType::LightingOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000004"}},
        {DefaultGraphicPipelineType::Skybox, Core::UUID{"20000000-0000-0000-0000-000000000005"}},
        {DefaultGraphicPipelineType::PostProcess, Core::UUID{"20000000-0000-0000-0000-000000000006"}},
        {DefaultGraphicPipelineType::UI, Core::UUID{"20000000-0000-0000-0000-000000000007"}},
    };

  public:
    GraphicPipelineManager(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : PipelineManager<GraphicPipeline>(shaderManager), mContext(context)
    {
        CreateDefault();
    }
    ~GraphicPipelineManager() override = default;
    void CreateDefault() override;
    std::shared_ptr<GraphicPipeline> CreateForwardOpaquePhong();
};
} // namespace MEngine::Resource