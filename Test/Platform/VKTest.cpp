#include <gtest/gtest.h>
#include <vulkan/vulkan.hpp>

TEST(VKTest, PipelineVertexInputStateCreateInfo)
{
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.setBinding(0).setStride(sizeof(float) * 5).setInputRate(vk::VertexInputRate::eVertex);
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].setBinding(0).setLocation(0).setFormat(vk::Format::eR32G32B32Sfloat).setOffset(0);
        attributeDescriptions[1]
            .setBinding(0)
            .setLocation(1)
            .setFormat(vk::Format::eR32G32Sfloat)
            .setOffset(sizeof(float) * 3);
        vertexInputInfo.setVertexBindingDescriptions(bindingDescription)
            .setVertexAttributeDescriptions(attributeDescriptions);
    }
    EXPECT_NE(vertexInputInfo.pVertexAttributeDescriptions, nullptr);
    EXPECT_NE(vertexInputInfo.pVertexBindingDescriptions, nullptr);
}
TEST(VKTest, PipelineInputAssemblyStateCreateInfo)
{
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList).setPrimitiveRestartEnable(VK_FALSE);
    EXPECT_EQ(inputAssembly.topology, vk::PrimitiveTopology::eTriangleList);
    EXPECT_EQ(inputAssembly.primitiveRestartEnable, VK_FALSE);
}
TEST(VKTest, PipelineRasterizationStateCreateInfo)
{
    vk::PipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.setDepthClampEnable(VK_FALSE)
        .setRasterizerDiscardEnable(VK_FALSE)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setDepthBiasEnable(VK_FALSE)
        .setLineWidth(1.0f);
    EXPECT_EQ(rasterizationState.polygonMode, vk::PolygonMode::eFill);
    EXPECT_EQ(rasterizationState.cullMode, vk::CullModeFlagBits::eBack);
    EXPECT_EQ(rasterizationState.frontFace, vk::FrontFace::eClockwise);
}