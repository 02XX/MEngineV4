#include "RenderSystem.hpp"
#include "AssetManager.hpp"
#include "CameraComponent.hpp"
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "IBind.hpp"
#include "IPendingResourceManager.hpp"
#include "LightComponent.hpp"
#include "Logger.hpp"
#include "Material.hpp"
#include "MaterialComponent.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "MeshComponent.hpp"
#include "OffscreenFrameResource.hpp"
#include "PBRMaterial.hpp"
#include "PhongMaterial.hpp"
#include "PipelineManager.hpp"
#include "RenderResource.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "SceneResource.hpp"
#include "TextureManager.hpp"
#include "TextureResource.hpp"
#include "TransformComponent.hpp"
#include <limits>
#include <memory>
#include <vector>

namespace MEngine::Function
{
RenderSystem::RenderSystem(std::shared_ptr<Context> context, std::shared_ptr<Scene> scene)
    : System(scene), mContext(context)
{
}
RenderSystem::~RenderSystem()
{
}
void RenderSystem::Init()
{
    // Configure Render Passes
    // PushRenderPass([this](OffscreenFrameResource *frameResource) { GBuffer(frameResource); });
    // PushRenderPass([this](OffscreenFrameResource *frameResource) { Lighting(frameResource); });
    PushRenderPass([this](OffscreenFrameResource *frameResource) { ForwardOpaque(frameResource); });
    // PushRenderPass([this](OffscreenFrameResource *frameResource) { ForwardTransparent(frameResource); });
}
void RenderSystem::Update(double deltaTime)
{
#pragma region Prepare
    PrepareRenderQueues();
#pragma endregion
    auto device = mContext->Device.get();
    auto result = device.waitForFences(mOffscreenFrameResource->InFlightFence.get(), vk::True,
                                       std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed to wait for fence: {}", vk::to_string(result));
        return;
    }
    device.resetFences(mOffscreenFrameResource->InFlightFence.get());
#pragma region Transfer

    RenderContext renderContext{mContext, mOffscreenFrameResource->TransferCommandBuffer};
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    mOffscreenFrameResource->TransferCommandBuffer.begin(beginInfo);
    //====================Last Frame 清理上一帧的资源=========================================
    AssetManager::Instance().ProcessPendingDeletionResources(
        renderContext); // TODO: After MAX_INFLIGHT_FRAME Count to delete
    //=======================================================================
    //====================This Frame 本帧资源初始化和更新=========================================
    AssetManager::Instance().ProcessPendingInitResources(renderContext);
    AssetManager::Instance().ProcessPendingUpdateResources(renderContext);
    mOffscreenFrameResource->TransferCommandBuffer.end();
    vk::SubmitInfo2 transferSubmitInfo{};
    transferSubmitInfo.setCommandBufferInfos(
        {vk::CommandBufferSubmitInfo().setCommandBuffer(mOffscreenFrameResource->TransferCommandBuffer)});
    transferSubmitInfo.setSignalSemaphoreInfos(
        {vk::SemaphoreSubmitInfo()
             .setSemaphore(mOffscreenFrameResource->TransferFinishedSemaphore.get())
             .setStageMask(vk::PipelineStageFlagBits2::eTransfer)});
#pragma endregion
#pragma region Graphics
    for (const auto &preRecord : mPreRecord)
    {
        preRecord(mOffscreenFrameResource);
    }
    vk::CommandBufferBeginInfo graphicsBeginInfo{};
    auto graphicCommandBuffer = mOffscreenFrameResource->GraphicsCommandBuffer;
    graphicCommandBuffer.begin(graphicsBeginInfo);
    for (const auto &preProcessPass : mPreProcessPasses)
    {
        preProcessPass(mOffscreenFrameResource);
    }
    for (const auto &renderPass : mRenderPasses)
    {
        renderPass(mOffscreenFrameResource);
    }
    for (const auto &postProcessPass : mPostProcessPasses)
    {
        postProcessPass(mOffscreenFrameResource);
    }
    graphicCommandBuffer.end();
    vk::SubmitInfo2 graphicSubmitInfo{};
    std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
        vk::CommandBufferSubmitInfo().setCommandBuffer(mOffscreenFrameResource->GraphicsCommandBuffer),
    };
    std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos = {
        vk::SemaphoreSubmitInfo()
            .setSemaphore(mOffscreenFrameResource->RenderFinishedSemaphore.get())
            .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)};
    std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos = {
        vk::SemaphoreSubmitInfo()
            .setSemaphore(mOffscreenFrameResource->ImageAvailableSemaphore.get())
            .setStageMask(vk::PipelineStageFlagBits2::eTopOfPipe),
        vk::SemaphoreSubmitInfo()
            .setSemaphore(mOffscreenFrameResource->TransferFinishedSemaphore.get())
            .setStageMask(vk::PipelineStageFlagBits2::eTransfer)};

    graphicSubmitInfo.setCommandBufferInfos(commandBufferInfos)
        .setSignalSemaphoreInfos(signalSemaphoreInfos)
        .setWaitSemaphoreInfos(waitSemaphoreInfos);

    mContext->GraphicsQueue.submit2({transferSubmitInfo, graphicSubmitInfo},
                                    mOffscreenFrameResource->InFlightFence.get());
    for (const auto &postSubmitPass : mPostSubmit)
    {
        postSubmitPass(mOffscreenFrameResource);
    }
