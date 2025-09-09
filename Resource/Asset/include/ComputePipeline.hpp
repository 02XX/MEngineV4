#pragma once
#include "Pipeline.hpp"
using namespace MEngine::Core;
namespace MEngine::Resource
{
class ComputePipeline final : public Pipeline
{
  protected:
    ComputePipeline() : Pipeline()
    {
    }

  public:
    ~ComputePipeline() override = default;
};
} // namespace MEngine::Resource