#pragma once
#include "Manager.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include <memory>

namespace MEngine::Resource
{
template <std::derived_from<Pipeline> TPipeline>
class PipelineManager : public Manager<TPipeline>, public virtual IManager<TPipeline>
{
  protected:
    std::shared_ptr<IManager<Shader>> mShaderManager;

  public:
    PipelineManager(std::shared_ptr<IManager<Shader>> shaderManager) : mShaderManager(shaderManager)
    {
    }
    ~PipelineManager() override = default;
};
} // namespace MEngine::Resource