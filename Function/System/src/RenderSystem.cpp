#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "GraphicPipeline.hpp"
#include "IndexResource.hpp"
#include "Logger.hpp"
#include "MaterialComponent.hpp"
#include "Math.hpp"
#include "MeshComponent.hpp"
#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
#include "RHIDescriptorSet.hpp"
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
    RHIBufferDesc globalBufferDesc{};
    globalBufferDesc.setSize(sizeof(CameraParameters) + sizeof(LightParameters) * 16)
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    // mGlobalUniformBufferHandler = RHIHandler<RHIBuffer>(new RHIBuffer(globalBufferDesc, allocCreateInfo));
}
RenderSystem::~RenderSystem()
{
}
void RenderSystem::Init()
{
}
void RenderSystem::Update(double deltaTime)
{
    if (mFrameBuffers.empty())
    {
        LogError("FrameBuffers are not set!");
        return;
    }
    if (mFrameBuffers[mCurrentFrameBufferIndex]->GetColorAttachments().empty() ||
        !mFrameBuffers[mCurrentFrameBufferIndex]->GetDepthStencilAttachment())
    {
        LogError("FrameBuffer attachments are not set!");
        return;
    }
    PrepareRenderQueues();
    PrepareGlobalResources();
    PrePare();
    RenderGBuffer();
    RenderLighting();
    End();
    mCurrentFrameBufferIndex = (mCurrentFrameBufferIndex + 1) % mFrameBuffers.size();
}
void RenderSystem::Shutdown()
{
}

