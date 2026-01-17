#include "Context.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Platform;
class CommandBufferTest : public ::testing::Test
{
  protected:
    std::shared_ptr<Context> mContext;
    void SetUp() override
    {
        ContextConfig config{};
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        mContext = std::make_shared<Context>(config);
    }
    void TearDown() override
    {
    }
};

TEST_F(CommandBufferTest, SubmitEmptyCommandBuffer)
{
    auto device = mContext->Device.get();
    vk::UniqueCommandPool commandPool =
        device.createCommandPoolUnique(vk::CommandPoolCreateInfo{}
                                           .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                           .setQueueFamilyIndex(mContext->QueueFamilyIndicates.graphicsFamily.value()));
    auto commandBuffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{}
                                                                  .setCommandPool(commandPool.get())
                                                                  .setLevel(vk::CommandBufferLevel::ePrimary)
                                                                  .setCommandBufferCount(1));
    vk::CommandBuffer commandBuffer = commandBuffers[0].get();
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);
    commandBuffer.end();
    vk::SubmitInfo2 submitInfo{};
    std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
        vk::CommandBufferSubmitInfo().setCommandBuffer(commandBuffer),
    };
    submitInfo.setCommandBufferInfos(commandBufferInfos);
    mContext->GraphicsQueue.submit2(submitInfo, {});
    device.waitIdle();
}
TEST_F(CommandBufferTest, SubmitEmptySubCommandBuffers)
{
    auto device = mContext->Device.get();
    vk::UniqueCommandPool commandPool =
        device.createCommandPoolUnique(vk::CommandPoolCreateInfo{}
                                           .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                           .setQueueFamilyIndex(mContext->QueueFamilyIndicates.graphicsFamily.value()));
    auto primaryCommandBuffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{}
                                                                         .setCommandPool(commandPool.get())
                                                                         .setLevel(vk::CommandBufferLevel::ePrimary)
                                                                         .setCommandBufferCount(1));
    vk::CommandBuffer primaryCommandBuffer = primaryCommandBuffers[0].get();

    auto secondaryCommandBuffers = device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{}
                                                                           .setCommandPool(commandPool.get())
                                                                           .setLevel(vk::CommandBufferLevel::eSecondary)
                                                                           .setCommandBufferCount(1));
    vk::CommandBuffer secondaryCommandBuffer = secondaryCommandBuffers[0].get();
    vk::CommandBufferInheritanceInfo inheritanceInfo{};
    vk::CommandBufferBeginInfo secondaryBeginInfo{};
    secondaryBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(&inheritanceInfo);
    secondaryCommandBuffer.begin(secondaryBeginInfo);
    secondaryCommandBuffer.end();
    vk::CommandBufferBeginInfo primaryBeginInfo{};
    primaryBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    primaryCommandBuffer.begin(primaryBeginInfo);
    primaryCommandBuffer.executeCommands(secondaryCommandBuffer);
    primaryCommandBuffer.end();
    vk::SubmitInfo2 submitInfo{};
    std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
        vk::CommandBufferSubmitInfo().setCommandBuffer(primaryCommandBuffer),
    };
    submitInfo.setCommandBufferInfos(commandBufferInfos);
    mContext->GraphicsQueue.submit2(submitInfo, {});
    device.waitIdle();
}