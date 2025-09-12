#include "PipelineLayoutManager.hpp"
#include "GBufferPBRPipelineLayoutBuilder.hpp"
#include "LightingPBRPipelineLayoutBuilder.hpp"
#include "Manager.hpp"
#include "PipelineLayoutDirector.hpp"

namespace MEngine::Function
{
PipelineLayoutManager::PipelineLayoutManager(std::shared_ptr<VulkanContext> vulkanContext)
    : Manager<PipelineLayout>(vulkanContext)
{
    PipelineLayoutDirector director;
    GBufferPBRPipelineLayoutBuilder builder(mVulkanContext);
    Add(director.Make(builder));
    LightingPBRPipelineLayoutBuilder lightingBuilder(mVulkanContext);
    Add(director.Make(lightingBuilder));
}
void PipelineLayoutManager::Add(std::shared_ptr<PipelineLayout> asset)
{
    Manager<PipelineLayout>::Add(asset);
    mTypeToID[asset->GetPipelineLayoutType()] = asset->GetID();
}

std::shared_ptr<PipelineLayout> PipelineLayoutManager::GetByType(PipelineLayoutType type) const
{
    if (mTypeToID.contains(type))
    {
        return Get(mTypeToID.at(type));
    }
    return nullptr;
}

void PipelineLayoutManager::Update(const UUID &id, std::shared_ptr<PipelineLayout> asset)
{
}
void PipelineLayoutManager::CreateGPUResource(std::shared_ptr<PipelineLayout> asset)
{
}
} // namespace MEngine::Function
