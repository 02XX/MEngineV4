#pragma once
#include "IGraphicPipelineManager.hpp"
#include "IShaderManager.hpp"
#include "Manager.hpp"
#include "PipelineLayoutManager.hpp"
#include "RenderPassManager.hpp"
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class GraphicPipelineManager final : public Manager<GraphicPipeline>, public virtual IGraphicPipelineManager
{
  private:
    std::unordered_map<std::string, UUID> mNameToID;
    std::shared_ptr<IPipelineLayoutManager> mPipelineLayoutManager{nullptr};
    std::shared_ptr<RenderPassManager> mRenderPassManager{nullptr};
    std::shared_ptr<IShaderManager> mShaderManager{nullptr};

  public:
    GraphicPipelineManager(std::shared_ptr<VulkanContext> vulkanContext,
                           std::shared_ptr<IPipelineLayoutManager> pipelineLayoutManager,
                           std::shared_ptr<RenderPassManager> renderPassManager,
                           std::shared_ptr<IShaderManager> shaderManager);

    ~GraphicPipelineManager() override = default;
    void Add(std::shared_ptr<GraphicPipeline> asset) override;
    std::shared_ptr<GraphicPipeline> GetByName(const std::string &name) const override;
    void Update(const UUID &id, std::shared_ptr<GraphicPipeline> asset) override;
    void CreateGPUResource(std::shared_ptr<GraphicPipeline> asset) override;
};
} // namespace MEngine::Function