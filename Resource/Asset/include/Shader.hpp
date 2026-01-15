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
    friend class ShaderResource;

  protected:
    std::vector<uint32_t> mSPIRVCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

  public:
    Shader(const std::string &name, const std::vector<uint32_t> &spirvCode, vk::ShaderStageFlagBits stage)
        : Asset(name), mSPIRVCode(spirvCode), mStage(stage)
    {
        mResource = std::make_unique<ShaderResource>(this);
    }
    ~Shader() override = default;
    inline vk::ShaderStageFlagBits GetShaderStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Resource