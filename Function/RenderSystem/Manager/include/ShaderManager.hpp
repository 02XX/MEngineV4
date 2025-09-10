#pragma once
#include "IShaderManager.hpp"
#include "Manager.hpp"
#include <string>

namespace MEngine::Resource
{

class ShaderManager final : public Manager<Shader>, public IShaderManager
{
  private:
    std::unordered_map<std::string, UUID> mNameToID;

  public:
    ~ShaderManager() override = default;
    void Add(std::shared_ptr<Shader> asset) override;
    std::shared_ptr<Shader> GetByName(const std::string &name) const override;
};
} // namespace MEngine::Resource