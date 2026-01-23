#include "SceneManager.hpp"
#include "Scene.hpp"

namespace MEngine::Resource
{
SceneManager::SceneManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager,
                           std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<MeshManager> meshManager)
    : Manager<Scene, SceneResource>(context), mPipelineManager(pipelineManager), mMaterialManager(materialManager),
      mMeshManager(meshManager)
{
}
std::shared_ptr<Scene> SceneManager::CreateSample()
{
    auto scene = std::make_shared<Scene>(DefaultScene::Sample);
    return scene;
}
void SceneManager::ProcessPendingInitResources(RenderContext renderContext)
{
    auto sceneResourcesToInit = ToVector(mPendingInitResources);
    for (auto &sceneResource : sceneResourcesToInit)
    {
        sceneResource->InitResource(renderContext.Context);
    }
}
void SceneManager::ProcessPendingUpdateResources(RenderContext renderContext)
{
    auto sceneResourcesToUpdate = ToVector(mPendingUpdateResources);
    std::vector<vk::BufferMemoryBarrier2> bufferBarriers{};
    bufferBarriers.reserve(sceneResourcesToUpdate.size() * 2);
    for (auto sceneResource : sceneResourcesToUpdate)
    {
        sceneResource->Upload();
        vk::BufferCopy2 uboCopyRegion{}, ssboCopyRegion{};
        uboCopyRegion.setSize(sizeof(SceneParam)).setSrcOffset(0).setDstOffset(0);
        vk::CopyBufferInfo2 uboCopyBufferInfo{}, ssboCopyBufferInfo{};
        uboCopyBufferInfo.setSrcBuffer(sceneResource->mUBOStagingBuffer)
            .setDstBuffer(sceneResource->mUBOBuffer)
            .setRegions(uboCopyRegion);
        renderContext.CommandBuffer.copyBuffer2(uboCopyBufferInfo);

        ssboCopyRegion.setSize(sizeof(LightParam) * MAX_LIGHTS).setSrcOffset(0).setDstOffset(0);
        ssboCopyBufferInfo.setSrcBuffer(sceneResource->mSSBOStagingBuffer)
            .setDstBuffer(sceneResource->mSSBOBuffer)
            .setRegions(ssboCopyRegion);
        renderContext.CommandBuffer.copyBuffer2(ssboCopyBufferInfo);

        vk::BufferMemoryBarrier2 uboBufferBarrier{}, ssboBufferBarrier{};
        uboBufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(sceneResource->mUBOBuffer)
            .setOffset(0)
            .setSize(sizeof(SceneParam));
        ssboBufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
            .setBuffer(sceneResource->mSSBOBuffer)
            .setOffset(0)
            .setSize(sizeof(LightParam) * MAX_LIGHTS);

        bufferBarriers.push_back(uboBufferBarrier);
        bufferBarriers.push_back(ssboBufferBarrier);
    }
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setBufferMemoryBarriers(bufferBarriers);
    renderContext.CommandBuffer.pipelineBarrier2(dependencyInfo);
}
} // namespace MEngine::Resource