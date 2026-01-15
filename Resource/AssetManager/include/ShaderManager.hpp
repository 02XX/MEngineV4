#pragma once
#include "AssetURL.hpp"
#include "Manager.hpp"
#include "Shader.hpp"
#include <memory>

namespace MEngine::Resource
{
class ShaderManager final : public Manager<Shader>, public virtual IManager<Shader>
{
  private:
    std::unordered_map<std::string, Core::UUID> mDefaultShaders{
        {"gbuffer_vert", Core::UUID{"00000000-0000-0000-0000-000000000000"}},
        {"gbuffer_frag", Core::UUID{"00000000-0000-0000-0000-000000000001"}},
        {"lighting_vert", Core::UUID{"00000000-0000-0000-0000-000000000002"}},
        {"lighting_frag", Core::UUID{"00000000-0000-0000-0000-000000000003"}},
        {"skybox_vert", Core::UUID{"00000000-0000-0000-0000-000000000004"}},
        {"skybox_frag", Core::UUID{"00000000-0000-0000-0000-000000000005"}},
        {"postprocess_vert", Core::UUID{"00000000-0000-0000-0000-000000000006"}},
        {"postprocess_frag", Core::UUID{"00000000-0000-0000-0000-000000000007"}},
        {"ui_vert", Core::UUID{"00000000-0000-0000-0000-000000000008"}},
        {"ui_frag", Core::UUID{"10000000-0000-1000-8000-100000001234"}},
    };

  public:
    ShaderManager()
    {
        CreateDefault();
    }
    ~ShaderManager() override = default;
    void CreateDefault() override;

  private:
    std::shared_ptr<Shader> CreateShader(const std::string &name, const AssetURL &path);
    vk::ShaderStageFlagBits GetShaderStageFromExtension(const std::string &extension);
};
} // namespace MEngine::Resource