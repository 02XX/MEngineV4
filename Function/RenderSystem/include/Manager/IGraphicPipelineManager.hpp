#pragma once
#include "GraphicPipeline.hpp"
#include "IManager.hpp"
#include <vulkan/vulkan.hpp>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class IGraphicPipelineManager : public virtual IManager<GraphicPipeline>
{
  public:
    ~IGraphicPipelineManager() override = default;
    virtual std::shared_ptr<GraphicPipeline> GetByName(const std::string &name) const = 0;
};
} // namespace MEngine::Function