#pragma once
#include "Context.hpp"
#include <memory>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class IBind
{
  public:
    virtual ~IBind() = default;
    virtual void Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout = {},
                      vk::Pipeline pipeline = {}) = 0;
};
} // namespace MEngine::Resource