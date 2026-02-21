#include "ShaderManager.hpp"
#include "AssetURL.hpp"
#include "Logger.hpp"
#include "Manager.hpp"
#include "RflEntity.hpp"
#include "Serialization.hpp"
#include "ShaderResource.hpp"
#include <filesystem>
#include <fstream>
#include <rfl/flexbuf/read.hpp>
#include <rfl/flexbuf/write.hpp>

using namespace MEngine::Core;
namespace MEngine::Resource
{
ShaderManager::ShaderManager(std::shared_ptr<Context> context) : Manager(context)
{
    InitializeSlang();
    auto forwardOpaquePhongVertShaderURL = AssetURL("shader://ForwardOpaquePhong.slang");
    auto forwardOpaquePhongFragShaderURL = AssetURL("shader://ForwardOpaquePhong.slang");
    auto frowardOpaquePhongVertShader = CreateShader(DefaultShaderType::ForwardOpaquePhongVert,
                                                     forwardOpaquePhongVertShaderURL, ShaderEntryPoint::Vertex);
    auto frowardOpaquePhongFragShader = CreateShader(DefaultShaderType::ForwardOpaquePhongFrag,
                                                     forwardOpaquePhongFragShaderURL, ShaderEntryPoint::Fragment);

    frowardOpaquePhongVertShader->SetID(sDefaultShaders.at(DefaultShaderType::ForwardOpaquePhongVert));
    frowardOpaquePhongFragShader->SetID(sDefaultShaders.at(DefaultShaderType::ForwardOpaquePhongFrag));

    auto forwardOpaquePhongVertShaderAssetURL = AssetURL("shader://" + std::string(DefaultShaderType::ForwardOpaquePhongVert));
    if (!std::filesystem::exists(forwardOpaquePhongVertShaderURL.GetPath()))
    {
        Save(frowardOpaquePhongVertShader, forwardOpaquePhongVertShaderURL);
    }
    if (!std::filesystem::exists(forwardOpaquePhongFragShaderURL.GetPath()))
    {
        Save(frowardOpaquePhongFragShader, forwardOpaquePhongFragShaderURL);
    }
}
void ShaderManager::InitializeSlang()
{
    SlangGlobalSessionDesc desc = {};
    desc.structureSize = sizeof(SlangGlobalSessionDesc);
    SlangResult result = slang_createGlobalSession2(&desc, mSlangGlobalSession.writeRef());
    if (SLANG_FAILED(result))
    {
        throw std::runtime_error("Failed to create Slang global session");
    }

    slang::TargetDesc targetDesc;
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = mSlangGlobalSession->findProfile("spirv_latest");
    slang::SessionDesc sessionDesc;
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;
    std::string basePathStr = AssetURL::GetBasePath(Protocol::SHADER).string();
    const char *searchPath = basePathStr.c_str();
    sessionDesc.searchPaths = &searchPath;
    sessionDesc.searchPathCount = 1;
    result = mSlangGlobalSession->createSession(sessionDesc, mSlangSession.writeRef());
    if (SLANG_FAILED(result))
    {
        throw std::runtime_error("Failed to create Slang session");
    }
}
std::vector<uint32_t> ShaderManager::CompileSlangToSPIRV(const AssetURL &url, const std::string &entryPointName)
{
    using namespace slang;
    ComPtr<IBlob> diagnosticBlob{};
    std::string shaderPathStr = url.GetPath().string();
    const char *shaderPath = shaderPathStr.c_str();
    IModule *module = mSlangSession->loadModule(shaderPath, diagnosticBlob.writeRef());
    if (diagnosticBlob)
    {
        LogError("Shader compilation diagnostics for {}: \n{}", url.GetPath().string(),
                 static_cast<const char *>(diagnosticBlob->getBufferPointer()));
    }
    if (!module)
    {
        LogError("Failed to load module: {}", url.GetPath().string());
        return {};
    }
    ComPtr<IEntryPoint> entryPoint{};
    module->findEntryPointByName(entryPointName.c_str(), entryPoint.writeRef());
    if (!entryPoint)
    {
        LogError("Failed to find entry point '{}' in module: {}", entryPointName, url.GetPath().string());
        LogError("Diagnostics: \n{}", static_cast<const char *>(diagnosticBlob->getBufferPointer()));
        return {};
    }
    IComponentType *components[] = {module, entryPoint};
    ComPtr<IComponentType> composedProgram{};
    SlangResult result = mSlangSession->createCompositeComponentType(components, 2, composedProgram.writeRef(),
                                                                     diagnosticBlob.writeRef());
    if (SLANG_FAILED(result) || !composedProgram)
    {
        LogError("Failed to create composite component type for module: {}", url.GetPath().string());
        LogError("Diagnostics: \n{}", static_cast<const char *>(diagnosticBlob->getBufferPointer()));
        return {};
    }
    ComPtr<IBlob> spirvBlob{};
    result = composedProgram->getEntryPointCode(0, // 入口点索引，我们只传了一个入口点，所以是 0
                                                0, // 目标索引，我们只配置了一个目标 (SPIR-V)，所以是 0
                                                spirvBlob.writeRef(), diagnosticBlob.writeRef());
    if (SLANG_FAILED(result) || !spirvBlob)
    {
        LogError("Failed to get SPIR-V code for module: {}", url.GetPath().string());
        LogError("Diagnostics: \n{}", static_cast<const char *>(diagnosticBlob->getBufferPointer()));
        return {};
    }
    const uint32_t *codePtr = (const uint32_t *)spirvBlob->getBufferPointer();
    size_t codeSize = spirvBlob->getBufferSize() / sizeof(uint32_t);
    return std::vector<uint32_t>(codePtr, codePtr + codeSize);
}
std::vector<uint32_t> ShaderManager::ReadSpirvFile(const std::filesystem::path &path)
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
std::shared_ptr<Shader> ShaderManager::CreateShader(const std::string &name, const AssetURL &path,
                                                    const std::string &entryPointName, bool writeSpirvFile)
{
    if (!std::filesystem::exists(path.GetPath()))
    {
        LogError("Shader file does not exist: {}", path.GetPath().string());
        return nullptr;
    }
    auto spirvPath = path.GetPath();
    auto extension = spirvPath.extension().string();
    spirvPath.replace_extension(extension + ".spv");
    std::vector<uint32_t> spirvCode{};
    if (!mAlwaysCompile && std::filesystem::exists(spirvPath))
    {
        spirvCode = ReadSpirvFile(spirvPath);
    }
    else
    {
        // 编译着色器
        LogDebug("Compiling shader: {}", path.GetPath().string());
        spirvCode = CompileSlangToSPIRV(path, entryPointName); // 编译并写入.spv文件
    }
    auto shader = std::make_shared<Shader>(name, spirvCode, GetShaderStageFromEntryPoint(entryPointName));
    if (writeSpirvFile)
    {
        std::ofstream spirvFile(spirvPath, std::ios::binary);
        spirvFile.write(reinterpret_cast<const char *>(spirvCode.data()), spirvCode.size() * sizeof(uint32_t));
        spirvFile.close();
        LogInfo("Wrote SPIR-V file: {}", spirvPath.string());
    }
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
vk::ShaderStageFlagBits ShaderManager::GetShaderStageFromEntryPoint(const std::string &entryPointName)
{
    if (entryPointName == ShaderEntryPoint::Vertex)
    {
        return vk::ShaderStageFlagBits::eVertex;
    }
    else if (entryPointName == ShaderEntryPoint::Fragment)
    {
        return vk::ShaderStageFlagBits::eFragment;
    }
    else if (entryPointName == ShaderEntryPoint::Main)
    {
        // 无法确定具体阶段，返回一个通用值
        return vk::ShaderStageFlagBits::eAll;
    }
    else
    {
        LogError("Unsupported shader entry point name: {}", entryPointName);
        return vk::ShaderStageFlagBits::eAll; // 返回一个无效的值
    }
}
std::shared_ptr<Asset> ShaderManager::Load(const AssetURL &url)
{
    return LoadHelper<Shader, ShaderEntity>(url, [this](const ShaderEntity &entity) {
        auto shader = std::make_shared<Shader>(entity.assetEntity.value_.name, entity.spirvCode, entity.stage);
        shader->mID = entity.assetEntity.value_.id;
        return shader;
    });
}
void ShaderManager::Save(std::shared_ptr<Asset> asset, const AssetURL &url)
{
    SaveHelper<Shader, ShaderEntity>(
        std::dynamic_pointer_cast<Shader>(asset),
        [](std::shared_ptr<Shader> shader) {
            ShaderEntity entity{
                .assetEntity =
                    AssetEntity{
                        .id = shader->mID,
                        .name = shader->mName,
                    },
                .stage = shader->mStage,
                .spirvCode = shader->mSPIRVCode,
            };
            return entity;
        },
        url);
}
void ShaderManager::Import(const AssetURL &url)
{
    auto path = url.GetPath();
    auto extension = path.extension().string();
    if (extension == ".slang")
    {
        auto vertShader = CreateShader(path.stem().string(), url, ShaderEntryPoint::Vertex);
        auto fragShader = CreateShader(path.stem().string(), url, ShaderEntryPoint::Fragment);
        Add(vertShader);
        Add(fragShader);
        Save(vertShader, AssetURL("shader://" + path.stem().string() + "_vert.shader"));
        Save(fragShader, AssetURL("shader://" + path.stem().string() + "_frag.shader"));
    }
    else
    {
        LogError("Unsupported shader extension for import: {}", extension);
    }
}
void ShaderManager::Export(std::shared_ptr<Asset> asset, const AssetURL &url)
{
}
} // namespace MEngine::Resource
