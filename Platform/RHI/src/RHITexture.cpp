#include "RHITexture.hpp"
#include <limits>

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
}
RHITexture::~RHITexture()
{
    if (mImage)
    {
        auto &rhiContext = RHIContext::Instance();
        vmaDestroyImage(rhiContext.GetVmaAllocator(), mImage, mAllocation);
        mImage = nullptr;
        mAllocation = nullptr;
    }
}

void RHITexture::TransitionImageLayout(vk::ImageLayout newLayout)
{
    auto &rhiContext = RHIContext::Instance();
    auto commandBuffer = rhiContext.GetGraphicsCommandBuffer(vk::CommandBufferLevel::ePrimary);
    auto graphicsFamily = rhiContext.GetQueueFamilyIndicates().graphicsFamily.value();
    vk::FenceCreateInfo fenceCreateInfo;
    auto fence = rhiContext.GetDevice().createFenceUnique(fenceCreateInfo);

    vk::ImageMemoryBarrier barrier{};
    barrier.setOldLayout(mCurrentLayout)
        .setNewLayout(newLayout)
        .setSrcQueueFamilyIndex(graphicsFamily)
        .setDstQueueFamilyIndex(graphicsFamily)
        .setImage(mImage)
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mTextureDesc.mipLevels, 0,
                                                       mTextureDesc.arrayLayers));

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (mCurrentLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.setSrcAccessMask({});
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (mCurrentLayout == vk::ImageLayout::eTransferDstOptimal &&
             newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (mCurrentLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask({});
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }
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
} // namespace MEngine::Platform