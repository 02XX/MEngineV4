
#include "ShaderManager.hpp"
#include "Logger.hpp"
#include "ShaderUtils.hpp"

using namespace MEngine::Core;
namespace MEngine::Resource
{
void ShaderManager::CreateDefault()
{
    auto frowardOpaquePBRVertShader =
        CreateShader(DefaultShaderType::ForwardOpaquePBRVert, AssetURL("shader://ForwardOpaquePBR.vert"));
    auto frowardOpaquePBRFragShader =
        CreateShader(DefaultShaderType::ForwardOpaquePBRFrag, AssetURL("shader://ForwardOpaquePBR.frag"));
    auto gBufferVertShader =
        CreateShader(DefaultShaderType::GBufferOpaquePBRVert, AssetURL("shader://GBufferOpaquePBR.vert"));
    auto gBufferFragShader =
        CreateShader(DefaultShaderType::GBufferOpaquePBRFrag, AssetURL("shader://GBufferOpaquePBR.frag"));
    auto lightingVertShader =
        CreateShader(DefaultShaderType::LightingOpaquePBRVert, AssetURL("shader://LightingOpaquePBR.vert"));
    auto lightingFragShader =
        CreateShader(DefaultShaderType::LightingOpaquePBRFrag, AssetURL("shader://LightingOpaquePBR.frag"));

    frowardOpaquePBRVertShader->SetID(sDefaultShaders.at(DefaultShaderType::ForwardOpaquePBRVert));
    frowardOpaquePBRFragShader->SetID(sDefaultShaders.at(DefaultShaderType::ForwardOpaquePBRFrag));
    gBufferVertShader->SetID(sDefaultShaders.at(DefaultShaderType::GBufferOpaquePBRVert));
    gBufferFragShader->SetID(sDefaultShaders.at(DefaultShaderType::GBufferOpaquePBRFrag));
    lightingVertShader->SetID(sDefaultShaders.at(DefaultShaderType::LightingOpaquePBRVert));
    lightingFragShader->SetID(sDefaultShaders.at(DefaultShaderType::LightingOpaquePBRFrag));

    Add(frowardOpaquePBRVertShader);
    Add(frowardOpaquePBRFragShader);
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
    // 编译着色器
    LogDebug("Compiling shader: {}", path.GetPath().string());
    spirvCode = ShaderUtils::CompileShader(path.GetPath(), true); // 编译并写入.spv文件
    // if (!std::filesystem::exists(spirvPath))
    // {
    //     // 编译着色器
    //     LogDebug("Compiling shader: {}", path.GetPath().string());
    //     spirvCode = ShaderUtils::CompileShader(path.GetPath(), true); // 编译并写入.spv文件
    // }
    // else
    // {
    //     // 读取已编译的SPIR-V文件
    //     LogDebug("Reading SPIR-V file: {}", spirvPath.string());
    //     spirvCode = ShaderUtils::ReadSpirvFile(spirvPath);
    // }
    // 创建Shader对象
    auto shader = std::make_shared<Shader>(name, spirvCode, GetShaderStageFromExtension(extension));
    LogInfo("Created「{}」shader from「{}」", name, path.GetPath().string());
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