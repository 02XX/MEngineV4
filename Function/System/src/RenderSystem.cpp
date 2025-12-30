#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "ECS.hpp"
#include "GraphicPipeline.hpp"
#include "IndexResource.hpp"
#include "Logger.hpp"
#include "MaterialComponent.hpp"
#include "Math.hpp"
#include "MeshComponent.hpp"
#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
#include "RHIDescriptorSet.hpp"
#include "SceneResource.hpp"
#include "StaticMeshResource.hpp"
#include "Texture.hpp"
#include "TextureRenderTarget2D.hpp"
#include "TextureRenderTarget2DResource.hpp"
#include "TransformComponent.hpp"
#include "VertexResource.hpp"
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace MEngine::Function
{
RenderSystem::RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<AssetManager> assetManager)
    : System(scene, assetManager)
{
}
RenderSystem::~RenderSystem()
{
}
void RenderSystem::Init()
{
    mScene->GetResourceAs<SceneResource>()->InitResource();
}
void RenderSystem::Update(double deltaTime)
{
    PrepareRenderQueues();
    PrepareGlobalResources();
    PrePare();
    RenderGBuffer();
    RenderLighting();
    End();
    mCurrentFrameBufferIndex = (mCurrentFrameBufferIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
void RenderSystem::Shutdown()
{
}
void RenderSystem::PrepareGlobalResources()
{
    auto mainCameraEntity = mScene->GetMainCameraEntity();
}
void RenderSystem::PrepareRenderQueues()
{
    mRenderQueues.clear();
    auto entities = mScene->GetRegistry()->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (const auto &entity : entities)
    {
        auto &materialComponent = entities.get<MaterialComponent>(entity);
        auto pipeline = materialComponent.Material->GetPipeline();
        pipeline->GetResource()->InitResource();
        materialComponent.Material->GetResource()->InitResource();
        auto &meshComponent = entities.get<MeshComponent>(entity);
        meshComponent.Mesh->GetResource()->InitResource();
        auto &transformComponent = entities.get<TransformComponent>(entity);
        mRenderQueues[pipeline->GetName()].push_back(entity);
    }
}
void RenderSystem::PrePare()
{
    auto &currentFrameResource = mFrameResources[mCurrentFrameBufferIndex];
    auto commandBuffer = currentFrameResource.CommandBuffer.get();
    auto result = RHIContext::Instance().GetDevice().waitForFences({currentFrameResource.InFlightFence.get()}, vk::True,
                                                                   std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed wait InFlightFences");
        return;
    }
    RHIContext::Instance().GetDevice().resetFences({currentFrameResource.InFlightFence.get()});
    mScene->GetResourceAs<SceneResource>()->UpdateSceneUBO(mCurrentFrameBufferIndex);
    commandBuffer.begin(vk::CommandBufferBeginInfo{});
}
void RenderSystem::RenderGBuffer()
{
    auto &currentFrameResource = mFrameResources[mCurrentFrameBufferIndex];
    auto commandBuffer = currentFrameResource.CommandBuffer.get();
    std::vector<vk::RenderingAttachmentInfo> colorAttachments{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource.ColorClearValue)
            .setImageView(currentFrameResource.ColorTextures->GetResourceAs<TextureRenderTarget2DResource>()
                              ->GetTextureView()
                              ->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Albedo
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource.AlbedoClearValue)
            .setImageView(currentFrameResource.AlbedoTextures->GetResourceAs<TextureRenderTarget2DResource>()
                              ->GetTextureView()
                              ->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Normal
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource.NormalClearValue)
            .setImageView(currentFrameResource.NormalTextures->GetResourceAs<TextureRenderTarget2DResource>()
                              ->GetTextureView()
                              ->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // ARM
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource.ARMClearValue)
            .setImageView(currentFrameResource.ARMTextures->GetResourceAs<TextureRenderTarget2DResource>()
                              ->GetTextureView()
                              ->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Position
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource.PositionClearValue)
            .setImageView(currentFrameResource.PositionTextures->GetResourceAs<TextureRenderTarget2DResource>()
                              ->GetTextureView()
                              ->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Emissive
        vk::RenderingAttachmentInfo()
            .setClearValue(currentFrameResource.PositionClearValue)
            .setImageView(currentFrameResource.PositionTextures->GetResourceAs<TextureRenderTarget2DResource>()
                              ->GetTextureView()
                              ->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
    };
    vk::RenderingAttachmentInfo depthStencilAttachment{};
    depthStencilAttachment.setClearValue(currentFrameResource.DepthClearValue)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setImageView(currentFrameResource.DepthStencilTextures->GetResourceAs<TextureRenderTarget2DResource>()
                          ->GetTextureView()
                          ->GetImageView());
    vk::RenderingInfo renderingInfo{};
    renderingInfo
        .setRenderArea(vk::Rect2D{{0, 0}, {currentFrameResource.Extent.width, currentFrameResource.Extent.height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachments)
        .setPDepthAttachment(&depthStencilAttachment);
    commandBuffer.beginRendering(renderingInfo);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(currentFrameResource.Extent.height)
        .setWidth(static_cast<float>(currentFrameResource.Extent.width))
        .setHeight(-static_cast<float>(currentFrameResource.Extent.height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    commandBuffer.setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({currentFrameResource.Extent.width, currentFrameResource.Extent.height});
    commandBuffer.setScissor(0, {scissor});
    if (mRenderQueues.contains("GBufferPipeline"))
    {
        auto &entities = mRenderQueues.at("GBufferPipeline");
        auto pipeline = mAssetManager->GetByName<GraphicPipeline>("GBufferPipeline");
        auto rhiPipeline = pipeline->GetResourceAs<GraphicPipelineResource>()->GetGraphicPipeline()->GetPipeline();
        auto rhiPipelineLayout =
            pipeline->GetResourceAs<GraphicPipelineResource>()->GetGraphicPipeline()->GetPipelineLayout();
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, rhiPipeline);
        auto currentFrameDescriptorSet =
            pipeline->GetResourceAs<GraphicPipelineResource>()->GetPerFrameDescriptorSet(mCurrentFrameBufferIndex);
        mScene->GetResourceAs<SceneResource>()->UpdateSceneUBO(mCurrentFrameBufferIndex);
        // commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, rhiPipelineLayout, 0,
        //                                  currentFrameDescriptorSet->GetDescriptorSets().front(), {});
        for (const auto &entity : entities)
        {
            auto &materialComponent = mScene->GetRegistry()->get<MaterialComponent>(entity);
            auto &meshComponent = mScene->GetRegistry()->get<MeshComponent>(entity);
            auto &transformComponent = mScene->GetRegistry()->get<TransformComponent>(entity);
            auto materialDescriptorSet = materialComponent.Material->GetResourceAs<MaterialResource>()
                                             ->GetDescriptorSet(mCurrentFrameBufferIndex)
                                             ->GetDescriptorSets()
                                             .front();
            auto material = materialComponent.Material->GetResourceAs<MaterialResource>();
            material->UpdateDescriptorSet(mCurrentFrameBufferIndex);
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, rhiPipelineLayout, 1,
                                             materialDescriptorSet, {});

            commandBuffer.pushConstants(rhiPipelineLayout,
                                        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
                                        sizeof(Matrix4), &transformComponent.modelMatrix);
            auto staticMeshResource = meshComponent.Mesh->GetResourceAs<StaticMeshResource>();
            auto vertexBufferResource = staticMeshResource->GetVertexResource();
            auto indexBufferResource = staticMeshResource->GetIndexResource();
            auto vertexBuffer = vertexBufferResource->GetVertexBuffer();
            auto indexBuffer = indexBufferResource->GetIndexBuffer();
            commandBuffer.bindVertexBuffers(0, vertexBuffer->GetBuffer(), {0});
            commandBuffer.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
            commandBuffer.drawIndexed(meshComponent.Mesh->GetIndices().size(), 1, 0, 0, 0);
        }
    }
    commandBuffer.endRendering();
}
void RenderSystem::RenderLighting()
{
}
void RenderSystem::End()
{
    auto &currentFrameResource = mFrameResources[mCurrentFrameBufferIndex];
    auto commandBuffer = currentFrameResource.CommandBuffer.get();
    mRenderCallbacks(currentFrameResource);
    commandBuffer.end();
    vk::SubmitInfo submitinfo;
    std::vector<vk::PipelineStageFlags> waitStages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitinfo.setCommandBuffers(commandBuffer)
        .setSignalSemaphores(currentFrameResource.RenderFinishedSemaphore.get())
        .setWaitSemaphores(currentFrameResource.ImageAvailableSemaphore.get())
        .setWaitDstStageMask(waitStages);
    RHIContext::Instance().GetGraphicsQueue().submit({submitinfo}, {currentFrameResource.InFlightFence.get()});
}
void RenderSystem::UpdateGlobalResources(vk::DescriptorSet globalDescriptorSet)
{
}
} // namespace MEngine::Function