#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "Logger.hpp"
#include "MaterialComponent.hpp"
#include "MaterialResource.hpp"
#include "MeshComponent.hpp"
#include "PBRMaterial.hpp"
#include "PBRMaterialResource.hpp"
#include "RenderResource.hpp"
#include "Texture2DResource.hpp"
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
    UpdateMaterial();
    PrepareRenderQueues();
    Prepare();
    RenderGBuffer();
    RenderLighting();
    End();
}
void RenderSystem::UpdateMaterial()
{
    auto entities = mScene->mRegistry->view<MaterialComponent>();
    for (const auto &entity : entities)
    {
        auto &materialComponent = entities.get<MaterialComponent>(entity);
        if (materialComponent.dirty)
        {
            auto material = materialComponent.Material;
            auto materialResource = material->GetResourceAs<MaterialResource>();
            materialResource->UpdateMaterial(mContext);
            materialComponent.dirty = false;
        }
    }
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
    mScene->GetResource()->InitResource(mContext);
    SceneParameter sceneParams{};
    auto cameraEntities = mScene->mRegistry->view<TransformComponent, CameraComponent>();
    for (const auto &entity : cameraEntities)
    {
        auto &cameraComponent = mScene->mRegistry->get<CameraComponent>(entity);
        auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
        if (cameraComponent.isMainCamera)
        {
            sceneParams.ViewMatrix = cameraComponent.viewMatrix;
            sceneParams.ProjectionMatrix = cameraComponent.projectionMatrix;
            sceneParams.CameraPosition = transformComponent.worldPosition;
            break;
        }
    }
    auto sceneResource = mScene->GetResourceAs<SceneResource>();
    sceneResource->UpdateSceneUBO(sceneParams);
    mOffscreenFrameResource->GraphicsCommandBuffer.begin(vk::CommandBufferBeginInfo{});
}
void RenderSystem::RenderGBuffer()
{
    auto currentGraphicCommandBuffer = mOffscreenFrameResource->GraphicsCommandBuffer;
    auto colorAttachment = mOffscreenFrameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto albedoAttachment = mOffscreenFrameResource->AlbedoTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto normalAttachment = mOffscreenFrameResource->NormalTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto armAttachment = mOffscreenFrameResource->ARMTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto positionAttachment = mOffscreenFrameResource->PositionTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto emissiveAttachment = mOffscreenFrameResource->EmissiveTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto depthStencilAttachment =
        mOffscreenFrameResource->DepthStencilTexture->GetResourceAs<TextureRenderTarget2DResource>();
    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->ColorClearValue)
            .setImageView(colorAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Albedo
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->AlbedoClearValue)
            .setImageView(albedoAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Normal
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->NormalClearValue)
            .setImageView(normalAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // ARM
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->ARMClearValue)
            .setImageView(armAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Position
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->PositionClearValue)
            .setImageView(positionAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Emissive
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->EmissiveClearValue)
            .setImageView(emissiveAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
    };
    vk::RenderingAttachmentInfo depthStencilAttachmentInfo{};
    depthStencilAttachmentInfo.setClearValue(mOffscreenFrameResource->DepthClearValue)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setImageView(depthStencilAttachment->GetImageView());
    vk::RenderingInfo renderingInfo{};
    renderingInfo
        .setRenderArea(
            vk::Rect2D{{0, 0}, {mOffscreenFrameResource->Extent.width, mOffscreenFrameResource->Extent.height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachmentInfos)
        .setPDepthAttachment(&depthStencilAttachmentInfo);
    currentGraphicCommandBuffer.beginRendering(renderingInfo);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(mOffscreenFrameResource->Extent.height)
        .setWidth(static_cast<float>(mOffscreenFrameResource->Extent.width))
        .setHeight(-static_cast<float>(mOffscreenFrameResource->Extent.height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    currentGraphicCommandBuffer.setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent(
        {mOffscreenFrameResource->Extent.width, mOffscreenFrameResource->Extent.height});
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

            auto pbrMaterial = static_cast<PBRMaterial *>(materialComponent.Material.get());
            auto pbrMaterialResource = materialComponent.Material->GetResourceAs<PBRMaterialResource>();

            auto modelMatrix = transformComponent.modelMatrix;

            PBRMaterialPushConstants pbrPushConstants{};
            pbrPushConstants.ModelMatrix = modelMatrix;
            pbrPushConstants.MaterialSSBOAddress = pbrMaterialResource->mSSBOAddress;
            pbrPushConstants.SceneSSBOAddress = mScene->GetResourceAs<SceneResource>()->mSceneSSBOAddress;
            currentGraphicCommandBuffer.pushConstants(graphicPipelineGBufferPipelineLayout,
                                                      vk::ShaderStageFlagBits::eVertex |
                                                          vk::ShaderStageFlagBits::eFragment,
                                                      0, sizeof(PBRMaterialPushConstants), &pbrPushConstants);

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
    auto currentGraphicCommandBuffer = mOffscreenFrameResource->GraphicsCommandBuffer;
    currentGraphicCommandBuffer.end();
}
} // namespace MEngine::Function