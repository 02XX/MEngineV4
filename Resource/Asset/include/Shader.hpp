#pragma once
#include "Asset.hpp"
#include "ShaderResource.hpp"
#include <filesystem>
#include <memory>
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
    friend class ShaderBuilder;
    friend class ShaderResource;
    friend class AssetManager;
    friend class ShaderManager;
    friend struct nlohmann::adl_serializer<Shader>;
    FRIEND_TEST(AssetManagerTest, SaveAndLoadShader);

  protected:
    std::vector<uint32_t> SPIRVCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

  protected:
    Shader() : Asset()
    {
    }

  public:
    Shader(const std::string &name, const std::vector<uint32_t> &spirvCode, vk::ShaderStageFlagBits stage)
        : Asset(name), SPIRVCode(spirvCode), mStage(stage)
    {
        mResource = std::make_unique<ShaderResource>(this);
    }
    ~Shader() override = default;
    inline vk::ShaderStageFlagBits GetStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Resource

namespace nlohmann
{
using namespace MEngine::Resource;
// template <> struct adl_serializer<Shader>
// {
//     static void to_json(json &j, const Shader &p)
//     {
//         j = static_cast<const Asset &>(p);
//         j["Stage"] = static_cast<uint32_t>(p.mStage);
//         j["GLSLFilePath"] = p.mGLSLFilePath.string();
//         j["GLSLSource"] = p.mGLSLSource;
//         j["SPIRVFilePath"] = p.mSPIRVFilePath.string();
//         j["SPIRVCode"] = p.mSPIRVCode;
//     };
//     static void from_json(const json &j, Shader &p)
//     {
//         j.get_to<Asset>(p);
//         p.mStage = static_cast<vk::ShaderStageFlagBits>(
//             j.value("Stage", static_cast<uint32_t>(vk::ShaderStageFlagBits::eVertex)));
//         p.mGLSLFilePath = j.value("GLSLFilePath", "");
//         p.mGLSLSource = j.value("GLSLSource", "");
//         p.mSPIRVFilePath = j.value("SPIRVFilePath", "");
//         p.mSPIRVCode = j.value("SPIRVCode", std::vector<uint32_t>{});
//     }
// };
} // namespace nlohmann