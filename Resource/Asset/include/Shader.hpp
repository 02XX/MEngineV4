#pragma once
#include "Asset.hpp"
#include "UUID.hpp"
#include <filesystem>
#include <gtest/gtest_prod.h>
#include <nlohmann/json_fwd.hpp>
#include <vulkan/vulkan.hpp>
using namespace MEngine::Core;
namespace MEngine::Function
{
class ShaderBuilder;
} // namespace MEngine::Function
namespace MEngine::Resource
{
class Shader final : public Asset
{
    friend class Function::ShaderBuilder;
    friend class AssetManager;
    friend struct nlohmann::adl_serializer<Shader>;
    FRIEND_TEST(AssetManagerTest, SaveAndLoadShader);

  private:
    vk::UniqueShaderModule mShaderModule{nullptr};

  protected:
    vk::ShaderModuleCreateInfo mShaderModuleCreateInfo{};
    std::vector<uint32_t> mCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

    std::filesystem::path mGLSLFilePath{};
    std::string mGLSLSource{};
    std::filesystem::path mSPIRVFilePath{};
    std::vector<uint32_t> mSPIRVCode{};

  protected:
    Shader() : Asset()
    {
    }

  public:
    ~Shader() override = default;
    inline vk::ShaderModule GetShaderModule() const
    {
        return mShaderModule.get();
    }
    inline vk::ShaderStageFlagBits GetStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Resource