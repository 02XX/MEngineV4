
#include "ShaderManager.hpp"
#include "AssetURL.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "ShaderUtils.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
using namespace MEngine::Core;
namespace MEngine::Resource
{
void ShaderManager::CreateDefault()
{
    auto gBufferVertShader = CreateShader("gbuffer_vert", AssetURL("shader://gbuffer.vert"));
    auto gBufferFragShader = CreateShader("gbuffer_frag", AssetURL("shader://gbuffer.frag"));
    auto lightingVertShader = CreateShader("lighting_vert", AssetURL("shader://lighting.vert"));
    auto lightingFragShader = CreateShader("lighting_frag", AssetURL("shader://lighting.frag"));
    // auto skyboxVertShader = CreateShader("skybox_vert", AssetURL("shader://skybox.vert"));
    // auto skyboxFragShader = CreateShader("skybox_frag", AssetURL("shader://skybox.frag"));
    // auto postprocessVertShader = CreateShader("postprocess_vert", AssetURL("shader://postprocess.vert"));
    // auto postprocessFragShader = CreateShader("postprocess_frag", AssetURL("shader://postprocess.frag"));
    // auto uiVertShader = CreateShader("ui_vert", AssetURL("shader://ui.vert"));
    // auto uiFragShader = CreateShader("ui_frag", AssetURL("shader://ui.frag"));
    gBufferVertShader->mID = mDefaultShaders["gbuffer_vert"];
    gBufferFragShader->mID = mDefaultShaders["gbuffer_frag"];
    lightingVertShader->mID = mDefaultShaders["lighting_vert"];
    lightingFragShader->mID = mDefaultShaders["lighting_frag"];
    // skyboxVertShader->mID = mDefaultShaders["skybox_vert"];
    // skyboxFragShader->mID = mDefaultShaders["skybox_frag"];
    // postprocessVertShader->mID = mDefaultShaders["postprocess_vert"];
    // postprocessFragShader->mID = mDefaultShaders["postprocess_frag"];
    // uiVertShader->mID = mDefaultShaders["ui_vert"];
    // uiFragShader->mID = mDefaultShaders["ui_frag"];
    Add(gBufferVertShader);
    Add(gBufferFragShader);
    Add(lightingVertShader);
    Add(lightingFragShader);
    // Add(skyboxVertShader);
    // Add(skyboxFragShader);
    // Add(postprocessVertShader);
    // Add(postprocessFragShader);
    // Add(uiVertShader);
    // Add(uiFragShader);
}
std::shared_ptr<Shader> ShaderManager::CreateShader(const std::string &name, const AssetURL &path)
{
    if (!std::filesystem::exists(path.GetPath()))
    {
        LogError("Shader file does not exist: {}", path.GetPath().string());
        return nullptr;
    }
    auto spirvPath = path.GetPath();
    auto extension = spirvPath.extension().string();
    spirvPath.replace_extension(extension + ".spv");
    std::vector<uint32_t> spirvCode;
    if (!std::filesystem::exists(spirvPath))
    {
        // 编译着色器
        LogInfo("Compiling shader: {}", path.GetPath().string());
        spirvCode = ShaderUtils::CompileShader(path.GetPath(), true); // 编译并写入.spv文件
    }
    else
    {
        // 读取已编译的SPIR-V文件
        LogInfo("Reading SPIR-V file: {}", spirvPath.string());
        spirvCode = ShaderUtils::ReadSpirvFile(spirvPath);
    }
    // 创建Shader对象
    auto shader = std::make_shared<Shader>(name, spirvCode, GetShaderStageFromExtension(extension));
    return shader;
}
vk::ShaderStageFlagBits ShaderManager::GetShaderStageFromExtension(const std::string &extension)
{
    if (extension == ".vert")
    {
        return vk::ShaderStageFlagBits::eVertex;
    }
    else if (extension == ".frag")
    {
        return vk::ShaderStageFlagBits::eFragment;
    }
    else if (extension == ".tesc")
    {
        return vk::ShaderStageFlagBits::eTessellationControl;
    }
    else if (extension == ".tese")
    {
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    }
    else if (extension == ".geom")
    {
        return vk::ShaderStageFlagBits::eGeometry;
    }
    else if (extension == ".comp")
    {
        return vk::ShaderStageFlagBits::eCompute;
    }
    else
    {
        LogError("Unsupported shader extension: {}", extension);
        return vk::ShaderStageFlagBits::eAll; // 返回一个无效的值
    }
}

} // namespace MEngine::Resource