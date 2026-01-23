#include "TextureManager.hpp"
#include "Context.hpp"
#include "IPendingResourceManager.hpp"
#include "PendingResourceManager.hpp"
#include "Texture.hpp"
#include "Texture2D.hpp"
#include "TextureRenderTarget.hpp"
#include "TextureRenderTargetResource.hpp"
#include "TextureResource.hpp"
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"
#include <cstddef>
#include <memory>
#include <unordered_set>
#include <vector>

namespace MEngine::Resource
{
TextureManager::TextureManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager)
    : Manager<Texture, TextureResource>(context), mPipelineManager(pipelineManager)
{

    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.setDescriptorPool(mPipelineManager->mDescriptorPool)
        .setSetLayouts({mPipelineManager->mDefaultDescriptorSetLayouts.at(DefaultDescriptorSetLayoutType::Bindless)})
        .setDescriptorSetCount(1);
    auto descriptorSets = context->Device->allocateDescriptorSets(allocateInfo);
    if (descriptorSets.empty())
    {
        throw std::runtime_error("Failed to allocate descriptor set");
    }
    mTextureBindlessDescriptorSet = descriptorSets.front();

    // Texture2D
    TextureSetting texture2DSetting{};
    texture2DSetting.extent = vk::Extent3D{1, 1, 1};
    texture2DSetting.mipLevels = 1;
    texture2DSetting.format = vk::Format::eR8G8B8A8Unorm;
    texture2DSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    texture2DSetting.samples = vk::SampleCountFlagBits::e1;

    auto magenta2D = std::make_shared<Texture2D>(DefaultTextureType::Magenta2D, texture2DSetting);
    auto white2D = std::make_shared<Texture2D>(DefaultTextureType::White2D, texture2DSetting);
    auto black2D = std::make_shared<Texture2D>(DefaultTextureType::Black2D, texture2DSetting);

    magenta2D->mTextureDatas = {
        MipMap{
            .Datas = {{{255, 0, 255, 255}}}, // Magenta pixel
            .Extent = {1, 1, 1},
        },
    };
    white2D->mTextureDatas = {
        MipMap{
            .Datas = {{{255, 255, 255, 255}}}, // White pixel
            .Extent = {1, 1, 1},
        },
    };
    black2D->mTextureDatas = {
        MipMap{
            .Datas = {{{0, 0, 0, 255}}}, // Black pixel
            .Extent = {1, 1, 1},
        },
    };

    magenta2D->SetID(sDefaultTextures.at(DefaultTextureType::Magenta2D));
    white2D->SetID(sDefaultTextures.at(DefaultTextureType::White2D));
    black2D->SetID(sDefaultTextures.at(DefaultTextureType::Black2D));

    Add(white2D);
    Add(black2D);
    Add(magenta2D);
    // Texture2DArray
    // TODO: Default Texture2DArray
    // TextureCube
    // TODO: Default TextureCube
    // TextureCubeArray
    // TODO: Default TextureCubeArray
}
TextureManager::~TextureManager()
{
    if (mTextureBindlessDescriptorSet)
    {
        mContext->Device->freeDescriptorSets(mPipelineManager->mDescriptorPool, {mTextureBindlessDescriptorSet});
        mTextureBindlessDescriptorSet = nullptr;
    }
}
uint32_t TextureManager::AllocateDescriptorIndex()
{
    if (!mFreeDescriptorIndices.empty())
    {
        uint32_t index = mFreeDescriptorIndices.front();
        mFreeDescriptorIndices.pop();
        return index;
    }
    if (mNextDescriptorIndex >= MAX_DESCRIPTOR_COUNT)
    {
        throw std::runtime_error("Exceeded maximum descriptor count");
    }
    return mNextDescriptorIndex++;
}

