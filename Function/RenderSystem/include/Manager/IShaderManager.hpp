#pragma once
#include "IManager.hpp"
#include "Shader.hpp"
#include <vulkan/vulkan.hpp>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class IShaderManager : public virtual IManager<Shader>
{
  public:
    ~IShaderManager() override = default;
    virtual std::shared_ptr<Shader> GetByName(const std::string &name) const = 0;
};
} // namespace MEngine::Function