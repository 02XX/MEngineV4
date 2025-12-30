#include "SceneResource.hpp"
#include "CameraComponent.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
namespace MEngine::Resource
{
void SceneResource::InitRHI()
{
    RHIBufferDesc globalBufferDesc{};
    globalBufferDesc.setSize(sizeof(CameraParameters))
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mSceneUniformBufferHandlers[i] = RHIHandler<RHIBuffer>(new RHIBuffer(globalBufferDesc, allocCreateInfo));
    }
    // descriptor set
    std::vector<vk::DescriptorSetLayoutBinding> bindings{{
        // set:0
        // Binding: 0 VP (View Projection Matrix)
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Light
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Environment Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 Irradiance Map
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 BRDF LUT
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    }};
    auto descriptorSetLayout = RHIContext::Instance().GetDevice().createDescriptorSetLayoutUnique(
        vk::DescriptorSetLayoutCreateInfo{}.setBindings(bindings));
    RHIDescriptorSetDesc desc{};
    desc.SetLayouts = {descriptorSetLayout.get()};
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mSceneDescriptorSets[i] = RHIHandler<RHIDescriptorSet>(new RHIDescriptorSet(desc));
    }
}
void SceneResource::ReleaseRHI()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mSceneUniformBufferHandlers[i].SafeRelease();
        mSceneDescriptorSets[i].SafeRelease();
    }
}
void SceneResource::UpdateSceneUBO(int frameIndex)
{
    // 获取主摄像机实体
    auto cameraEntity = mScene->GetMainCameraEntity();
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets{};
    if (cameraEntity != NullEntity)
    {
        auto &cameraComponent = mScene->GetRegistry()->get<CameraComponent>(cameraEntity);
        auto &transformComponent = mScene->GetRegistry()->get<TransformComponent>(cameraEntity);
        CameraParameters cameraParams{};
        cameraParams.Position = transformComponent.worldPosition;
        cameraParams.Direction = transformComponent.worldRotation * Vector3(0.0f, 0.0f, -1.0f);
        cameraParams.ViewMatrix = cameraComponent.viewMatrix;
        cameraParams.ProjectionMatrix = cameraComponent.projectionMatrix;
        memcpy(mSceneUniformBufferHandlers[frameIndex]->GetAllocationInfo().pMappedData, &cameraParams,
               sizeof(CameraParameters));
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.setBuffer(mSceneUniformBufferHandlers[frameIndex]->GetBuffer())
            .setOffset(0)
            .setRange(sizeof(CameraParameters));
        writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
                                          .setDstSet(mSceneDescriptorSets[frameIndex]->GetDescriptorSets().front())
                                          .setDstBinding(0)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                          .setBufferInfo(bufferInfo));
    }
    else
    {
        LogWarn("Camera entity is null, cannot update global resources");
    }
}
} // namespace MEngine::Resource