void TextureManager::FreeDescriptorIndex(uint32_t index)
{
    if (index >= MAX_DESCRIPTOR_COUNT)
    {
        throw std::runtime_error("Invalid descriptor index");
    }
    mFreeDescriptorIndices.push(index);
    // TODO: 销毁index对应的描述符资源
}
void TextureManager::ProcessPendingUpdateResources(RenderContext renderContext)
{
    // https://vkguide.dev/docs/ascendant/ascendant_light/
    //  In the tutorial, barriers are done as a call like ` vkutil::transition_image(cmd, new_image.image,
    //  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);` . This directly encodes the barrier command.
    //  This was fine with the tutorial, as its sync needs weren’t much. But now that we need to deal with gbuffers and
    //  indirect command buffers, we often see cases like encoding 8 barriers at a time into the commands. The GPU
    //  driver doesn’t really like to handle barriers like this, and it performs better if it does a single
    //  VkCmdPipelineBarrier that does multiple barriers at a time, instead of multiple vkCmdPipelineBarrier calls.
#pragma region 收集待更新/能更新的TextureResource
    auto textureResourcesToUpdate = ToVector(mPendingUpdateResources);
    std::vector<UploadableTextureResource *> uploadableTextureResourcesToUpdate{};
    uploadableTextureResourcesToUpdate.reserve(textureResourcesToUpdate.size());
    for (auto textureResource : textureResourcesToUpdate)
    {
        if (auto uploadableTextureResource = dynamic_cast<UploadableTextureResource *>(textureResource))
        {
            uploadableTextureResourcesToUpdate.push_back(uploadableTextureResource);
        }
        else
        {
            LogError("Unsupported texture type for update");
        }
    }
#pragma endregion
#pragma region 屏障创建
    std::vector<vk::ImageMemoryBarrier2> preBarriers{}, postBarriers{};
    preBarriers.reserve(uploadableTextureResourcesToUpdate.size());
    postBarriers.reserve(uploadableTextureResourcesToUpdate.size());
    std::vector<vk::BufferImageCopy2> bufferImageCopies{};
    bufferImageCopies.reserve(uploadableTextureResourcesToUpdate.size());
    vk::ImageMemoryBarrier2 preBarrier{}, postBarrier{};
    preBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite);
    postBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    vk::ImageSubresourceRange subresourceRange{};
    subresourceRange.setBaseMipLevel(0).setBaseArrayLayer(0);
    for (auto uploadableTextureResource : uploadableTextureResourcesToUpdate)
    {
        auto uploadableTexture = static_cast<UploadableTexture *>(uploadableTextureResource->mOwnerAsset);
        if (uploadableTextureResource->mCurrentLayout == vk::ImageLayout::eUndefined)
        {
            preBarrier.setOldLayout(vk::ImageLayout::eUndefined)
                .setSrcAccessMask(vk::AccessFlagBits2::eNone)
                .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe);
        }
        subresourceRange.setLevelCount(uploadableTexture->mTextureSettings.mipLevels)
            .setLayerCount(uploadableTexture->mTextureSettings.arrayLayers)
            .setAspectMask(uploadableTexture->IsDepthStencil() ? (vk::ImageAspectFlagBits::eDepth)
                                                               : vk::ImageAspectFlagBits::eColor);
        preBarrier.setImage(uploadableTextureResource->mImage).setSubresourceRange(subresourceRange);
        postBarrier.setImage(uploadableTextureResource->mImage).setSubresourceRange(subresourceRange);
        preBarriers.push_back(preBarrier);
        postBarriers.push_back(postBarrier);
    }
#pragma endregion 屏障创建
#pragma region Pre屏障
    vk::DependencyInfo preDependencyInfo{};
    preDependencyInfo.setImageMemoryBarriers(preBarriers);
    renderContext.CommandBuffer.pipelineBarrier2(preDependencyInfo);
