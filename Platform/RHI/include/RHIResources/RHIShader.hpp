#pragma once
#include "RHIResource.hpp"
#include <filesystem>
#include <vulkan/vulkan.hpp>

namespace MEngine::Function
{
class ShaderBuilder;
class ShaderManager;
} // namespace MEngine::Function
namespace MEngine::Platform
{
class RHIShader final : public RHIResource
{
    friend class Function::ShaderBuilder;
    friend class Function::ShaderManager;
    friend class AssetManager;
    // friend struct nlohmann::adl_serializer<RHIShader>;
    // FRIEND_TEST(AssetManagerTest, SaveAndLoadShader);

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
    RHIShader() : RHIResource()
    {
    }

  public:
    ~RHIShader() override = default;
    inline vk::ShaderModule GetShaderModule() const
    {
        return mShaderModule.get();
    }
    inline vk::ShaderStageFlagBits GetStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Platform