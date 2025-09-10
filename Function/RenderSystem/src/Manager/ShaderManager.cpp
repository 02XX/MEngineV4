#include "ShaderManager.hpp"
#include "AssetURL.hpp"
#include "ShaderBuilder.hpp"
#include "ShaderDirector.hpp"

namespace MEngine::Function
{
ShaderManager::ShaderManager(std::shared_ptr<VulkanContext> vulkanContext) : Manager<Shader>(vulkanContext)
{
    ShaderDirector director("gbuffer_vert", AssetURL("shader://gbuffer_vert").GetPath(),
                            vk::ShaderStageFlagBits::eVertex);
    ShaderBuilder builder(mVulkanContext);
    Add(director.Make(builder));
    director =
        ShaderDirector("gbuffer_frag", AssetURL("shader://gbuffer_frag").GetPath(), vk::ShaderStageFlagBits::eFragment);
    Add(director.Make(builder));
    director =
        ShaderDirector("lighting_vert", AssetURL("shader://lighting_vert").GetPath(), vk::ShaderStageFlagBits::eVertex);
    Add(director.Make(builder));
    director = ShaderDirector("lighting_frag", AssetURL("shader://lighting_frag").GetPath(),
                              vk::ShaderStageFlagBits::eFragment);
    Add(director.Make(builder));
}
void ShaderManager::Add(std::shared_ptr<Shader> asset)
{
    Manager<Shader>::Add(asset);
    mNameToID[asset->GetName()] = asset->GetID();
}
std::shared_ptr<Shader> ShaderManager::GetByName(const std::string &name) const
{
    if (mNameToID.contains(name))
    {
        return Get(mNameToID.at(name));
    }
    return nullptr;
}
void ShaderManager::Update(const UUID &id, std::shared_ptr<Shader> asset)
{
    if (!mAssets.contains(id))
        return;
    auto shader = mAssets.at(id);
    if (!shader->IsLoaded())
    {
        ShaderBuilder builder(mVulkanContext);
        if (!asset->mGLSLFilePath.empty())
        {
            ShaderDirector director(asset->mName, asset->mGLSLFilePath, asset->mStage);
            mAssets[id] = director.Make(builder);
        }
        else if (!asset->mGLSLSource.empty())
        {
            ShaderDirector director(asset->mName, asset->mGLSLSource, asset->mStage);
            mAssets[id] = director.Make(builder);
        }
        else if (!asset->mSPIRVCode.empty())
        {
            ShaderDirector director(asset->mName, asset->mSPIRVCode, asset->mStage);
            mAssets[id] = director.Make(builder);
        }
    }
}
} // namespace MEngine::Function
