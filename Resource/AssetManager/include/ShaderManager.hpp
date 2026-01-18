#pragma once
#include "AssetURL.hpp"
#include "Manager.hpp"
#include "Shader.hpp"
#include <memory>

namespace MEngine::Resource
{
struct DefaultShaderType
{
    static constexpr const char *ForwardOpaquePBRVert = "forward_opaque_pbr_vert";
    static constexpr const char *ForwardOpaquePBRFrag = "forward_opaque_pbr_frag";
    static constexpr const char *GBufferOpaquePBRVert = "gbuffer_opaque_pbr_vert";
    static constexpr const char *GBufferOpaquePBRFrag = "gbuffer_opaque_pbr_frag";
    static constexpr const char *LightingOpaquePBRVert = "lighting_opaque_pbr_vert";
    static constexpr const char *LightingOpaquePBRFrag = "lighting_opaque_pbr_frag";
    static constexpr const char *SkyboxVert = "skybox_vert";
    static constexpr const char *SkyboxFrag = "skybox_frag";
    static constexpr const char *PostprocessVert = "postprocess_vert";
    static constexpr const char *PostprocessFrag = "postprocess_frag";
    static constexpr const char *UIVert = "ui_vert";
    static constexpr const char *UIFrag = "ui_frag";
};
class ShaderManager final : public Manager<Shader>, public virtual IManager<Shader>
{
  private:
    static inline const std::unordered_map<std::string, Core::UUID> sDefaultShaders{
        {DefaultShaderType::ForwardOpaquePBRVert, Core::UUID{"00000000-0000-0000-0000-000000000000"}},
        {DefaultShaderType::ForwardOpaquePBRFrag, Core::UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultShaderType::LightingOpaquePBRVert, Core::UUID{"00000000-0000-0000-0000-000000000002"}},
        {DefaultShaderType::LightingOpaquePBRFrag, Core::UUID{"00000000-0000-0000-0000-000000000003"}},
        {DefaultShaderType::SkyboxVert, Core::UUID{"00000000-0000-0000-0000-000000000004"}},
        {DefaultShaderType::SkyboxFrag, Core::UUID{"00000000-0000-0000-0000-000000000005"}},
        {DefaultShaderType::GBufferOpaquePBRVert, Core::UUID{"00000000-0000-0000-0000-000000000006"}},
        {DefaultShaderType::GBufferOpaquePBRFrag, Core::UUID{"00000000-0000-0000-0000-000000000007"}},
        {DefaultShaderType::PostprocessVert, Core::UUID{"00000000-0000-0000-0000-000000000008"}},
        {DefaultShaderType::PostprocessFrag, Core::UUID{"00000000-0000-0000-0000-000000000009"}},
        {DefaultShaderType::UIVert, Core::UUID{"00000000-0000-0000-0000-000000000010"}},
        {DefaultShaderType::UIFrag, Core::UUID{"00000000-0000-0000-0000-000000000011"}},
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