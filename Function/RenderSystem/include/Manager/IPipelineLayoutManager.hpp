#pragma once
#include "IManager.hpp"
#include "PipelineLayout.hpp"
#include "PipelineLayoutType.hpp"
#include <vulkan/vulkan.hpp>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class IPipelineLayoutManager : public virtual IManager<PipelineLayout>
{
  public:
    ~IPipelineLayoutManager() override = default;
    virtual std::shared_ptr<PipelineLayout> GetByType(PipelineLayoutType type) const = 0;
};
} // namespace MEngine::Function