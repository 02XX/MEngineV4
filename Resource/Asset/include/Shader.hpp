#pragma once
#include "Asset.hpp"
#include "ShaderResource.hpp"
#include <filesystem>
#include <memory>
using namespace MEngine::Core;
namespace MEngine::Resource
{
class Shader final : public Asset
{
  protected:
    std::vector<uint32_t> SPIRVCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

  public:
    Shader(const std::string &name, const std::vector<uint32_t> &spirvCode, vk::ShaderStageFlagBits stage)
        : Asset(name), SPIRVCode(spirvCode), mStage(stage)
    {
        mResource = std::make_unique<ShaderResource>(SPIRVCode, mStage);
    }
    ~Shader() override = default;
    inline vk::ShaderStageFlagBits GetStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Resource