#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "GraphicPipelineManager.hpp"
#include "LightComponent.hpp"
#include "Logger.hpp"
#include "Material.hpp"
#include "MaterialComponent.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "MeshComponent.hpp"
#include "OffscreenFrameResource.hpp"
#include "PBRMaterial.hpp"
#include "PBRMaterialResource.hpp"
#include "PhongMaterial.hpp"
#include "PhongMaterialManager.hpp"
#include "PhongMaterialResource.hpp"
#include "RenderResource.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "SceneResource.hpp"
#include "Texture2DManager.hpp"
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
    // Configure Render Passes
    // PushRenderPass([this](OffscreenFrameResource *frameResource) { GBuffer(frameResource); });
    // PushRenderPass([this](OffscreenFrameResource *frameResource) { Lighting(frameResource); });
    PushRenderPass([this](OffscreenFrameResource *frameResource) { ForwardOpaque(frameResource); });
    // PushRenderPass([this](OffscreenFrameResource *frameResource) { ForwardTransparent(frameResource); });
}
void RenderSystem::Update(double deltaTime)
{
    PrepareRenderQueues();
    Render();
}
void RenderSystem::PrepareRenderQueues()
{
    mRenderQueues.clear();
    auto entities = mScene->mRegistry->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (const auto &entity : entities)
    {
        auto &materialComponent = entities.get<MaterialComponent>(entity);
        if (materialComponent.Dirty)
        {
            auto materialAsset = materialComponent.Material;
            if (auto phongMaterial = std::dynamic_pointer_cast<PhongMaterial>(materialAsset))
            {
                auto phongMaterialManager = mAssetManager->GetManager<PhongMaterial, PhongMaterialManager>();
                phongMaterialManager->PushPendingUpdateAsset(
                    std::static_pointer_cast<PhongMaterial>(materialComponent.Material));
            }
            else
            {
                LogError("Unsupported material type for rendering");
                continue;
            }
            materialComponent.Dirty = false;
        }
        auto pipeline = materialComponent.Material->GetPipeline();
        pipeline->GetResource()->InitResource(mContext);
        materialComponent.Material->GetResource()->InitResource(mContext);
        auto &meshComponent = entities.get<MeshComponent>(entity);
        meshComponent.Mesh->GetResource()->InitResource(mContext);
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
        auto sceneManager = mAssetManager->GetManager<Scene, SceneManager>();
        sceneManager->PushPendingUpdateAsset(std::static_pointer_cast<Scene>(mScene));
        mScene->mSceneParamsDirty = false;
        mScene->mLightParamsDirty = false;
    }
}
void RenderSystem::Render()
{
    auto device = mContext->Device.get();
    auto result = device.waitForFences(mOffscreenFrameResource->InFlightFence.get(), vk::True,
                                       1000000000); // 1 second timeout
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed to wait for fence: {}", vk::to_string(result));
        return;
    }
    device.resetFences(mOffscreenFrameResource->InFlightFence.get());
    device.resetCommandPool(mOffscreenFrameResource->GraphicsCommandPool);
    device.resetCommandPool(mOffscreenFrameResource->TransferCommandPool);

    //=========================准备渲染资源=========================
    auto transferCommandBuffer = mOffscreenFrameResource->TransferCommandBuffer;
    for (auto subCommandBuffer : mOffscreenFrameResource->SecondaryTransferCommandBuffers)
    {
        device.freeCommandBuffers(mOffscreenFrameResource->TransferCommandPool, subCommandBuffer);
    }
    mOffscreenFrameResource->SecondaryTransferCommandBuffers.clear();
    vk::CommandBufferInheritanceInfo inheritanceInfo{};
    // PhongMaterialResource
    auto phongMaterialManager = mAssetManager->GetManager<PhongMaterial, PhongMaterialManager>();
    if (phongMaterialManager->GetPendingUpdateAssetCount() != 0)
    {
        vk::CommandBufferAllocateInfo materialCommandAllocateInfo{};
        materialCommandAllocateInfo.setCommandPool(mOffscreenFrameResource->TransferCommandPool)
            .setLevel(vk::CommandBufferLevel::eSecondary)
            .setCommandBufferCount(1);
        auto secondaryMaterialCommandBuffers = device.allocateCommandBuffers(materialCommandAllocateInfo).front();
        phongMaterialManager->UpdateAssetRenderResource(mContext, secondaryMaterialCommandBuffers, &inheritanceInfo);
        mOffscreenFrameResource->SecondaryTransferCommandBuffers.push_back(secondaryMaterialCommandBuffers);
    }
    //   Texture2DResource
    auto texture2DManager = mAssetManager->GetManager<Texture2D, Texture2DManager>();
    if (texture2DManager->GetPendingUpdateAssetCount() != 0)
    {
        vk::CommandBufferAllocateInfo textureCommandAllocateInfo{};
        textureCommandAllocateInfo.setCommandPool(mOffscreenFrameResource->TransferCommandPool)
            .setLevel(vk::CommandBufferLevel::eSecondary)
            .setCommandBufferCount(1);
        auto secondaryTextureCommandBuffers = device.allocateCommandBuffers(textureCommandAllocateInfo).front();
        texture2DManager->UpdateAssetRenderResource(mContext, secondaryTextureCommandBuffers, &inheritanceInfo);
        mOffscreenFrameResource->SecondaryTransferCommandBuffers.push_back(std::move(secondaryTextureCommandBuffers));
    }
    // Scene
    auto sceneManager = mAssetManager->GetManager<Scene, SceneManager>();
    if (sceneManager->GetPendingUpdateAssetCount() != 0)
    {
        vk::CommandBufferAllocateInfo sceneCommandAllocateInfo{};
        sceneCommandAllocateInfo.setCommandPool(mOffscreenFrameResource->TransferCommandPool)
            .setLevel(vk::CommandBufferLevel::eSecondary)
            .setCommandBufferCount(1);
        auto sceneSecondaryCommandBuffer = device.allocateCommandBuffers(sceneCommandAllocateInfo).front();

        sceneManager->UpdateAssetRenderResource(mContext, sceneSecondaryCommandBuffer, &inheritanceInfo);
        mOffscreenFrameResource->SecondaryTransferCommandBuffers.push_back(sceneSecondaryCommandBuffer);
    }
    vk::CommandBufferBeginInfo transferBeginInfo{};
    transferCommandBuffer.begin(transferBeginInfo);
    std::vector<vk::CommandBuffer> secondaryBuffers{};
    if (!mOffscreenFrameResource->SecondaryTransferCommandBuffers.empty())
        transferCommandBuffer.executeCommands(mOffscreenFrameResource->SecondaryTransferCommandBuffers);
    transferCommandBuffer.end();
    vk::SubmitInfo2 transferSubmitInfo{};
    transferSubmitInfo.setCommandBufferInfos(
        {vk::CommandBufferSubmitInfo().setCommandBuffer(mOffscreenFrameResource->TransferCommandBuffer)});
    transferSubmitInfo.setSignalSemaphoreInfos(
        {vk::SemaphoreSubmitInfo()
             .setSemaphore(mOffscreenFrameResource->TransferFinishedSemaphore.get())
             .setStageMask(vk::PipelineStageFlagBits2::eTransfer)});
    mContext->TransferQueue.submit2({transferSubmitInfo}, {});
    //=========================开始渲染=========================
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

    vk::SubmitInfo2 graphicSumbitInfo{};
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

    graphicSumbitInfo.setCommandBufferInfos(commandBufferInfos)
        .setSignalSemaphoreInfos(signalSemaphoreInfos)
        .setWaitSemaphoreInfos(waitSemaphoreInfos);

    mContext->GraphicsQueue.submit2({graphicSumbitInfo}, mOffscreenFrameResource->InFlightFence.get());
    for (const auto &postSubmitPass : mPostSubmit)
    {
        postSubmitPass(mOffscreenFrameResource);
    }
}
void RenderSystem::Shutdown()
{
}
void RenderSystem::ForwardOpaque(OffscreenFrameResource *frameResource)
{
    auto currentGraphicCommandBuffer = frameResource->GraphicsCommandBuffer;
    auto colorAttachment = frameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto depthStencilAttachment = frameResource->DepthStencilTexture->GetResourceAs<TextureRenderTarget2DResource>();
    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos{
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->ColorClearValue)
            .setImageView(colorAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)};
    vk::RenderingAttachmentInfo depthStencilAttachmentInfo{};
    depthStencilAttachmentInfo.setClearValue(frameResource->DepthClearValue)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setImageView(depthStencilAttachment->GetImageView());
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
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({frameResource->Extent.width, frameResource->Extent.height});
    currentGraphicCommandBuffer.setScissor(0, {scissor});
    if (mRenderQueues.contains(DefaultGraphicPipelineType::ForwardOpaquePhong))
    {
        auto &entities = mRenderQueues.at(DefaultGraphicPipelineType::ForwardOpaquePhong);
        auto pipelineAsset = mAssetManager->GetByName<GraphicPipeline>(DefaultGraphicPipelineType::ForwardOpaquePhong);
        auto graphicPipelineGBufferPipeline = pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipeline();
        auto graphicPipelineGBufferPipelineLayout =
            pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipelineLayout();
        currentGraphicCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPipelineGBufferPipeline);
        currentGraphicCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                       graphicPipelineGBufferPipelineLayout, 0,
                                                       mContext->TextureBindlessDescriptorSet.get(), {});
        currentGraphicCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                       graphicPipelineGBufferPipelineLayout, 1,
                                                       mScene->GetResourceAs<SceneResource>()->mDescriptorSet, {});
        for (const auto &entity : entities)
        {
            auto &materialComponent = mScene->mRegistry->get<MaterialComponent>(entity);
            auto &meshComponent = mScene->mRegistry->get<MeshComponent>(entity);
            auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);

            auto phongMaterial = static_cast<PhongMaterial *>(materialComponent.Material.get());
            auto phongMaterialResource = materialComponent.Material->GetResourceAs<PhongMaterialResource>();

            auto modelMatrix = transformComponent.modelMatrix;
            currentGraphicCommandBuffer.pushConstants(graphicPipelineGBufferPipelineLayout,
                                                      vk::ShaderStageFlagBits::eVertex |
                                                          vk::ShaderStageFlagBits::eFragment,
                                                      0, sizeof(Matrix4), &modelMatrix);
            currentGraphicCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                           graphicPipelineGBufferPipelineLayout, 2,
                                                           phongMaterialResource->mDescriptorSet, {});
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