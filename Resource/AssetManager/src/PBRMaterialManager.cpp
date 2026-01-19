#include "PBRMaterialManager.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{

void PBRMaterialManager::CreateDefault()
{
    auto forwardOpaque = CreateForwardOpaque();
    // auto gBufferOpaque = CreateGBufferOpaque();
    // auto lightingOpaque = CreateLightOpaque();
    forwardOpaque->mID = mDefaultMaterials.at(DefaultPBRMaterialType::ForwardOpaque);
    // gBufferOpaque->mID = mDefaultMaterials.at(DefaultPBRMaterialType::GBufferOpaque);
    // lightingOpaque->mID = mDefaultMaterials.at(DefaultPBRMaterialType::LightingOpaque);
    Add(forwardOpaque);
    // Add(gBufferOpaque);
    // Add(lightingOpaque);
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateForwardOpaque()
{
    auto pipeline = mPipelineManager->GetByName(DefaultGraphicPipelineType::ForwardOpaquePBR);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto pbrMaterial = std::make_shared<PBRMaterial>(DefaultPBRMaterialType::ForwardOpaque, pipeline, props, textures);
    LogInfo("Created「Default ForwardPBR_Opaque」pbrMaterial");
    return pbrMaterial;
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateGBufferOpaque()
{
    auto pipeline = mPipelineManager->GetByName(DefaultGraphicPipelineType::GBufferOpaquePBR);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto pbrMaterial = std::make_shared<PBRMaterial>(DefaultPBRMaterialType::GBufferOpaque, pipeline, props, textures);
    LogInfo("Created「Default GBufferPBR_Opaque」pbrMaterial");
    return pbrMaterial;
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateLightOpaque()
{
    auto pipeline = mPipelineManager->GetByName(DefaultGraphicPipelineType::LightingOpaquePBR);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto pbrMaterial = std::make_shared<PBRMaterial>(DefaultPBRMaterialType::LightingOpaque, pipeline, props, textures);
    LogInfo("Created「Default Lighting_PBR」pbrMaterial");
    return pbrMaterial;
}
void PBRMaterialManager::UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                                   vk::CommandBufferInheritanceInfo *inheritanceInfo)
{
    std::unordered_set<std::shared_ptr<PBRMaterial>> pbrMaterialToUpdate{};
    pbrMaterialToUpdate.reserve(mPendingUpdateAssets.Size());
    std::shared_ptr<PBRMaterial> pbrMaterial{};
    while (mPendingUpdateAssets.TryPop(pbrMaterial))
    {
        pbrMaterialToUpdate.insert(pbrMaterial);
    }

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(inheritanceInfo);
    commandBuffer.begin(beginInfo);
    for (auto &pbrMaterial : pbrMaterialToUpdate)
    {
        auto pbrMaterialResource = pbrMaterial->GetResourceAs<PBRMaterialResource>();
        mTextureManager->PushPendingUpdateAsset(pbrMaterial->mTextures.Albedo);
        mTextureManager->PushPendingUpdateAsset(pbrMaterial->mTextures.Normal);
        mTextureManager->PushPendingUpdateAsset(pbrMaterial->mTextures.ARM);
        mTextureManager->PushPendingUpdateAsset(pbrMaterial->mTextures.Emissive);
        void *data;
        uint8_t *mappedData = static_cast<uint8_t *>(pbrMaterialResource->GetStagingBufferAllocationInfo().pMappedData);
        std::memcpy(mappedData, &pbrMaterial->mProperties, sizeof(PBRProperties));
        vk::BufferCopy copyRegion{};
        copyRegion.setSize(sizeof(PBRProperties)).setSrcOffset(0).setDstOffset(0);
        commandBuffer.copyBuffer(pbrMaterialResource->GetStagingBuffer(), pbrMaterialResource->GetBuffer(), copyRegion);
    }
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &pbrMaterial : pbrMaterialToUpdate)
    {
        auto pbrMaterialResource = pbrMaterial->GetResourceAs<PBRMaterialResource>();
        vk::BufferMemoryBarrier2 bufferBarrier{};
        bufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(pbrMaterialResource->GetBuffer())
            .setOffset(0)
            .setSize(sizeof(PBRProperties));
        bufferBarriers.push_back(bufferBarrier);
    }
    vk::DependencyInfo depInfo{};
    depInfo.setBufferMemoryBarriers(bufferBarriers);
    commandBuffer.pipelineBarrier2(depInfo);
    commandBuffer.end();
}
} // namespace MEngine::Resource