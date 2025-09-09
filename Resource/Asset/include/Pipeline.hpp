#pragma once
#include "Asset.hpp"
#include "Pipeline.hpp"
#include "PipelineLayout.hpp"
#include "PipelineLayoutType.hpp"
#include "UUID.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>
using namespace MEngine::Core;
namespace MEngine::Resource
{
class Pipeline : public Asset
{
  protected:
    vk::UniquePipeline mPipeline{nullptr};
    PipelineLayoutType mPipelineLayoutType{PipelineLayoutType::None};

  protected:
    Pipeline() : Asset()
    {
    }

  public:
    ~Pipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
};
} // namespace MEngine::Resource