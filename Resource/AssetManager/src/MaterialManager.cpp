#include "MaterialManager.hpp"
#include "GraphicPipeline.hpp"
#include "IPendingResourceManager.hpp"
#include "Logger.hpp"
#include "Material.hpp"
#include "MaterialResource.hpp"
#include "PhongMaterial.hpp"
#include "PhongMaterialResource.hpp"
#include "PipelineManager.hpp"
#include "RenderResource.hpp"
#include "Texture2D.hpp"
#include <memory>
#include <unordered_set>
#include <vector>
namespace MEngine::Resource
{
MaterialManager::MaterialManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager,
                                 std::shared_ptr<TextureManager> textureManager)
    : Manager<Material, MaterialResource>(context), mTextureManager(textureManager), mPipelineManager(pipelineManager)
{
    PhongParams phongParams{};
    PhongTextures phongTextures{
        .DiffuseTexture = mTextureManager->GetByNameAs<Texture2D>(DefaultTextureType::White2D),
        .SpecularTexture = mTextureManager->GetByNameAs<Texture2D>(DefaultTextureType::White2D),
    };
    auto forwardOpaquePhong = std::make_shared<PhongMaterial>(
        DefaultMaterialType::ForwardOpaquePhong,
        mPipelineManager->GetGraphicPipelineByName(DefaultGraphicPipelineType::ForwardOpaquePhong), phongParams,
        phongTextures);

    forwardOpaquePhong->SetID(mDefaultMaterials.at(DefaultMaterialType::ForwardOpaquePhong));
    Add(forwardOpaquePhong);
}
void MaterialManager::ProcessPendingUpdateResources(RenderContext renderContext)
{
    auto materialResourcesToUpdate = ToVector(mPendingUpdateResources);
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &materialResource : materialResourcesToUpdate)
    {
        auto material = static_cast<Material *>(materialResource->mOwnerAsset);
        if (auto phongMaterialResource = dynamic_cast<PhongMaterialResource *>(materialResource))
        {
            if (!phongMaterialResource->mStagingBuffer)
            {
                phongMaterialResource->InitStaging(renderContext.Context, sizeof(PhongParams));
            }
            phongMaterialResource->Upload();
            if (!material->mDynamic)
            {
                mPendingTasks.Push([phongMaterialResource](RenderContext renderContext) {
                    phongMaterialResource->ReleaseStaging(renderContext.Context);
                });
            }
            vk::BufferCopy copyRegion{};
            copyRegion.setSize(sizeof(PhongParams)).setSrcOffset(0).setDstOffset(0);
            renderContext.CommandBuffer.copyBuffer(phongMaterialResource->mStagingBuffer,
                                                   phongMaterialResource->mBuffer, copyRegion);
            vk::BufferMemoryBarrier2 bufferBarrier{};
            bufferBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eCopy)
                .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader |
                                 vk::PipelineStageFlagBits2::eFragmentShader)
                .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
                .setBuffer(phongMaterialResource->mBuffer)
                .setOffset(0)
                .setSize(sizeof(PhongParams));
            bufferBarriers.push_back(bufferBarrier);
            vk::DependencyInfo dependencyInfo{};
            dependencyInfo.setBufferMemoryBarriers(bufferBarriers);
            renderContext.CommandBuffer.pipelineBarrier2(dependencyInfo);
        }
        else
        {
            LogWarn("MaterialResource of type {} does not support update", typeid(*materialResource).name());
            continue;
        }
    }
}
} // namespace MEngine::Resource