#pragma once
#include "Context.hpp"
#include <memory>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
struct BindContext
{
    vk::CommandBuffer CommandBuffer;
    vk::PipelineLayout PipelineLayout;
    vk::Pipeline Pipeline;
};
class IBind
{
  public:
    virtual ~IBind() = default;
    virtual void Bind(BindContext bindContext) = 0;
};
} // namespace MEngine::Resource