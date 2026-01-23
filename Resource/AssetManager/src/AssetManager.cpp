#include "AssetManager.hpp"
#include "Asset.hpp"
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "GraphicPipelineResource.hpp"
#include "Logger.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "MeshResource.hpp"
#include "PBRMaterialResource.hpp"
#include "PhongMaterialResource.hpp"
#include "PipelineManager.hpp"
#include "SceneManager.hpp"
#include "Shader.hpp"
#include "ShaderManager.hpp"
#include "ShaderResource.hpp"
#include "TextureManager.hpp"
#include "TextureRenderTargetResource.hpp"
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"

namespace MEngine::Resource
{
AssetManager::~AssetManager()
{
    if (!mIsShutdown)
    {
        LogError("AssetManager was not shutdown before destruction. Calling Shutdown() before destruction.");
    }
}
void AssetManager::Init(std::shared_ptr<Context> context)
{
    auto shaderManager = std::make_shared<ShaderManager>(context);
    auto meshManager = std::make_shared<MeshManager>(context);
    auto pipelineManager = std::make_shared<PipelineManager>(context, shaderManager);
    auto textureManager = std::make_shared<TextureManager>(context, pipelineManager);
    auto materialManager = std::make_shared<MaterialManager>(context, pipelineManager, textureManager);
    auto sceneManager = std::make_shared<SceneManager>(context, pipelineManager, materialManager, meshManager);
    RegisterManager<Shader, ShaderResource>(shaderManager);
    RegisterManager<Mesh, MeshResource>(meshManager);
    RegisterManager<Pipeline, PipelineResource, GraphicPipelineResource>(pipelineManager);
    RegisterManager<Texture, TextureResource, UploadableTextureResource, TextureRenderTargetResource>(textureManager);
    RegisterManager<Material, MaterialResource, PBRMaterialResource, PhongMaterialResource>(materialManager);
    RegisterManager<Scene, SceneResource>(sceneManager);
}
void AssetManager::Shutdown(std::shared_ptr<Context> context)
{
    DestroyAll();
    ProcessPendingDeletionResources(RenderContext{context, vk::CommandBuffer{}});
    mIsShutdown = true;
}
AssetManager &AssetManager::Instance()
{
    static AssetManager instance;
    return instance;
}
void AssetManager::DestroyAll()
{
    for (auto &[type, manager] : mManagers)
    {
        manager->DestroyAll();
    }
}
std::shared_ptr<Asset> AssetManager::Load(const AssetURL &url)
{
    auto assetType = GetAssetTypeFromURL(url);
    if (mManagers.contains(assetType))
    {
        auto manager = mManagers.at(assetType);
        return manager->Load(url);
    }
    else
    {
        LogError("No manager found for asset type {}", assetType.name());
        return nullptr;
    }
}
void AssetManager::Save(std::shared_ptr<Asset> asset, const AssetURL &url)
{
    auto assetType = GetAssetType(asset.get());
    if (mManagers.contains(assetType))
    {
        auto manager = mManagers.at(assetType);
        manager->Save(asset, url);
    }
    else
    {
        LogError("No manager found for asset type {}", assetType.name());
    }
}
void AssetManager::Add(std::shared_ptr<Asset> asset)
{
    auto assetType = GetAssetType(asset.get());
    if (mManagers.contains(assetType))
    {
        auto manager = mManagers.at(assetType);
        manager->Add(asset);
    }
    else
    {
        LogError("No manager found for asset type {}", assetType.name());
    }
}
std::shared_ptr<Asset> AssetManager::Get(const Core::UUID &id) const
{
    for (const auto &[type, manager] : mManagers)
    {
        auto asset = manager->Get(id);
        if (asset)
        {
            return asset;
        }
    }
    LogError("Asset with ID {} not found", id.ToString());
    return nullptr;
}
std::shared_ptr<Asset> AssetManager::GetByName(const std::string &name) const
{
    for (const auto &[type, manager] : mManagers)
    {
        auto asset = manager->GetByName(name);
        if (asset)
        {
            return asset;
        }
    }
    LogError("Asset with name {} not found", name);
    return nullptr;
}
std::vector<std::shared_ptr<Asset>> AssetManager::GetAll() const
{
    std::vector<std::shared_ptr<Asset>> allAssets;
    for (const auto &[type, manager] : mManagers)
    {
        auto assets = manager->GetAll();
        allAssets.insert(allAssets.end(), assets.begin(), assets.end());
    }
    return allAssets;
}
void AssetManager::Remove(const Core::UUID &id)
{
    for (const auto &[type, manager] : mManagers)
    {
        manager->Remove(id);
    }
}
std::type_index AssetManager::GetAssetType(Asset *asset) const
{
    return std::type_index(typeid(*asset));
}
std::type_index AssetManager::GetAssetTypeFromRenderResource(RenderResource *resource) const
{
    auto renderResourceType = std::type_index(typeid(*resource));
    if (mResourceTypeToAssetTypeMap.contains(renderResourceType))
    {
        return mResourceTypeToAssetTypeMap.at(renderResourceType);
    }
    else
    {
        LogError("No asset type found for render resource type {}", renderResourceType.name());
        return std::type_index(typeid(void));
    }
}
std::type_index AssetManager::GetAssetTypeFromURL(const AssetURL &url) const
{
}

void AssetManager::ProcessPendingInitResources(RenderContext renderContext)
{
    for (const auto &[type, manager] : mManagers)
    {
        manager->ProcessPendingInitResources(renderContext);
    }
}

void AssetManager::ProcessPendingUpdateResources(RenderContext renderContext)
{
    for (const auto &[type, manager] : mManagers)
    {
        manager->ProcessPendingUpdateResources(renderContext);
    }
}

void AssetManager::ProcessPendingDeletionResources(RenderContext renderContext)
{
    for (const auto &[type, manager] : mManagers)
    {
        manager->ProcessPendingDeletionResources(renderContext);
    }
}
void AssetManager::ProcessPendingTasks(RenderContext renderContext)
{
    for (const auto &[type, manager] : mManagers)
    {
        manager->ProcessPendingTasks(renderContext);
    }
}
void AssetManager::PendingInit(RenderResource *resource)
{
    auto assetType = GetAssetTypeFromRenderResource(resource);
    if (mManagers.contains(assetType))
    {
        auto manager = mManagers.at(assetType);
        manager->PendingInit(resource);
    }
    else
    {
        LogError("No manager found for asset type {}", assetType.name());
    }
}
void AssetManager::PendingUpdate(RenderResource *resource)
{
    auto assetType = GetAssetTypeFromRenderResource(resource);
    if (mManagers.contains(assetType))
    {
        auto manager = mManagers.at(assetType);
        manager->PendingUpdate(resource);
    }
    else
    {
        LogError("No manager found for asset type {}", assetType.name());
    }
}
void AssetManager::PendingDelete(std::unique_ptr<RenderResource> resource)
{
    auto assetType = GetAssetTypeFromRenderResource(resource.get());
    if (mManagers.contains(assetType))
    {
        auto manager = mManagers.at(assetType);
        manager->PendingDelete(std::move(resource));
    }
    else
    {
        LogError("No manager found for asset type {}", assetType.name());
    }
}
void AssetManager::PendingTask(Task task)
{
    // for (const auto &[type, manager] : mManagers)
    // {
    //     manager->PendingTask(task);
    // }
    throw std::runtime_error("Not Implemented");
}
} // namespace MEngine::Resource