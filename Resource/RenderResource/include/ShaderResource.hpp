#pragma once
#include "RenderResource.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Shader;
class ShaderResource final : public RenderResource
{
  protected:
    std::vector<uint32_t> mCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};
    vk::ShaderModule mShaderModule{nullptr};

  public:
    ShaderResource(std::vector<uint32_t> code, vk::ShaderStageFlagBits stage)
        : RenderResource(), mCode(std::move(code)), mStage(stage)
    {
    }
    ~ShaderResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    inline const vk::ShaderModule GetShaderModule() const
    {
        return mShaderModule;
    }
    inline const vk::ShaderStageFlagBits GetShaderStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Resource