#pragma endregion
#pragma region 纹理数据上传
    for (auto uploadableTextureResource : uploadableTextureResourcesToUpdate)
    {
        auto uploadableTexture = static_cast<UploadableTexture *>(uploadableTextureResource->mOwnerAsset);
        if (uploadableTexture->mTextureSettings.mipLevels != uploadableTexture->mTextureDatas.size())
        {
            LogError("Mismatch between texture mip levels and provided mip data size for texture: ID {}, Name {}",
                     uploadableTexture->GetID().ToString(), uploadableTexture->GetName());
            continue;
        }
        if (!uploadableTextureResource->mStagingBuffer)
        {
            auto &datas = uploadableTexture->mTextureDatas;
            vk::DeviceSize totalSize = 0;
            for (size_t mipLevel = 0; mipLevel < datas.size(); ++mipLevel)
            {
                const auto &mipData = datas[mipLevel];
                for (size_t arrayLayer = 0; arrayLayer < mipData.Datas.size(); ++arrayLayer)
                {
                    const auto &layerData = mipData.Datas[arrayLayer];
                    totalSize += layerData.size();
                }
            }
            uploadableTextureResource->InitStaging(renderContext.Context, totalSize);
        }
        uploadableTextureResource->Upload();
        if (!uploadableTexture->mDynamic)
        {
            mPendingTasks.Push([uploadableTextureResource](RenderContext renderContext) mutable {
                uploadableTextureResource->ReleaseStaging(renderContext.Context);
            });
        }
        vk::DeviceSize bufferOffset = 0;
        for (size_t mipLevel = 0; mipLevel < uploadableTexture->mTextureSettings.mipLevels; ++mipLevel)
        {
            vk::BufferImageCopy2 copyRegion{};
            auto &mipData = uploadableTexture->mTextureDatas[mipLevel];
            if (mipData.Datas.size() != uploadableTexture->mTextureSettings.arrayLayers)
            {
                LogError("Mismatch between mipData Datas size and texture array layers.");
                continue;
            }
            vk::ImageSubresourceLayers imageSubresourceLayers{};
            imageSubresourceLayers.setMipLevel(mipLevel)
                .setBaseArrayLayer(0)
                .setLayerCount(mipData.Datas.size())
                .setAspectMask(uploadableTexture->IsDepthStencil() ? (vk::ImageAspectFlagBits::eDepth)
                                                                   : vk::ImageAspectFlagBits::eColor);
            copyRegion.setBufferOffset(bufferOffset)
                .setBufferRowLength(0)
                .setBufferImageHeight(0)
                .setImageSubresource(imageSubresourceLayers)
                .setImageExtent(mipData.Extent);
            vk::CopyBufferToImageInfo2 copyInfo{};
            copyInfo.setDstImage(uploadableTextureResource->mImage)
                .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
                .setSrcBuffer(uploadableTextureResource->mStagingBuffer)
                .setRegions(copyRegion);
            renderContext.CommandBuffer.copyBufferToImage2(copyInfo);
        }
    }
#pragma endregion
#pragma region Post屏障
    vk::DependencyInfo postDependencyInfo{};
    postDependencyInfo.setImageMemoryBarriers(postBarriers);
    renderContext.CommandBuffer.pipelineBarrier2(postDependencyInfo);
#pragma endregion
}
void TextureManager::ProcessPendingDeletionResources(RenderContext renderContext)
{
    std::unique_ptr<TextureResource> resource;
    while (mPendingDeletion.TryPop(resource))
    {
        if (auto uploadableTextureResource = dynamic_cast<UploadableTextureResource *>(resource.get()))
        {
            FreeDescriptorIndex(uploadableTextureResource->mBindlessDescriptorIndex);
        }
        resource->ReleaseResource(renderContext.Context);
    }
}
void TextureManager::Bind(BindContext bindContext)
{
    mBindInfo.setDescriptorSets(mTextureBindlessDescriptorSet).setFirstSet(0).setLayout(bindContext.PipelineLayout);
    bindContext.CommandBuffer.bindDescriptorSets2(mBindInfo);
}
} // namespace MEngine::Resource