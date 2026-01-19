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
        auto sceneResource = scene->GetResourceAs<SceneResource>();
        auto mappedData = static_cast<uint8_t *>(sceneResource->GetStagingBufferAllocationInfo().pMappedData);
        auto bufferSize = sizeof(SceneParam);
        std::memcpy(mappedData, &scene->GetSceneParams(), bufferSize);
        vk::BufferCopy2 copyRegion{};
        copyRegion.setSize(sizeof(SceneParam)).setSrcOffset(0).setDstOffset(0);
        vk::CopyBufferInfo2 copyBufferInfo{};
        copyBufferInfo.setSrcBuffer(sceneResource->GetStagingBuffer())
            .setDstBuffer(sceneResource->GetBuffer())
            .setRegions(copyRegion);
        commandBuffer.copyBuffer2(copyBufferInfo);
    }
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    for (auto &scene : sceneToUpdate)
    {
        auto sceneResource = scene->GetResourceAs<SceneResource>();
        vk::BufferMemoryBarrier2 bufferBarrier{};
        bufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(sceneResource->GetBuffer())
            .setOffset(0)
            .setSize(sizeof(SceneParam));
        bufferBarriers.push_back(bufferBarrier);
    }
    vk::DependencyInfo sceneDepInfo{};
    sceneDepInfo.setBufferMemoryBarriers(bufferBarriers);
    commandBuffer.pipelineBarrier2(sceneDepInfo);
    commandBuffer.end();
}
} // namespace MEngine::Resource