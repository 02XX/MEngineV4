#include "PBRMaterialManager.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{

void PBRMaterialManager::CreateDefault()
{
    auto gBufferMaterial = CreateGBufferOpaqueMaterial();
    auto lightingMaterial = CreateLightMaterial();
    gBufferMaterial->mID = mDefaultMaterials.at(DefaultMaterialType::GBufferPBROpaque);
    lightingMaterial->mID = mDefaultMaterials.at(DefaultMaterialType::GBufferPBRTransparent);
    Add(gBufferMaterial);
    Add(lightingMaterial);
    mPendingAssets.Push(gBufferMaterial);
    // mPendingAssets.Push(lightingMaterial);
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateGBufferOpaqueMaterial()
{
    auto pipeline = mPipelineManager->GetPipeline(GraphicPipelineType::GBuffer);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto material = std::make_shared<PBRMaterial>("GBufferPBR_Opaque", pipeline, props, textures);
    LogInfo("Created「Default GBufferPBR_Opaque」material");
    return material;
}
std::shared_ptr<PBRMaterial> PBRMaterialManager::CreateLightMaterial()
{
    auto pipeline = mPipelineManager->GetPipeline(GraphicPipelineType::Lighting);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PBRProperties props{};
    PBRTextures textures{
        .Albedo = texture,
        .Normal = texture,
        .ARM = texture,
        .Emissive = texture,
    };
    auto material = std::make_shared<PBRMaterial>("Lighting_PBR", pipeline, props, textures);
    LogInfo("Created「Default Lighting_PBR」material");
    return material;
}
void PBRMaterialManager::CollectUpdateAssets()
{
    mMaterialToUpdate.clear();
    mMaterialToUpdate.reserve(mPendingAssets.Size());
    std::shared_ptr<PBRMaterial> material{};
    while (mPendingAssets.TryPop(material))
    {
        mMaterialToUpdate.push_back(material);
    }
}
void PBRMaterialManager::UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                                   vk::CommandBufferInheritanceInfo *inheritanceInfo)
{
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(inheritanceInfo);
    commandBuffer.begin(beginInfo);
    for (auto &material : mMaterialToUpdate)
    {
        auto materialResource = material->GetResourceAs<PBRMaterialResource>();
        mTextureManager->mPendingAssets.Push(material->mTextures.Albedo);
        mTextureManager->mPendingAssets.Push(material->mTextures.Normal);
        mTextureManager->mPendingAssets.Push(material->mTextures.ARM);
        mTextureManager->mPendingAssets.Push(material->mTextures.Emissive);
        void *data;
        uint8_t *mappedData = static_cast<uint8_t *>(materialResource->GetStagingBufferAllocationInfo().pMappedData);
        std::memcpy(mappedData, &material->mProperties, sizeof(PBRProperties));
        vk::BufferCopy copyRegion{};
        copyRegion.setSize(sizeof(PBRProperties)).setSrcOffset(0).setDstOffset(0);
        commandBuffer.copyBuffer(materialResource->GetStagingBuffer(), materialResource->GetSSBO(), copyRegion);
    }
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &material : mMaterialToUpdate)
    {
        auto materialResource = material->GetResourceAs<PBRMaterialResource>();
        vk::BufferMemoryBarrier2 bufferBarrier{};
        bufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(materialResource->GetSSBO())
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