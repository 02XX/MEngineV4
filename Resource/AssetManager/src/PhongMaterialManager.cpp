#include "PhongMaterialManager.hpp"
#include "Logger.hpp"
#include "PhongMaterial.hpp"
#include <unordered_set>
#include <vector>
namespace MEngine::Resource
{

void PhongMaterialManager::CreateDefault()
{
    auto forwardOpaque = CreateForwardOpaque();
    // auto forwardTransparent = CreateForwardTransparent();

    forwardOpaque->mID = mDefaultMaterials.at(DefaultPhongMaterialType::ForwardOpaque);
    // forwardTransparent->mID = mDefaultMaterials.at(DefaultPhongMaterialType::ForwardTransparent);

    Add(forwardOpaque);
    // Add(forwardTransparent);
}
std::shared_ptr<PhongMaterial> PhongMaterialManager::CreateForwardOpaque()
{
    auto pipeline = mPipelineManager->GetByName(DefaultGraphicPipelineType::ForwardOpaquePhong);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PhongParam param{};
    PhongTextures textures{
        .mDiffuseTexture = texture,
        .mSpecularTexture = texture,
    };
    auto phongMaterial =
        std::make_shared<PhongMaterial>(DefaultPhongMaterialType::ForwardOpaque, pipeline, param, textures);
    LogInfo("Created「Default ForwardPhong_Opaque」phongMaterial");
    return phongMaterial;
}
std::shared_ptr<PhongMaterial> PhongMaterialManager::CreateForwardTransparent()
{
    auto pipeline = mPipelineManager->GetByName(DefaultGraphicPipelineType::ForwardTransparentPhong);
    auto texture = mTextureManager->GetTexture2D(DefaultTextureType::White);
    PhongParam param{};
    PhongTextures textures{
        .mDiffuseTexture = texture,
        .mSpecularTexture = texture,
    };
    auto phongMaterial =
        std::make_shared<PhongMaterial>(DefaultPhongMaterialType::ForwardTransparent, pipeline, param, textures);
    LogInfo("Created「Default Lighting_Phong」phongMaterial");
    return phongMaterial;
}
void PhongMaterialManager::UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                                     vk::CommandBufferInheritanceInfo *inheritanceInfo)
{
    std::unordered_set<std::shared_ptr<PhongMaterial>> phongMaterialToUpdate{};
    phongMaterialToUpdate.reserve(mPendingUpdateAssets.Size());
    std::shared_ptr<PhongMaterial> phongMaterial{};
    while (mPendingUpdateAssets.TryPop(phongMaterial))
    {
        phongMaterialToUpdate.insert(phongMaterial);
    }

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(inheritanceInfo);
    commandBuffer.begin(beginInfo);
    for (auto &phongMaterial : phongMaterialToUpdate)
    {
        auto phongMaterialResource = phongMaterial->GetResourceAs<PhongMaterialResource>();
        mTextureManager->PushPendingUpdateAsset(phongMaterial->mTextures.mDiffuseTexture);
        mTextureManager->PushPendingUpdateAsset(phongMaterial->mTextures.mSpecularTexture);
        void *data;
        uint8_t *mappedData = static_cast<uint8_t *>(phongMaterialResource->mStagingBufferAllocationInfo.pMappedData);
        std::memcpy(mappedData, &phongMaterial->mParam, sizeof(PhongParam));
        vk::BufferCopy copyRegion{};
        copyRegion.setSize(sizeof(PhongParam)).setSrcOffset(0).setDstOffset(0);
        commandBuffer.copyBuffer(phongMaterialResource->mStagingBuffer, phongMaterialResource->mBuffer, copyRegion);
    }
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &phongMaterial : phongMaterialToUpdate)
    {
        auto phongMaterialResource = phongMaterial->GetResourceAs<PhongMaterialResource>();
        vk::BufferMemoryBarrier2 bufferBarrier{};
        bufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(phongMaterialResource->mBuffer)
            .setOffset(0)
            .setSize(sizeof(PhongParam));
        bufferBarriers.push_back(bufferBarrier);
    }
    vk::DependencyInfo depInfo{};
    depInfo.setBufferMemoryBarriers(bufferBarriers);
    commandBuffer.pipelineBarrier2(depInfo);
    commandBuffer.end();
}
} // namespace MEngine::Resource