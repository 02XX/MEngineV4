#pragma once
#include "Asset.hpp"
#include <filesystem>
#include <vulkan/vulkan.hpp>
using namespace MEngine::Core;
namespace MEngine::Function
{
class ShaderBuilder;
class ShaderManager;
} // namespace MEngine::Function
namespace MEngine::Resource
{
class Shader final : public Asset
{
    friend class Function::ShaderBuilder;
    friend class Function::ShaderManager;
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

namespace nlohmann
{
using namespace MEngine::Resource;
template <> struct adl_serializer<Shader>
{
    static void to_json(json &j, const Shader &p)
    {
        j = static_cast<const Asset &>(p);
        j["Stage"] = static_cast<uint32_t>(p.mStage);
        j["GLSLFilePath"] = p.mGLSLFilePath.string();
        j["GLSLSource"] = p.mGLSLSource;
        j["SPIRVFilePath"] = p.mSPIRVFilePath.string();
        j["SPIRVCode"] = p.mSPIRVCode;
    };
    static void from_json(const json &j, Shader &p)
    {
        j.get_to<Asset>(p);
        p.mStage = static_cast<vk::ShaderStageFlagBits>(
            j.value("Stage", static_cast<uint32_t>(vk::ShaderStageFlagBits::eVertex)));
        p.mGLSLFilePath = j.value("GLSLFilePath", "");
        p.mGLSLSource = j.value("GLSLSource", "");
        p.mSPIRVFilePath = j.value("SPIRVFilePath", "");
        p.mSPIRVCode = j.value("SPIRVCode", std::vector<uint32_t>{});
    }
};
} // namespace nlohmann