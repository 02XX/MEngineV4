#include "MaterialManager.hpp"
#include "GraphicPipeline.hpp"
#include "Logger.hpp"
#include "Material.hpp"
#include "MaterialResource.hpp"
#include "PhongMaterial.hpp"
#include "PipelineManager.hpp"
#include <memory>
#include <unordered_set>
#include <vector>
namespace MEngine::Resource
{
MaterialManager::MaterialManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager,
                                 std::shared_ptr<TextureManager> textureManager)
    : Manager<Material, MaterialResource>(context), mTextureManager(textureManager), mPipelineManager(pipelineManager)
{
    auto forwardOpaquePhong = std::make_shared<PhongMaterial>(
        DefaultMaterialType::ForwardOpaquePhong,
        mPipelineManager->GetGraphicPipelineByName(DefaultGraphicPipelineType::ForwardOpaquePhong));

    forwardOpaquePhong->SetID(mDefaultMaterials.at(DefaultMaterialType::ForwardOpaquePhong));
    Add(forwardOpaquePhong);
}
void MaterialManager::ProcessPendingInitResources(RenderContext renderContext)
{
    auto materialResourcesToInit = ToVector(mPendingInitResources);
    for (auto materialResource : materialResourcesToInit)
    {
        materialResource->InitResource(renderContext.Context);
        auto material = static_cast<Material *>(materialResource->mOwnerAsset);
        if (material->mDynamic)
        {
            materialResource->InitStaging(renderContext.Context, material->mBufferSize);
        }
    }
}
void MaterialManager::ProcessPendingUpdateResources(RenderContext renderContext)
{
    auto materialResourcesToUpdate = ToVector(mPendingUpdateResources);
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &materialResource : materialResourcesToUpdate)
    {
        auto material = static_cast<Material *>(materialResource->mOwnerAsset);
        if (!materialResource->mStagingBuffer)
        {
            materialResource->InitStaging(renderContext.Context, material->mBufferSize);
        }
        materialResource->UploadData();
        if (!material->mDynamic)
        {
            // mPendingTasks.Push([materialResource](std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer)
            // {
            //     materialResource->ReleaseStaging(context);
            // });
        }
        vk::BufferCopy copyRegion{};
        copyRegion.setSize(material->mBufferSize).setSrcOffset(0).setDstOffset(0);
        renderContext.CommandBuffer.copyBuffer(materialResource->mStagingBuffer, materialResource->mBuffer, copyRegion);
        vk::BufferMemoryBarrier2 bufferBarrier{};
        bufferBarrier.setSrcStageMask(vk::PipelineStageFlagBits2::eCopy)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(materialResource->mBuffer)
            .setOffset(0)
            .setSize(material->mBufferSize);
        bufferBarriers.push_back(bufferBarrier);
    }

    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setBufferMemoryBarriers(bufferBarriers);
    renderContext.CommandBuffer.pipelineBarrier2(dependencyInfo);
}
} // namespace MEngine::Resource