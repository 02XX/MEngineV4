#include "RenderSystem.hpp"
#include "Logger.hpp"
#include "RenderResource.hpp"

namespace MEngine::Function
{
RenderSystem::RenderSystem(std::shared_ptr<Context> context) : System(), mContext(context)
{
}
RenderSystem::~RenderSystem()
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (mFrameResources[i])
        {
            mFrameResources[i]->ReleaseResource(mContext);
            mFrameResources[i].reset();
        }
    }
}
void RenderSystem::Init()
{
    // mScene->GetResourceAs<SceneResource>()->InitResource();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        mFrameResources[i] = std::make_unique<FrameResource>(vk::Extent3D{800, 600, 1});
        mFrameResources[i]->InitResource(mContext);
    }
}
void RenderSystem::Update(double deltaTime)
{
    // PrepareRenderQueues();
    // PrepareGlobalResources();
    Prepare();
    RenderGBuffer();
    RenderLighting();
    End();
    Submit();
    mCurrentFrameBufferIndex = (mCurrentFrameBufferIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
void RenderSystem::Shutdown()
{
}
// void RenderSystem::PrepareGlobalResources()
// {
//     auto mainCameraEntity = mScene->GetMainCameraEntity();
// }
// void RenderSystem::PrepareRenderQueues()
// {
//     mRenderQueues.clear();
//     auto entities = mScene->GetRegistry()->view<TransformComponent, MeshComponent, MaterialComponent>();
//     for (const auto &entity : entities)
//     {
//         auto &materialComponent = entities.get<MaterialComponent>(entity);
//         auto pipeline = materialComponent.Material->GetPipeline();
//         pipeline->GetResource()->InitResource();
//         materialComponent.Material->GetResource()->InitResource();
//         auto &meshComponent = entities.get<MeshComponent>(entity);
//         meshComponent.Mesh->GetResource()->InitResource();
//         auto &transformComponent = entities.get<TransformComponent>(entity);
//         mRenderQueues[pipeline->GetName()].push_back(entity);
//     }
// }
void RenderSystem::Prepare()
{
    auto device = mContext->Device.get();
    auto currentFrameResource = mFrameResources[mCurrentFrameBufferIndex].get();
    auto currentGraphicCommandBuffer = currentFrameResource->GraphicsCommandBuffer;
    auto result = device.waitForFences({currentFrameResource->InFlightFence.get()}, vk::True,
                                       1000000000); // 1 second
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed wait InFlightFences");
        return;
    }
    device.resetFences({currentFrameResource->InFlightFence.get()});
    // mContext->GetResourceAs<SceneResource>()->UpdateSceneUBO(mCurrentFrameBufferIndex);
    currentGraphicCommandBuffer.begin(vk::CommandBufferBeginInfo{});
}
void RenderSystem::RenderGBuffer()
{
    auto currentFrameResource = mFrameResources[mCurrentFrameBufferIndex].get();
    auto currentGraphicCommandBuffer = currentFrameResource->GraphicsCommandBuffer;
    std::vector<vk::RenderingAttachmentInfo> colorAttachments{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource->ColorClearValue)
            .setImageView(currentFrameResource->ColorTexture->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Albedo
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource->AlbedoClearValue)
            .setImageView(currentFrameResource->AlbedoTexture->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Normal
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource->NormalClearValue)
            .setImageView(currentFrameResource->NormalTexture->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // ARM
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource->ARMClearValue)
            .setImageView(currentFrameResource->ARMTexture->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Position
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource->PositionClearValue)
            .setImageView(currentFrameResource->PositionTexture->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Emissive
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource->EmissiveClearValue)
            .setImageView(currentFrameResource->EmissiveTexture->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
    };
    vk::RenderingAttachmentInfo depthStencilAttachment{};
    depthStencilAttachment.setClearValue(currentFrameResource->DepthClearValue)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setImageView(currentFrameResource->DepthStencilTexture->GetImageView());
    vk::RenderingInfo renderingInfo{};
    renderingInfo
        .setRenderArea(vk::Rect2D{{0, 0}, {currentFrameResource->Extent.width, currentFrameResource->Extent.height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachments)
        .setPDepthAttachment(&depthStencilAttachment);
    currentGraphicCommandBuffer.beginRendering(renderingInfo);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(currentFrameResource->Extent.height)
        .setWidth(static_cast<float>(currentFrameResource->Extent.width))
        .setHeight(-static_cast<float>(currentFrameResource->Extent.height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    currentGraphicCommandBuffer.setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({currentFrameResource->Extent.width, currentFrameResource->Extent.height});
    currentGraphicCommandBuffer.setScissor(0, {scissor});
    // if (mRenderQueues.contains("GBufferPipeline"))
    // {
    //     auto &entities = mRenderQueues.at("GBufferPipeline");
    //     auto pipeline = mAssetManager->GetByName<GraphicPipeline>("GBufferPipeline");
    //     auto rhiPipeline = pipeline->GetResourceAs<GraphicPipelineResource>()->GetGraphicPipeline()->GetPipeline();
    //     auto rhiPipelineLayout =
    //         pipeline->GetResourceAs<GraphicPipelineResource>()->GetGraphicPipeline()->GetPipelineLayout();
    //     commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, rhiPipeline);
    //     auto currentFrameDescriptorSet =
    //         pipeline->GetResourceAs<GraphicPipelineResource>()->GetPerFrameDescriptorSet(mCurrentFrameBufferIndex);
    //     mScene->GetResourceAs<SceneResource>()->UpdateSceneUBO(mCurrentFrameBufferIndex);
    //     // commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, rhiPipelineLayout, 0,
    //     //                                  currentFrameDescriptorSet->GetDescriptorSets().front(), {});
    //     for (const auto &entity : entities)
    //     {
    //         auto &materialComponent = mScene->GetRegistry()->get<MaterialComponent>(entity);
    //         auto &meshComponent = mScene->GetRegistry()->get<MeshComponent>(entity);
    //         auto &transformComponent = mScene->GetRegistry()->get<TransformComponent>(entity);
    //         auto materialDescriptorSet = materialComponent.Material->GetResourceAs<MaterialResource>()
    //                                          ->GetDescriptorSet(mCurrentFrameBufferIndex)
    //                                          ->GetDescriptorSets()
    //                                          .front();
    //         auto material = materialComponent.Material->GetResourceAs<MaterialResource>();
    //         material->UpdateDescriptorSet(mCurrentFrameBufferIndex);
    //         commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, rhiPipelineLayout, 1,
    //                                          materialDescriptorSet, {});

    //         commandBuffer.pushConstants(rhiPipelineLayout,
    //                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
    //                                     sizeof(Matrix4), &transformComponent.modelMatrix);
    //         auto staticMeshResource = meshComponent.Mesh->GetResourceAs<StaticMeshResource>();
    //         auto vertexBufferResource = staticMeshResource->GetVertexResource();
    //         auto indexBufferResource = staticMeshResource->GetIndexResource();
    //         auto vertexBuffer = vertexBufferResource->GetVertexBuffer();
    //         auto indexBuffer = indexBufferResource->GetIndexBuffer();
    //         commandBuffer.bindVertexBuffers(0, vertexBuffer->GetBuffer(), {0});
    //         commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
    //         commandBuffer.drawIndexed(meshComponent.Mesh->GetIndices().size(), 1, 0, 0, 0);
    //     }
    // }
    currentGraphicCommandBuffer.endRendering();
}
void RenderSystem::RenderLighting()
{
}
void RenderSystem::End()
{
    auto device = mContext->Device.get();
    auto currentFrameResource = mFrameResources[mCurrentFrameBufferIndex].get();
    auto currentGraphicCommandBuffer = currentFrameResource->GraphicsCommandBuffer;
    currentGraphicCommandBuffer.end();
    vk::SubmitInfo submitinfo;
    std::vector<vk::PipelineStageFlags> waitStages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitinfo.setCommandBuffers(currentGraphicCommandBuffer)
        .setSignalSemaphores(currentFrameResource->RenderFinishedSemaphore.get())
        .setWaitSemaphores(currentFrameResource->ImageAvailableSemaphore.get())
        .setWaitDstStageMask(waitStages);
    mSubmitQueue.Push([submitinfo, currentFrameResource](vk::Queue queue) {
        queue.submit(submitinfo, currentFrameResource->InFlightFence.get());
    });
}
void RenderSystem::Submit()
{
    auto graphicsQueue = mContext->GraphicsQueue;
    std::function<void(vk::Queue)> item;
    while (mSubmitQueue.TryPop(item))
    {
        item(graphicsQueue);
    }
}
} // namespace MEngine::Function