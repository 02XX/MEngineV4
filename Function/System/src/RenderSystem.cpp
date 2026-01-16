#include "RenderSystem.hpp"
#include "Logger.hpp"
#include "MaterialComponent.hpp"
#include "MeshComponent.hpp"
#include "RenderResource.hpp"
#include "TransformComponent.hpp"
#include <vector>

namespace MEngine::Function
{
RenderSystem::RenderSystem(std::shared_ptr<Context> context, std::shared_ptr<Scene> scene,
                           std::shared_ptr<AssetManager> assetManager)
    : System(scene, assetManager), mContext(context)
{
}
RenderSystem::~RenderSystem()
{
}
void RenderSystem::Init()
{
}
void RenderSystem::Update(double deltaTime)
{
    PrepareRenderQueues();
    Prepare();
    RenderGBuffer();
    RenderLighting();
    End();
}
void RenderSystem::Shutdown()
{
}

void RenderSystem::PrepareRenderQueues()
{
    mRenderQueues.clear();
    auto entities = mScene->mRegistry->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (const auto &entity : entities)
    {
        auto &materialComponent = entities.get<MaterialComponent>(entity);
        auto pipeline = materialComponent.Material->GetPipeline();
        pipeline->GetResource()->InitResource(mContext);
        materialComponent.Material->GetResource()->InitResource(mContext);
        auto &meshComponent = entities.get<MeshComponent>(entity);
        meshComponent.Mesh->GetResource()->InitResource(mContext);
        auto &transformComponent = entities.get<TransformComponent>(entity);
        mRenderQueues[pipeline->GetName()].push_back(entity);
    }
}
void RenderSystem::Prepare()
{
    auto device = mContext->Device.get();
    mFrameResource->GraphicsCommandBuffer.begin(vk::CommandBufferBeginInfo{});
}
void RenderSystem::RenderGBuffer()
{
    auto currentGraphicCommandBuffer = mFrameResource->GraphicsCommandBuffer;
    auto colorAttachment = mFrameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto albedoAttachment = mFrameResource->AlbedoTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto normalAttachment = mFrameResource->NormalTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto armAttachment = mFrameResource->ARMTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto positionAttachment = mFrameResource->PositionTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto emissiveAttachment = mFrameResource->EmissiveTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto depthStencilAttachment = mFrameResource->DepthStencilTexture->GetResourceAs<TextureRenderTarget2DResource>();
    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(mFrameResource->ColorClearValue)
            .setImageView(colorAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Albedo
        vk::RenderingAttachmentInfo()
            .setClearValue(mFrameResource->AlbedoClearValue)
            .setImageView(albedoAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Normal
        vk::RenderingAttachmentInfo()
            .setClearValue(mFrameResource->NormalClearValue)
            .setImageView(normalAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // ARM
        vk::RenderingAttachmentInfo()
            .setClearValue(mFrameResource->ARMClearValue)
            .setImageView(armAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Position
        vk::RenderingAttachmentInfo()
            .setClearValue(mFrameResource->PositionClearValue)
            .setImageView(positionAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Emissive
        vk::RenderingAttachmentInfo()
            .setClearValue(mFrameResource->EmissiveClearValue)
            .setImageView(emissiveAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
    };
    vk::RenderingAttachmentInfo depthStencilAttachmentInfo{};
    depthStencilAttachmentInfo.setClearValue(mFrameResource->DepthClearValue)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setImageView(depthStencilAttachment->GetImageView());
    vk::RenderingInfo renderingInfo{};
    renderingInfo.setRenderArea(vk::Rect2D{{0, 0}, {mFrameResource->Extent.width, mFrameResource->Extent.height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachmentInfos)
        .setPDepthAttachment(&depthStencilAttachmentInfo);
    currentGraphicCommandBuffer.beginRendering(renderingInfo);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(mFrameResource->Extent.height)
        .setWidth(static_cast<float>(mFrameResource->Extent.width))
        .setHeight(-static_cast<float>(mFrameResource->Extent.height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    currentGraphicCommandBuffer.setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({mFrameResource->Extent.width, mFrameResource->Extent.height});
    currentGraphicCommandBuffer.setScissor(0, {scissor});
    if (mRenderQueues.contains("GraphicPipeline_GBuffer"))
    {
        auto &entities = mRenderQueues.at("GraphicPipeline_GBuffer");
        auto pipelineAsset = mAssetManager->GetByName<GraphicPipeline>("GraphicPipeline_GBuffer");
        auto graphicPipelineGBufferPipeline = pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipeline();
        auto graphicPipelineGBufferPipelineLayout =
            pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipelineLayout();
        currentGraphicCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPipelineGBufferPipeline);
        currentGraphicCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                       graphicPipelineGBufferPipelineLayout, 0,
                                                       mContext->DescriptorSet.get(), {});
        for (const auto &entity : entities)
        {
            auto &materialComponent = mScene->mRegistry->get<MaterialComponent>(entity);
            auto &meshComponent = mScene->mRegistry->get<MeshComponent>(entity);
            auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
            auto staticMeshResource = meshComponent.Mesh->GetResourceAs<StaticMeshResource>();
            auto vertexBuffer = staticMeshResource->GetVertexBuffer();
            auto indexBuffer = staticMeshResource->GetIndexBuffer();
            currentGraphicCommandBuffer.bindVertexBuffers(0, vertexBuffer, {0});
            currentGraphicCommandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
            currentGraphicCommandBuffer.drawIndexed(staticMeshResource->GetIndexCount(), 1, 0, 0, 0);
        }
    }
    currentGraphicCommandBuffer.endRendering();
}
void RenderSystem::RenderLighting()
{
}
void RenderSystem::End()
{
    auto device = mContext->Device.get();
    auto currentGraphicCommandBuffer = mFrameResource->GraphicsCommandBuffer;
    currentGraphicCommandBuffer.end();
}
} // namespace MEngine::Function