void RenderSystem::SetFrameBuffers(std::vector<std::shared_ptr<FrameBuffer>> frameBuffers)
{
    if (frameBuffers.empty())
    {
        LogError("FrameBuffers is empty!");
        return;
    }
    RHIContext::Instance().GetDevice().waitIdle();
    mFrameBuffers = std::move(frameBuffers);
    for (const auto &frameBuffer : mFrameBuffers)
    {
        for (const auto &colorAttachment : frameBuffer->GetColorAttachments())
        {
            colorAttachment->GetResource()->InitResource();
        }
        frameBuffer->GetDepthStencilAttachment()->GetResource()->InitResource();
        auto commandBuffer = RHIContext::Instance().GetGraphicsCommandBuffer();
        mCommandBuffers.push_back(std::move(commandBuffer));
        auto semaphores = RHIContext::Instance().GetDevice().createSemaphoreUnique({});
        mImageAvailableSemaphores.push_back(std::move(semaphores));
        auto fence = RHIContext::Instance().GetDevice().createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
        mInFlightFences.push_back(std::move(fence));
    }
}
void RenderSystem::PrepareGlobalResources()
{
    auto entities = mScene->GetRegistry()->view<TransformComponent, CameraComponent>();
    for (const auto &entity : entities)
    {
        auto &cameraComponent = entities.get<CameraComponent>(entity);
        auto &transformComponent = entities.get<TransformComponent>(entity);
        if (cameraComponent.isEditorCamera || cameraComponent.isMainCamera)
        {
            mCameraEntity = entity;
            break;
        }
    }
}
void RenderSystem::PrepareRenderQueues()
{
    mRenderQueues.clear();
    auto entities = mScene->GetRegistry()->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (const auto &entity : entities)
    {
        auto &materialComponent = entities.get<MaterialComponent>(entity);
        materialComponent.Material->GetResource()->InitResource();
        auto &meshComponent = entities.get<MeshComponent>(entity);
        meshComponent.Mesh->GetResource()->InitResource();
        auto &transformComponent = entities.get<TransformComponent>(entity);
        auto pipeline = materialComponent.Material->GetPipeline();
        pipeline->GetResource()->InitResource();
        mRenderQueues[pipeline->GetName()].push_back(entity);
    }
}
void RenderSystem::PrePare()
{
    auto result = RHIContext::Instance().GetDevice().waitForFences({mInFlightFences[mCurrentFrameBufferIndex].get()},
                                                                   vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed wait InFlightFences");
        return;
    }
    RHIContext::Instance().GetDevice().resetFences({mInFlightFences[mCurrentFrameBufferIndex].get()});
}
void RenderSystem::RenderGBuffer()
{
    mCommandBuffers[mCurrentFrameBufferIndex]->begin(vk::CommandBufferBeginInfo{});
    std::vector<vk::RenderingAttachmentInfo> colorAttachments{};
    vk::RenderingAttachmentInfo depthStencilAttachment{};
    colorAttachments.reserve(mFrameBuffers[mCurrentFrameBufferIndex]->GetColorAttachments().size());
    for (const auto &colorAttachment : mFrameBuffers[mCurrentFrameBufferIndex]->GetColorAttachments())
    {
        vk::RenderingAttachmentInfo colorAttachmentInfo{};
        colorAttachmentInfo
            .setImageView(
                colorAttachment->GetResourceAs<TextureRenderTarget2DResource>()->GetTextureView()->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f})));
        colorAttachments.push_back(colorAttachmentInfo);
    }
    auto depthStencil = mFrameBuffers[mCurrentFrameBufferIndex]->GetDepthStencilAttachment();
    depthStencilAttachment
        .setImageView(depthStencil->GetResourceAs<TextureRenderTarget2DResource>()->GetTextureView()->GetImageView())
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setClearValue(vk::ClearValue(vk::ClearDepthStencilValue{1.0f, 0}));
    vk::RenderingInfo renderingInfo{};
    auto width =
        depthStencil->GetResourceAs<TextureRenderTarget2DResource>()->GetTexture()->GetTextureDesc().extent.width;
    auto height =
        depthStencil->GetResourceAs<TextureRenderTarget2DResource>()->GetTexture()->GetTextureDesc().extent.height;
    renderingInfo.setRenderArea(vk::Rect2D{{0, 0}, {width, height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachments)
        .setPDepthAttachment(&depthStencilAttachment);
    mCommandBuffers[mCurrentFrameBufferIndex]->beginRendering(renderingInfo);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(height)
        .setWidth(static_cast<float>(width))
        .setHeight(-static_cast<float>(height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    mCommandBuffers[mCurrentFrameBufferIndex]->setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({width, height});
    mCommandBuffers[mCurrentFrameBufferIndex]->setScissor(0, {scissor});
    if (mRenderQueues.contains("GBufferPipeline"))
    {
        auto &entities = mRenderQueues.at("GBufferPipeline");
        auto pipeline = mAssetManager->GetByName<GraphicPipeline>("GBufferPipeline");
        auto rhiPipeline = pipeline->GetResourceAs<GraphicPipelineResource>()->GetGraphicPipeline()->GetPipeline();
        auto rhiPipelineLayout =
            pipeline->GetResourceAs<GraphicPipelineResource>()->GetGraphicPipeline()->GetPipelineLayout();
        mCommandBuffers[mCurrentFrameBufferIndex]->bindPipeline(vk::PipelineBindPoint::eGraphics, rhiPipeline);
        for (const auto &entity : entities)
        {
            auto &materialComponent = mScene->GetRegistry()->get<MaterialComponent>(entity);
            auto &meshComponent = mScene->GetRegistry()->get<MeshComponent>(entity);
            auto &transformComponent = mScene->GetRegistry()->get<TransformComponent>(entity);
            // auto materialDescriptorSet =
            //     materialComponent.Material->GetResourceAs<MaterialResource>()->GetDescriptorSet()->GetDescriptorSets();
            // // 更新数据
            // UpdateGlobalResources(materialDescriptorSet[0]);
            // mCommandBuffers[mCurrentFrameBufferIndex]->bindDescriptorSets(
            //     vk::PipelineBindPoint::eGraphics, rhiPipelineLayout, 0, materialDescriptorSet, {});
            // mCommandBuffers[mCurrentFrameBufferIndex]->bindDescriptorSets(
            //     vk::PipelineBindPoint::eGraphics, rhiPipelineLayout, 1, materialDescriptorSet, {});
            // mCommandBuffers[mCurrentFrameBufferIndex]->pushConstants(
            //     rhiPipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
            //     sizeof(Matrix4), &transformComponent.modelMatrix);
            auto staticMeshResource = meshComponent.Mesh->GetResourceAs<StaticMeshResource>();
            auto vertexBufferResource = staticMeshResource->GetVertexResource();
            auto indexBufferResource = staticMeshResource->GetIndexResource();
            auto vertexBuffer = vertexBufferResource->GetVertexBuffer();
            auto indexBuffer = indexBufferResource->GetIndexBuffer();
            mCommandBuffers[mCurrentFrameBufferIndex]->bindVertexBuffers(0, vertexBuffer->GetBuffer(), {0});
            mCommandBuffers[mCurrentFrameBufferIndex]->bindIndexBuffer(indexBuffer->GetBuffer(), 0,
                                                                       vk::IndexType::eUint32);
            mCommandBuffers[mCurrentFrameBufferIndex]->drawIndexed(meshComponent.Mesh->GetIndices().size(), 1, 0, 0, 0);
        }
    }
    mCommandBuffers[mCurrentFrameBufferIndex]->endRendering();
    mCommandBuffers[mCurrentFrameBufferIndex]->end();
}
void RenderSystem::RenderLighting()
{
}
void RenderSystem::End()
{
    vk::SubmitInfo submitinfo;
    submitinfo.setCommandBuffers(mCommandBuffers[mCurrentFrameBufferIndex].get());
    // .setSignalSemaphores(mImageAvailableSemaphores[mCurrentFrameBufferIndex].get());
    RHIContext::Instance().GetGraphicsQueue().submit({submitinfo}, {mInFlightFences[mCurrentFrameBufferIndex].get()});
}
void RenderSystem::UpdateGlobalResources(vk::DescriptorSet globalDescriptorSet)
{
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets{};
    auto &cameraComponent = mScene->GetRegistry()->get<CameraComponent>(mCameraEntity);
    auto &transformComponent = mScene->GetRegistry()->get<TransformComponent>(mCameraEntity);
    CameraParameters cameraParams{};
    cameraParams.Position = transformComponent.worldPosition;
    cameraParams.Direction = transformComponent.worldRotation * Vector3(0.0f, 0.0f, -1.0f);
    cameraParams.ViewMatrix = cameraComponent.viewMatrix;
    cameraParams.ProjectionMatrix = cameraComponent.projectionMatrix;
    // memcpy(mGlobalUniformBufferHandler->GetAllocationInfo().pMappedData, &cameraParams, sizeof(CameraParameters));
    // vk::DescriptorBufferInfo bufferInfo{};
    // bufferInfo.setBuffer(mGlobalUniformBufferHandler->GetBuffer()).setOffset(0).setRange(sizeof(CameraParameters));
    // writeDescriptorSets.push_back(vk::WriteDescriptorSet{}
    //                                   .setDstSet(globalDescriptorSet)
    //                                   .setDstBinding(0)
    //                                   .setDstArrayElement(0)
    //                                   .setDescriptorType(vk::DescriptorType::eUniformBuffer)
    //                                   .setBufferInfo(bufferInfo));
}
} // namespace MEngine::Function