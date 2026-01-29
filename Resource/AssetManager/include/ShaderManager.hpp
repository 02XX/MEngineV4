#pragma once
#include "RflEntity.hpp"

#include "AssetURL.hpp"
#include "Manager.hpp"
#include "PendingResourceManager.hpp"
#include "Shader.hpp"
#include "ShaderResource.hpp"
#include <memory>
#include <slang-com-ptr.h>
#include <slang.h>

using namespace Slang;
namespace MEngine::Resource
{
struct DefaultShaderType
{
    static constexpr const char *ForwardOpaquePhongVert = "forward_opaque_phong_vert";
    static constexpr const char *ForwardOpaquePhongFrag = "forward_opaque_phong_frag";
    static constexpr const char *ForwardTransparentPhongVert = "forward_transparent_phong_vert";
    static constexpr const char *ForwardTransparentPhongFrag = "forward_transparent_phong_frag";
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

struct ShaderEntryPoint
{
    static constexpr const char *Main = "main";
    static constexpr const char *Vertex = "vertex";
    static constexpr const char *Fragment = "fragment";
};

class ShaderManager final : public Manager, PendingResourceManager<ShaderResource>
{
  private:
    static inline const std::unordered_map<std::string, Core::UUID> sDefaultShaders{
        {DefaultShaderType::ForwardOpaquePhongVert, Core::UUID{"00000000-0000-0000-0000-000000000000"}},
        {DefaultShaderType::ForwardOpaquePhongFrag, Core::UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultShaderType::ForwardTransparentPhongVert, Core::UUID{"00000000-0000-0000-0000-000000000002"}},
        {DefaultShaderType::ForwardTransparentPhongFrag, Core::UUID{"00000000-0000-0000-0000-000000000003"}},
        {DefaultShaderType::ForwardOpaquePBRVert, Core::UUID{"00000000-0000-0000-0000-000000000004"}},
        {DefaultShaderType::ForwardOpaquePBRFrag, Core::UUID{"00000000-0000-0000-0000-000000000005"}},
        {DefaultShaderType::GBufferOpaquePBRVert, Core::UUID{"00000000-0000-0000-0000-000000000006"}},
        {DefaultShaderType::GBufferOpaquePBRFrag, Core::UUID{"00000000-0000-0000-0000-000000000007"}},
        {DefaultShaderType::LightingOpaquePBRVert, Core::UUID{"00000000-0000-0000-0000-000000000008"}},
        {DefaultShaderType::LightingOpaquePBRFrag, Core::UUID{"00000000-0000-0000-0000-000000000009"}},
        {DefaultShaderType::SkyboxVert, Core::UUID{"00000000-0000-0000-0000-000000000010"}},
        {DefaultShaderType::SkyboxFrag, Core::UUID{"00000000-0000-0000-0000-000000000011"}},
        {DefaultShaderType::PostprocessVert, Core::UUID{"00000000-0000-0000-0000-000000000012"}},
        {DefaultShaderType::PostprocessFrag, Core::UUID{"00000000-0000-0000-0000-000000000013"}},
        {DefaultShaderType::UIVert, Core::UUID{"00000000-0000-0000-0000-000000000014"}},
        {DefaultShaderType::UIFrag, Core::UUID{"00000000-0000-0000-0000-000000000015"}},
    };

    ComPtr<slang::IGlobalSession> mSlangGlobalSession;
    ComPtr<slang::ISession> mSlangSession;

  public:
    bool mAlwaysCompile{true};
    ShaderManager(std::shared_ptr<Context> context);

  private:
    void InitializeSlang();
    std::vector<uint32_t> CompileSlangToSPIRV(const AssetURL &url, const std::string &entryPointName);
    std::vector<uint32_t> ReadSpirvFile(const std::filesystem::path &path);
    std::shared_ptr<Shader> CreateShader(const std::string &name, const AssetURL &path,
                                         const std::string &entryPointName, bool writeSpirvFile = true);
    vk::ShaderStageFlagBits GetShaderStageFromExtension(const std::string &extension);
    vk::ShaderStageFlagBits GetShaderStageFromEntryPoint(const std::string &entryPointName);

  public:
    std::shared_ptr<Asset> Load(const AssetURL &url) override;
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override;
};

} // namespace MEngine::Resource
