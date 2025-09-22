#pragma once
#include "GraphicPipeline.hpp"
#include "IBuilder.hpp"
#include "IGraphicPipelineBuilder.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Resource
{
class ICustomGraphicPipelineBuilder : public virtual IGraphicPipelineBuilder
{
  public:
    ~ICustomGraphicPipelineBuilder() override = default;
    virtual void SetCustomShader(vk::ShaderStageFlagBits stage, const std::string &shaderName) = 0;
};
} // namespace MEngine::Resource