#include "SceneManager.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "MaterialComponent.hpp"
#include "MeshComponent.hpp"
#include "Scene.hpp"
#include "TransformComponent.hpp"

namespace MEngine::Resource
{
void SceneManager::CreateDefault()
{
    // auto sampleScene = CreateSample();
    // sampleScene->SetID(mDefaultScenes.at(DefaultScene::Sample));
    // Add(sampleScene);
}
std::shared_ptr<Scene> SceneManager::CreateSample()
{
    auto scene = std::make_shared<Scene>(DefaultScene::Sample);
    return scene;
}
void SceneManager::UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                             vk::CommandBufferInheritanceInfo *inheritanceInfo)
{
    std::unordered_set<std::shared_ptr<Scene>> sceneToUpdate{};
    sceneToUpdate.reserve(mPendingUpdateAssets.Size());
    std::shared_ptr<Scene> scene{};
    while (mPendingUpdateAssets.TryPop(scene))
    {
        sceneToUpdate.insert(scene);
    }
    vk::CommandBufferBeginInfo sceneBeginInfo{};
    sceneBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(inheritanceInfo);
    commandBuffer.begin(sceneBeginInfo);
    for (auto &scene : sceneToUpdate)
    {
        // UBO
        auto sceneResource = scene->GetResourceAs<SceneResource>();
        auto mappedData = static_cast<uint8_t *>(sceneResource->mUBOStagingBufferAllocationInfo.pMappedData);
        auto bufferSize = sizeof(SceneParam);
        std::memcpy(mappedData, &scene->mSceneParams, bufferSize);
        vk::BufferCopy2 uboCopyRegion{};
        uboCopyRegion.setSize(sizeof(SceneParam)).setSrcOffset(0).setDstOffset(0);
        vk::CopyBufferInfo2 uboCopyBufferInfo{};
        uboCopyBufferInfo.setSrcBuffer(sceneResource->mUBOStagingBuffer)
            .setDstBuffer(sceneResource->mUBOBuffer)
            .setRegions(uboCopyRegion);
        commandBuffer.copyBuffer2(uboCopyBufferInfo);
        // SSBO
        mappedData = static_cast<uint8_t *>(sceneResource->mSSBOStagingBufferAllocationInfo.pMappedData);
        auto ssboBufferSize = sizeof(LightParam) * MAX_LIGHTS;
        for (size_t i = 0; i < MAX_LIGHTS; ++i)
        {
            std::memcpy(mappedData + i * sizeof(LightParam), &scene->mLightParams[i], sizeof(LightParam));
        }
        vk::BufferCopy2 ssboCopyRegion{};
        ssboCopyRegion.setSize(ssboBufferSize).setSrcOffset(0).setDstOffset(0);
        vk::CopyBufferInfo2 ssboCopyBufferInfo{};
        ssboCopyBufferInfo.setSrcBuffer(sceneResource->mSSBOStagingBuffer)
            .setDstBuffer(sceneResource->mSSBOBuffer)
            .setRegions(ssboCopyRegion);
        commandBuffer.copyBuffer2(ssboCopyBufferInfo);
    }
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &scene : sceneToUpdate)
    {
        // UBO
        auto sceneResource = scene->GetResourceAs<SceneResource>();
        vk::BufferMemoryBarrier2 uboBufferBarrier{};
        uboBufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(sceneResource->mUBOBuffer)
            .setOffset(0)
            .setSize(sizeof(SceneParam));
        bufferBarriers.push_back(uboBufferBarrier);
        // SSBO
        vk::BufferMemoryBarrier2 ssboBufferBarrier{};
        ssboBufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(sceneResource->mSSBOBuffer)
            .setOffset(0)
            .setSize(sizeof(LightParam) * MAX_LIGHTS);
        bufferBarriers.push_back(ssboBufferBarrier);
    }
    vk::DependencyInfo sceneDepInfo{};
    sceneDepInfo.setBufferMemoryBarriers(bufferBarriers);
    commandBuffer.pipelineBarrier2(sceneDepInfo);
    commandBuffer.end();
}
} // namespace MEngine::Resource