#pragma once
#include "Asset.hpp"
#include "ShaderResource.hpp"
#include <filesystem>
#include <memory>
#include <string>
using namespace MEngine::Core;
namespace MEngine::Resource
{
class Shader final : public Asset
{
    friend class ShaderResource;

  public:
    std::vector<uint32_t> mSPIRVCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};
    // std::string mEntryPoint{"main"};

  public:
    Shader(const std::string &name, const std::vector<uint32_t> &spirvCode, vk::ShaderStageFlagBits stage)
        : Asset(name), mSPIRVCode(spirvCode), mStage(stage)
    {
        mAssetType = AssetType::Shader;
        mResource = std::make_unique<ShaderResource>(this);
    }

    inline vk::ShaderStageFlagBits GetShaderStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Resource