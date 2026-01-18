#include "Texture2DManager.hpp"
#include "Context.hpp"
#include "Texture.hpp"
#include "VMA.hpp"
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_structs.hpp>

namespace MEngine::Resource
{

void Texture2DManager::CreateDefault()
{
    auto whiteTexture = CreateWhiteTexture();
    auto blackTexture = CreateBlackTexture();
    auto magentaTexture = CreateMagentaTexture();
    whiteTexture->SetID(mDefaultTextures[DefaultTextureType::White]);
    blackTexture->SetID(mDefaultTextures[DefaultTextureType::Black]);
    magentaTexture->SetID(mDefaultTextures[DefaultTextureType::Magenta]);
    Add(whiteTexture);
    Add(blackTexture);
    Add(magentaTexture);
    PushPendingUpdateAsset(whiteTexture);
    PushPendingUpdateAsset(blackTexture);
    PushPendingUpdateAsset(magentaTexture);
}
std::shared_ptr<Texture2D> Texture2DManager::CreateWhiteTexture()
{
    TextureSetting textureSetting{};
    SamplerSetting samplerSetting{};
    textureSetting.extent = vk::Extent3D{1, 1, 1};
    textureSetting.mipLevels = 1;
    textureSetting.format = vk::Format::eR8G8B8A8Unorm;
    textureSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    textureSetting.samples = vk::SampleCountFlagBits::e1;
    auto texture = std::make_shared<Texture2D>("White", textureSetting, samplerSetting);
    texture->SetTextureData({
        Texture2DMipMap{
            .Data = {255, 255, 255, 255}, // White pixel
            .Extent = {1, 1, 1},
        },
    });
    return texture;
}
std::shared_ptr<Texture2D> Texture2DManager::CreateMagentaTexture()
{
    TextureSetting textureSetting{};
    SamplerSetting samplerSetting{};
    textureSetting.extent = vk::Extent3D{1, 1, 1};
    textureSetting.mipLevels = 1;
    textureSetting.format = vk::Format::eR8G8B8A8Unorm;
    textureSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    textureSetting.samples = vk::SampleCountFlagBits::e1;
    auto texture = std::make_shared<Texture2D>("Magenta", textureSetting, samplerSetting);
    texture->SetTextureData({
        Texture2DMipMap{
            .Data = {255, 0, 255, 255}, // Magenta pixel
            .Extent = {1, 1, 1},
        },
    });
    return texture;
}
std::shared_ptr<Texture2D> Texture2DManager::CreateBlackTexture()
{
    TextureSetting textureSetting{};
    SamplerSetting samplerSetting{};
    textureSetting.extent = vk::Extent3D{1, 1, 1};
    textureSetting.mipLevels = 1;
    textureSetting.format = vk::Format::eR8G8B8A8Unorm;
    textureSetting.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    textureSetting.samples = vk::SampleCountFlagBits::e1;
    auto texture = std::make_shared<Texture2D>("Black", textureSetting, samplerSetting);
    texture->SetTextureData({
        Texture2DMipMap{
            .Data = {0, 0, 0, 255}, // Black pixel
            .Extent = {1, 1, 1},
        },
    });
    return texture;
}
std::shared_ptr<Texture2D> Texture2DManager::GetTexture2D(DefaultTextureType type) const
{
    if (mDefaultTextures.contains(type))
    {
        return Get(mDefaultTextures.at(type));
    }
    LogError("Default texture type {} not found", static_cast<int>(type));
    return nullptr;
}
void Texture2DManager::UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                                 vk::CommandBufferInheritanceInfo *inheritanceInfo)
{
    std::unordered_set<std::shared_ptr<Texture2D>> texturesToUpdate{};

    std::shared_ptr<Texture2D> texture{};
    while (mPendingUpdateAssets.TryPop(texture))
    {
        texturesToUpdate.insert(texture);
    }
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(inheritanceInfo);
    commandBuffer.begin(beginInfo);
    // https://vkguide.dev/docs/ascendant/ascendant_light/
    //  In the tutorial, barriers are done as a call like ` vkutil::transition_image(cmd, new_image.image,
    //  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);` . This directly encodes the barrier command.
    //  This was fine with the tutorial, as its sync needs weren’t much. But now that we need to deal with gbuffers and
    //  indirect command buffers, we often see cases like encoding 8 barriers at a time into the commands. The GPU
    //  driver doesn’t really like to handle barriers like this, and it performs better if it does a single
    //  VkCmdPipelineBarrier that does multiple barriers at a time, instead of multiple vkCmdPipelineBarrier calls.
    std::vector<vk::ImageMemoryBarrier2> preBarriers{}, postBarriers{};
    for (auto texture : texturesToUpdate)
    {
        vk::ImageMemoryBarrier2 preBarrier{};
        auto texture2DResource = texture->GetResourceAs<Texture2DResource>();
        auto &textureSetting = texture->GetTextureSettings();
        auto &samplerSetting = texture->GetSamplerSettings();
        if (texture2DResource->mWroteStagingBuffer)
        {
            preBarrier.setImage(texture2DResource->GetImage())
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                                               textureSetting.mipLevels, 0, textureSetting.arrayLayers})
                .setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                .setSrcStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
                .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
                .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
                .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite);
        }
        else
        {
            texture2DResource->InitResource(context);
            preBarrier.setImage(texture2DResource->GetImage())
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                                               textureSetting.mipLevels, 0, textureSetting.arrayLayers})
                .setOldLayout(vk::ImageLayout::eUndefined)
                .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
                .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
                .setSrcAccessMask(vk::AccessFlagBits2::eNone)
                .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite);
        }
        preBarriers.push_back(preBarrier);
    }
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setImageMemoryBarriers(preBarriers);
    commandBuffer.pipelineBarrier2(dependencyInfo);
    for (auto texture : texturesToUpdate)
    {
        auto texture2DResource = texture->GetResourceAs<Texture2DResource>();
        auto &textureSetting = texture->GetTextureSettings();
        auto &samplerSetting = texture->GetSamplerSettings();
        // Copy texture data to staging buffer
        uint8_t *mappedData =
            reinterpret_cast<uint8_t *>(texture2DResource->GetStagingBufferAllocationInfo().pMappedData);
        size_t offset = 0;
        for (const auto &mip : texture->GetTextureData())
        {
            std::memcpy(mappedData + offset, mip.Data.data(), mip.Data.size());
            offset += mip.Data.size();
        }
        texture2DResource->mWroteStagingBuffer = true;
        if (texture->GetTextureData().empty())
        {
            throw std::runtime_error("Texture data is empty");
        }

        if (textureSetting.mipLevels != texture->GetTextureData().size())
        {
            LogWarn(
                "Texture mipmap levels do not match the provided mip data size. Using the smaller mipmap level of the "
                "two.");
        }
        size_t bufferOffset = 0;
        std::vector<vk::BufferImageCopy2> copyRegions{};
        for (size_t i = 0; i < textureSetting.mipLevels && i < texture->GetTextureData().size(); i++)
        {
            vk::BufferImageCopy2 copyRegion{};
            auto &mipData = texture->GetTextureData()[i];
            copyRegion.setBufferOffset(bufferOffset)
                .setBufferRowLength(0)
                .setBufferImageHeight(0)
                .setImageSubresource(
                    vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, static_cast<uint32_t>(i), 0, 1})
                .setImageExtent(mipData.Extent);
            bufferOffset += mipData.Data.size();
            copyRegions.push_back(copyRegion);
        }
        vk::CopyBufferToImageInfo2 copyInfo{};
        copyInfo.setDstImage(texture2DResource->GetImage())
            .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
            .setSrcBuffer(texture2DResource->GetStagingBuffer())
            .setRegions(copyRegions);
        commandBuffer.copyBufferToImage2(copyInfo);
    }
    for (auto texture : texturesToUpdate)
    {
        auto texture2DResource = texture->GetResourceAs<Texture2DResource>();
        auto &textureSetting = texture->GetTextureSettings();
        auto &samplerSetting = texture->GetSamplerSettings();
        vk::ImageMemoryBarrier2 postBarrier{};
        postBarrier.setImage(texture->GetResourceAs<Texture2DResource>()->GetImage())
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, textureSetting.mipLevels,
                                                           0, textureSetting.arrayLayers})
            .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
        postBarriers.push_back(postBarrier);
    }
    dependencyInfo.setImageMemoryBarriers(postBarriers);
    commandBuffer.pipelineBarrier2(dependencyInfo);
    commandBuffer.end();
}
} // namespace MEngine::Resource
