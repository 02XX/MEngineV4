#pragma once
#include "IShaderManager.hpp"
#include "Manager.hpp"
#include <string>
using namespace MEngine::Resource;
namespace MEngine::Function
{

class ShaderManager final : public Manager<Shader>, public IShaderManager
{
  private:
    std::unordered_map<std::string, UUID> mNameToID;

  public:
    ShaderManager(std::shared_ptr<VulkanContext> vulkanContext);

    ~ShaderManager() override = default;
    void Add(std::shared_ptr<Shader> asset) override;
    std::shared_ptr<Shader> GetByName(const std::string &name) const override;
    void Update(const UUID &id, std::shared_ptr<Shader> asset) override;
};
} // namespace MEngine::Function