#include "PBRMaterialResource.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include <cstring>
namespace MEngine::Resource
{
void PBRMaterialResource::InitRHI()
{
    // create uniform buffer
    RHIBufferDesc bufferDesc{};
    bufferDesc.size = sizeof(PBRProperties);
    bufferDesc.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    mRHIUniformBufferHandler = RHIHandler<RHIBuffer>(new RHIBuffer(bufferDesc, allocCreateInfo));
}
void PBRMaterialResource::ReleaseRHI()
{
    mRHIUniformBufferHandler.SafeRelease();
    mDescriptorSetHandler.SafeRelease();
}
void PBRMaterialResource::UpdateDescriptorSet(uint32_t set)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mMaterial);
    if (set >= mDescriptorSetHandler->GetDescriptorSets().size())
    {
        LogError("Descriptor set index out of range");
        return;
    }
    auto descriptorSet = mDescriptorSetHandler->GetDescriptorSets()[set];
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets{};
    // parameters
    memcpy(mRHIUniformBufferHandler->GetAllocationInfo().pMappedData, &pbrMaterial->GetProperties(),
           sizeof(PBRProperties));
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.setBuffer(mRHIUniformBufferHandler->GetBuffer()).setOffset(0).setRange(sizeof(PBRProperties));
    writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                      .setDstSet(descriptorSet)
                                      .setDstBinding(0)
                                      .setDstArrayElement(0)
                                      .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                      .setBufferInfo(bufferInfo));
    // albedo
    vk::DescriptorImageInfo albedoImageInfo{};
    auto albedoTexture = pbrMaterial->GetTextures().Albedo;
    albedoImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(albedoTexture->GetResourceAs<Texture2DResource>()->GetTextureView()->GetImageView())
        .setSampler(albedoTexture->GetResourceAs<Texture2DResource>()->GetSampler()->GetSampler());
    writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                      .setDstSet(descriptorSet)
                                      .setDstBinding(1)
                                      .setDstArrayElement(0)
                                      .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                      .setImageInfo(albedoImageInfo));
    // normal
    vk::DescriptorImageInfo normalImageInfo{};
    auto normalTexture = pbrMaterial->GetTextures().Normal;
    normalImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(normalTexture->GetResourceAs<Texture2DResource>()->GetTextureView()->GetImageView())
        .setSampler(normalTexture->GetResourceAs<Texture2DResource>()->GetSampler()->GetSampler());
    writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                      .setDstSet(descriptorSet)
                                      .setDstBinding(2)
                                      .setDstArrayElement(0)
                                      .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                      .setImageInfo(normalImageInfo));
    // arm
    vk::DescriptorImageInfo armImageInfo{};
    auto armTexture = pbrMaterial->GetTextures().ARM;
    armImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(armTexture->GetResourceAs<Texture2DResource>()->GetTextureView()->GetImageView())
        .setSampler(armTexture->GetResourceAs<Texture2DResource>()->GetSampler()->GetSampler());
    writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                      .setDstSet(descriptorSet)
                                      .setDstBinding(3)
                                      .setDstArrayElement(0)
                                      .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                      .setImageInfo(armImageInfo));
    // emissive
    vk::DescriptorImageInfo emissiveImageInfo{};
    auto emissiveTexture = pbrMaterial->GetTextures().Emissive;
    emissiveImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(emissiveTexture->GetResourceAs<Texture2DResource>()->GetTextureView()->GetImageView())
        .setSampler(emissiveTexture->GetResourceAs<Texture2DResource>()->GetSampler()->GetSampler());
    writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                      .setDstSet(descriptorSet)
                                      .setDstBinding(4)
                                      .setDstArrayElement(0)
                                      .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                      .setImageInfo(emissiveImageInfo));
    auto &device = Platform::RHIContext::Instance().GetDevice();
    device.updateDescriptorSets(writeDescriptorSets, {});
}
} // namespace MEngine::Resource