#include "PBRMaterialResource.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include <cstring>
namespace MEngine::Resource
{
void PBRMaterialResource::InitRHI()
{
    // create uniform buffer
    MaterialResource::InitRHI();
    RHIBufferDesc bufferDesc{};
    bufferDesc.size = sizeof(PBRProperties);
    bufferDesc.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mPBRPropertiesUBO[i] = RHIHandler<RHIBuffer>(new RHIBuffer(bufferDesc, allocCreateInfo));
    }
}
void PBRMaterialResource::ReleaseRHI()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mPBRPropertiesUBO[i].SafeRelease();
    }
    MaterialResource::ReleaseRHI();
}
void PBRMaterialResource::UpdateDescriptorSet(int frameIndex)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mMaterial);
    if (frameIndex >= mDescriptorSetHandlers.size())
    {
        LogError("Descriptor set index out of range");
        return;
    }
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets{};
    // parameters
    memcpy(mPBRPropertiesUBO[frameIndex]->GetAllocationInfo().pMappedData, &pbrMaterial->GetProperties(),
           sizeof(PBRProperties));
    auto descriptorSet = mDescriptorSetHandlers[frameIndex]->GetDescriptorSets().front();
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.setBuffer(mPBRPropertiesUBO[frameIndex]->GetBuffer()).setOffset(0).setRange(sizeof(PBRProperties));
    writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                      .setDstSet(descriptorSet)
                                      .setDstBinding(0)
                                      .setDstArrayElement(0)
                                      .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                      .setBufferInfo(bufferInfo));
    // albedo
    vk::DescriptorImageInfo albedoImageInfo{};
    auto albedoTexture = pbrMaterial->GetTextures().Albedo;
    albedoTexture->GetResource()->InitResource();
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
    normalTexture->GetResource()->InitResource();
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
    armTexture->GetResource()->InitResource();
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
    emissiveTexture->GetResource()->InitResource();
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
}
} // namespace MEngine::Resource