#include "RHITexture.hpp"
#include "RHIContext.hpp"
#include <cstdint>
#include <format>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace MEngine::Platform
{
RHITexture::RHITexture(const RHITextureDesc &desc)
    : RHIResource(), mTextureDesc(desc), mCurrentLayout(vk::ImageLayout::eUndefined)
{
    auto &rhiContext = RHIContext::Instance();
    auto result = vmaCreateImage(rhiContext.GetVmaAllocator(), reinterpret_cast<VkImageCreateInfo *>(&mTextureDesc),
                                 &mAllocationCreateInfo, reinterpret_cast<VkImage *>(&mImage), &mAllocation,
                                 reinterpret_cast<VmaAllocationInfo *>(&mAllocationInfo));
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image!");
    }
    mSubresourceRange.setBaseArrayLayer(0)
        .setLayerCount(mTextureDesc.arrayLayers)
        .setBaseMipLevel(0)
        .setLevelCount(mTextureDesc.mipLevels);
    switch (mTextureDesc.format)
    {
    case vk::Format::eD16Unorm:
    case vk::Format::eX8D24UnormPack32:
    case vk::Format::eD32Sfloat:
    case vk::Format::eS8Uint:
    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        mSubresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
        break;
    default:
        mSubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        break;
    }
}
RHITexture::~RHITexture()
{
    if (mImage && !mExternalImage)
    {
        auto &rhiContext = RHIContext::Instance();
        vmaDestroyImage(rhiContext.GetVmaAllocator(), mImage, mAllocation);
        mImage = nullptr;
        mAllocation = nullptr;
    }
}

void RHITexture::TransitionImageLayout(vk::ImageLayout newLayout)
{
    if (newLayout == mCurrentLayout)
    {
        return;
    }
    auto &rhiContext = RHIContext::Instance();
    auto commandBuffer = rhiContext.GetGraphicsCommandBuffer(vk::CommandBufferLevel::ePrimary);
    auto graphicsFamily = rhiContext.GetQueueFamilyIndicates().graphicsFamily.value();
    vk::FenceCreateInfo fenceCreateInfo;
    auto fence = rhiContext.GetDevice().createFenceUnique(fenceCreateInfo);
    static const std::unordered_map<vk::ImageLayout, std::pair<vk::AccessFlags, vk::PipelineStageFlags>> barrierCache =
        {{vk::ImageLayout::eUndefined, {vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eTopOfPipe}},

         {vk::ImageLayout::eGeneral,
          {vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite,
           vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eFragmentShader}},

         {vk::ImageLayout::eColorAttachmentOptimal,
          {vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead,
           vk::PipelineStageFlagBits::eColorAttachmentOutput}},

         {vk::ImageLayout::eTransferDstOptimal,
          {vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eTransferRead,
           vk::PipelineStageFlagBits::eTransfer}},

         {vk::ImageLayout::eTransferSrcOptimal,
          {vk::AccessFlagBits::eTransferRead | vk::AccessFlagBits::eTransferWrite,
           vk::PipelineStageFlagBits::eTransfer}},

         {vk::ImageLayout::eShaderReadOnlyOptimal,
          {vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite,
           vk::PipelineStageFlagBits::eFragmentShader}},
         {vk::ImageLayout::eDepthStencilAttachmentOptimal,
          {vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
           vk::PipelineStageFlagBits::eLateFragmentTests}},

         {vk::ImageLayout::ePresentSrcKHR, {vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eTopOfPipe}}};
    vk::ImageMemoryBarrier barrier{};
    if (!barrierCache.contains(mCurrentLayout) || !barrierCache.contains(newLayout))
    {
        throw std::runtime_error(
            std::format("UnSupport {} to {}", vk::to_string(mCurrentLayout), vk::to_string(newLayout)));
    }
    auto [sourceAccessMask, sourceStage] = barrierCache.at(mCurrentLayout);
    auto [destinationAccessMask, destinationStage] = barrierCache.at(newLayout);
    barrier.setOldLayout(mCurrentLayout)
        .setNewLayout(newLayout)
        .setSrcAccessMask(sourceAccessMask)
        .setDstAccessMask(destinationAccessMask)
        .setSrcQueueFamilyIndex(graphicsFamily)
        .setDstQueueFamilyIndex(graphicsFamily)
        .setImage(mImage)
        .setSubresourceRange(mSubresourceRange);
    commandBuffer->begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandBuffer->pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
    commandBuffer->end();

    rhiContext.GetGraphicsQueue().submit({vk::SubmitInfo{}.setCommandBuffers(commandBuffer.get())}, {fence.get()});
    auto result = rhiContext.GetDevice().waitForFences({fence.get()}, vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to wait for fence");
    }
    mCurrentLayout = newLayout;
}
void RHITexture::CopyTo(RHITexture *dstTexture)
{
    auto &rhiContext = RHIContext::Instance();
    auto commandBuffer = rhiContext.GetGraphicsCommandBuffer(vk::CommandBufferLevel::ePrimary);
    auto graphicsFamily = rhiContext.GetQueueFamilyIndicates().graphicsFamily.value();
    vk::FenceCreateInfo fenceCreateInfo;
    auto fence = rhiContext.GetDevice().createFenceUnique(fenceCreateInfo);
    auto dstLayout = dstTexture->GetCurrentLayout();
    auto thisLayout = this->GetCurrentLayout();
    dstTexture->TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);
    this->TransitionImageLayout(vk::ImageLayout::eTransferSrcOptimal);
    vk::ImageSubresourceLayers subresourceLayers;
    subresourceLayers.setAspectMask(mSubresourceRange.aspectMask)
        .setBaseArrayLayer(mSubresourceRange.baseArrayLayer)
        .setLayerCount(mSubresourceRange.layerCount)
        .setMipLevel(mSubresourceRange.baseMipLevel);
    vk::ImageCopy copyRegion{};
    copyRegion.setSrcSubresource(subresourceLayers)
        .setDstSubresource(subresourceLayers)
        .setSrcOffset({0, 0, 0})
        .setDstOffset({0, 0, 0})
        .setExtent(mTextureDesc.extent);
    commandBuffer->begin(vk::CommandBufferBeginInfo{});
    commandBuffer->copyImage(mImage, vk::ImageLayout::eTransferSrcOptimal, dstTexture->mImage,
                             vk::ImageLayout::eTransferDstOptimal, {copyRegion});
    commandBuffer->end();
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers({commandBuffer.get()});
    RHIContext::Instance().GetTransferQueue().submit(submitInfo, fence.get());
    auto result = rhiContext.GetDevice().waitForFences({fence.get()}, vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to wait for fence");
    }
    this->TransitionImageLayout(thisLayout);
    dstTexture->TransitionImageLayout(dstLayout);
}
} // namespace MEngine::Platform