#pragma endregion
}
void RenderSystem::PrepareRenderQueues()
{
    mRenderQueues.clear();
    auto entities = mScene->mRegistry->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (const auto &entity : entities)
    {
        auto &materialComponent = entities.get<MaterialComponent>(entity);
        auto &meshComponent = entities.get<MeshComponent>(entity);
        auto pipeline = materialComponent.Material->mPipeline;
        materialComponent.Material->GetResource()->InitResource(mContext);
        meshComponent.Mesh->GetResource()->InitResource(mContext);
        if (materialComponent.Dirty)
        {
            auto materialAsset = materialComponent.Material;
            materialAsset->PendingUpdate();
            materialComponent.Dirty = false;
        }
        if (meshComponent.Dirty)
        {
            auto meshAsset = meshComponent.Mesh;
            meshAsset->PendingUpdate();
            meshComponent.Dirty = false;
        }
        auto &transformComponent = entities.get<TransformComponent>(entity);
        mRenderQueues[pipeline->GetName()].push_back(entity);
    }
    // Scene
    mScene->GetResource()->InitResource(mContext);
    auto cameraEntities = mScene->mRegistry->view<TransformComponent, CameraComponent>();
    auto lightEntities = mScene->mRegistry->view<LightComponent, TransformComponent>();
    int lightIndex = 0;
    for (const auto &entity : lightEntities)
    {
        auto &lightComponent = mScene->mRegistry->get<LightComponent>(entity);
        auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
        if (lightComponent.Dirty && lightComponent.Enabled)
        {
            auto &lightParam = mScene->mLightParams[lightIndex++];
            lightParam.LightType = static_cast<uint32_t>(lightComponent.LightType);
            lightParam.Intensity = lightComponent.Intensity;
            lightParam.Color = Vector4(lightComponent.Color, 1.0f);
            lightParam.Radius = lightComponent.Radius;
            lightParam.InnerConeAngle = lightComponent.InnerConeAngle;
            lightParam.OuterConeAngle = lightComponent.OuterConeAngle;
            lightParam.Position = Vector4(transformComponent.worldPosition, 1.0f);
            lightParam.Direction = Vector4(transformComponent.worldRotation * glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
            lightComponent.Dirty = false;
            mScene->mLightParamsDirty = true;
        }
    }
    for (const auto &entity : cameraEntities)
    {
        auto &cameraComponent = mScene->mRegistry->get<CameraComponent>(entity);
        auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
        if (cameraComponent.Dirty && cameraComponent.isMainCamera)
        {
            mScene->mSceneParams.ViewMatrix = cameraComponent.viewMatrix;
            mScene->mSceneParams.ProjectionMatrix = cameraComponent.projectionMatrix;
            mScene->mSceneParams.CameraPosition = Vector4(transformComponent.worldPosition, 1.0f);
            mScene->mSceneParams.CameraDirection =
                Vector4(transformComponent.worldRotation * glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
            mScene->mSceneParams.LightCount = lightIndex;
            cameraComponent.Dirty = false;
            mScene->mSceneParamsDirty = true;
            break;
        }
    }
    if (mScene->mSceneParamsDirty || mScene->mLightParamsDirty)
    {
        mScene->PendingUpdate();
        mScene->mSceneParamsDirty = false;
        mScene->mLightParamsDirty = false;
    }
}
void RenderSystem::Shutdown()
{
}
void RenderSystem::ForwardOpaque(OffscreenFrameResource *frameResource)
{
    auto currentGraphicCommandBuffer = frameResource->GraphicsCommandBuffer;
    auto depthStencilAttachment = frameResource->DepthStencilTexture->GetResourceAs<TextureResource>();
    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos{
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->AlbedoTexture->mClearValue)
            .setImageView(frameResource->AlbedoTexture->GetResourceAs<TextureResource>()->mImageView)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->NormalTexture->mClearValue)
            .setImageView(frameResource->NormalTexture->GetResourceAs<TextureResource>()->mImageView)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->ARMTexture->mClearValue)
            .setImageView(frameResource->ARMTexture->GetResourceAs<TextureResource>()->mImageView)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->PositionTexture->mClearValue)
            .setImageView(frameResource->PositionTexture->GetResourceAs<TextureResource>()->mImageView)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        vk::RenderingAttachmentInfo()
            .setClearValue(mOffscreenFrameResource->EmissiveTexture->mClearValue)
            .setImageView(frameResource->EmissiveTexture->GetResourceAs<TextureResource>()->mImageView)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),

    };
    vk::RenderingAttachmentInfo depthStencilAttachmentInfo{};
    depthStencilAttachmentInfo.setClearValue(mOffscreenFrameResource->DepthStencilTexture->mClearValue)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setImageView(depthStencilAttachment->mImageView);
    vk::RenderingInfo renderingInfo{};
    renderingInfo.setRenderArea(vk::Rect2D{{0, 0}, {frameResource->Extent.width, frameResource->Extent.height}})
        .setLayerCount(1)
        .setColorAttachments(colorAttachmentInfos)
        .setPDepthAttachment(&depthStencilAttachmentInfo);
    currentGraphicCommandBuffer.beginRendering(renderingInfo);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(frameResource->Extent.height)
        .setWidth(static_cast<float>(frameResource->Extent.width))
        .setHeight(-static_cast<float>(frameResource->Extent.height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    currentGraphicCommandBuffer.setViewport(0, {viewport});
    currentGraphicCommandBuffer.setViewportWithCount({viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({frameResource->Extent.width, frameResource->Extent.height});
    currentGraphicCommandBuffer.setScissorWithCount({scissor});
    currentGraphicCommandBuffer.setScissor(0, {scissor});
    if (mRenderQueues.contains(DefaultGraphicPipelineType::ForwardOpaquePhong))
    {
        auto &entities = mRenderQueues.at(DefaultGraphicPipelineType::ForwardOpaquePhong);
        auto &assetManager = AssetManager::Instance();
        auto graphicPipelineAsset =
            assetManager.GetByNameAs<GraphicPipeline>(DefaultGraphicPipelineType::ForwardOpaquePhong);
        auto graphicPipelinePipeline = graphicPipelineAsset->GetResourceAs<GraphicPipelineResource>()->mPipeline;
        auto graphicPipelinePipelineLayout =
            graphicPipelineAsset->GetResourceAs<GraphicPipelineResource>()->mPipelineLayout;
        currentGraphicCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPipelinePipeline);
        BindContext bindContext{
            .CommandBuffer = currentGraphicCommandBuffer,
            .PipelineLayout = graphicPipelinePipelineLayout,
            .Pipeline = graphicPipelinePipeline,
        };
        auto textureManager = assetManager.GetManager<Texture>()->As<TextureManager>();
        textureManager->Bind(bindContext);
        mScene->GetResourceAs<SceneResource>()->Bind(bindContext);
        for (const auto &entity : entities)
        {
            auto &materialComponent = mScene->mRegistry->get<MaterialComponent>(entity);
            auto &meshComponent = mScene->mRegistry->get<MeshComponent>(entity);
            auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
            auto material = static_cast<PhongMaterial *>(materialComponent.Material.get());
            auto materialResource = materialComponent.Material->GetResourceAs<MaterialResource>();
            auto meshResource = meshComponent.Mesh->GetResourceAs<MeshResource>();
            materialResource->Bind(bindContext);
            auto modelMatrix = transformComponent.modelMatrix;
            currentGraphicCommandBuffer.pushConstants(
                graphicPipelinePipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
                sizeof(Matrix4), &modelMatrix);
            meshResource->Bind(bindContext);
        }
    }
    currentGraphicCommandBuffer.endRendering();
}
void RenderSystem::ForwardTransparent(OffscreenFrameResource *frameResource)
{
}
void RenderSystem::GBuffer(OffscreenFrameResource *frameResource)
{
}
void RenderSystem::Lighting(OffscreenFrameResource *frameResource)
{
}

} // namespace MEngine::Function