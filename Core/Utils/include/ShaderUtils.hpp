#pragma once
#include <filesystem>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>

namespace MEngine::Core
{
class ShaderUtils
{
  public:
    static shaderc::Compiler &GetCompiler();
    static shaderc::CompileOptions &GetCompileOptions();
    static std::vector<uint32_t> CompileShader(const std::filesystem::path &path, bool writeSpirvFile = true);
    static shaderc_shader_kind GetShaderKindFromExtension(const std::string &extension);
    static std::vector<uint32_t> ReadSpirvFile(const std::filesystem::path &path);
};
} // namespace MEngine::Core
