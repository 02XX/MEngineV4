#include "ShaderUtils.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace MEngine::Core
{

shaderc::Compiler &ShaderUtils::GetCompiler()
{
    static shaderc::Compiler compiler;
    return compiler;
}
shaderc::CompileOptions &ShaderUtils::GetCompileOptions()
{
    static shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);
    options.SetGenerateDebugInfo();
    options.SetHlslFunctionality1(true);
    options.SetHlsl16BitTypes(false);
    options.SetVulkanRulesRelaxed(true);
    options.SetInvertY(false);
    options.SetNanClamp(true);
    return options;
}
std::vector<uint32_t> ShaderUtils::CompileShader(const std::filesystem::path &path, bool writeSpirvFile)
{
    if (!std::filesystem::exists(path))
    {
        LogError("Shader file does not exist: {}", path.string());
        return {};
    }
    std::ifstream file(path);
    if (!file.is_open())
    {
        LogError("Failed to open shader file: {}", path.string());
        return {};
    }
    std::stringstream shaderStream;
    shaderStream << file.rdbuf();
    file.close();
    auto extension = path.extension().string();
    shaderc::Compiler &compiler = GetCompiler();
    shaderc::CompileOptions &options = GetCompileOptions();
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        shaderStream.str(), GetShaderKindFromExtension(extension), path.stem().string().c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        LogError("Shader compilation failed: {}", result.GetErrorMessage());
    }
    std::vector<uint32_t> spirv(result.cbegin(), result.cend());
    // 写入Spirv文件
    if (writeSpirvFile)
    {
        auto spirvPath = path;
        spirvPath.replace_extension(extension + ".spv");
        std::ofstream spirvFile(spirvPath, std::ios::binary);
        if (!spirvFile.is_open())
        {
            LogError("Failed to open SPIR-V file for writing: {}", spirvPath.string());
            return {};
        }
        spirvFile.write(reinterpret_cast<const char *>(spirv.data()), spirv.size() * sizeof(uint32_t));
        spirvFile.close();
    }
    return spirv;
}
std::vector<uint32_t> ShaderUtils::ReadSpirvFile(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path))
    {
        LogError("SPIR-V file does not exist: {}", path.string());
        return {};
    }
    std::ifstream spirvFile(path, std::ios::binary);
    if (!spirvFile.is_open())
    {
        LogError("Failed to open SPIR-V file: {}", path.string());
        return {};
    }
    spirvFile.seekg(0, std::ios::end);
    size_t fileSize = spirvFile.tellg();
    std::vector<char> buffer(fileSize);
    spirvFile.seekg(0, std::ios::beg);
    spirvFile.read((char *)buffer.data(), fileSize);
    spirvFile.close();
    std::vector<uint32_t> spirv(buffer.size() / sizeof(uint32_t));
    std::memcpy(spirv.data(), buffer.data(), buffer.size());
    return spirv;
}
shaderc_shader_kind ShaderUtils::GetShaderKindFromExtension(const std::string &extension)
{
    if (extension == ".vert")
        return shaderc_shader_kind::shaderc_glsl_vertex_shader;
    else if (extension == ".frag")
        return shaderc_shader_kind::shaderc_glsl_fragment_shader;
    else if (extension == ".comp")
        return shaderc_shader_kind::shaderc_glsl_compute_shader;
    else if (extension == ".geom")
        return shaderc_shader_kind::shaderc_glsl_geometry_shader;
    else if (extension == ".tesc")
        return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
    else if (extension == ".tese")
        return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
    else
        throw std::invalid_argument("Unsupported shader extension: " + extension);
}
} // namespace MEngine::Core
