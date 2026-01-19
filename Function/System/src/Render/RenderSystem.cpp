#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "Logger.hpp"
#include "Material.hpp"
#include "MaterialComponent.hpp"
#include "MaterialResource.hpp"
#include "Math.hpp"
#include "MeshComponent.hpp"
#include "OffscreenFrameResource.hpp"
#include "PBRMaterial.hpp"
#include "PBRMaterialManager.hpp"
#include "PBRMaterialResource.hpp"
#include "RenderResource.hpp"
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
    PushRenderPass([this](OffscreenFrameResource *frameResource) { GBuffer(frameResource); });
    PushRenderPass([this](OffscreenFrameResource *frameResource) { Lighting(frameResource); });
    PushRenderPass([this](OffscreenFrameResource *frameResource) { ForwardOpaque(frameResource); });
    PushRenderPass([this](OffscreenFrameResource *frameResource) { ForwardTransparent(frameResource); });
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
            if (auto pbrMaterial = std::dynamic_pointer_cast<PBRMaterial>(materialAsset))
            {
                auto pbrMaterialManager = mAssetManager->GetManager<PBRMaterial, PBRMaterialManager>();
                pbrMaterialManager->PushPendingUpdateAsset(
                    std::static_pointer_cast<PBRMaterial>(materialComponent.Material));
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

    auto cameraEntities = mScene->mRegistry->view<TransformComponent, CameraComponent>();
    auto lightEntities = mScene->mRegistry->view<LightComponent, TransformComponent>();
    for (const auto &entity : cameraEntities)
    {
        auto &cameraComponent = mScene->mRegistry->get<CameraComponent>(entity);
        auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
        if (cameraComponent.isMainCamera)
        {
            mOffscreenFrameResource->SceneParams.ViewMatrix = cameraComponent.viewMatrix;
            mOffscreenFrameResource->SceneParams.ProjectionMatrix = cameraComponent.projectionMatrix;
            mOffscreenFrameResource->SceneParams.CameraPosition = Vector4(transformComponent.worldPosition, 1.0f);
            break;
        }
    }
    mOffscreenFrameResource->SceneParams.NumLights = 0;
    for (const auto &entity : lightEntities)
    {
        if (mOffscreenFrameResource->SceneParams.NumLights >= 16)
            break;
        auto &lightComponent = mScene->mRegistry->get<LightComponent>(entity);
        auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);

        if (lightComponent.Enabled)
        {
            auto &lightParam =
                mOffscreenFrameResource->SceneParams.Lights[mOffscreenFrameResource->SceneParams.NumLights++];
            lightParam.LightType = static_cast<uint32_t>(lightComponent.LightType);
            lightParam.Intensity = lightComponent.Intensity;
            lightParam.Color = Vector4(lightComponent.Color, 1.0f);
            lightParam.Radius = lightComponent.Radius;
            lightParam.InnerConeAngle = lightComponent.InnerConeAngle;
            lightParam.OuterConeAngle = lightComponent.OuterConeAngle;
            lightParam.Position = Vector4(transformComponent.worldPosition, 1.0f);
            lightParam.Direction = Vector4(transformComponent.worldRotation * glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
        }
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
    // PBRMaterialResource
    auto pbrMaterialManager = mAssetManager->GetManager<PBRMaterial, PBRMaterialManager>();
    if (pbrMaterialManager->GetPendingUpdateAssetCount() != 0)
    {
        vk::CommandBufferAllocateInfo materialCommandAllocateInfo{};
        materialCommandAllocateInfo.setCommandPool(mOffscreenFrameResource->TransferCommandPool)
            .setLevel(vk::CommandBufferLevel::eSecondary)
            .setCommandBufferCount(1);
        auto secondaryMaterialCommandBuffers = device.allocateCommandBuffers(materialCommandAllocateInfo).front();
        pbrMaterialManager->UpdateAssetRenderResource(mContext, secondaryMaterialCommandBuffers, &inheritanceInfo);
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
    vk::CommandBufferAllocateInfo sceneCommandAllocateInfo{};
    sceneCommandAllocateInfo.setCommandPool(mOffscreenFrameResource->TransferCommandPool)
        .setLevel(vk::CommandBufferLevel::eSecondary)
        .setCommandBufferCount(1);
    auto sceneSecondaryCommandBuffer = device.allocateCommandBuffers(sceneCommandAllocateInfo).front();
    auto mappedData = static_cast<uint8_t *>(mOffscreenFrameResource->SceneStagingBufferAllocationInfo.pMappedData);
    std::memcpy(mappedData, &mOffscreenFrameResource->SceneParams, sizeof(SceneParam));
    vk::CommandBufferBeginInfo sceneBeginInfo{};
    sceneBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit).setPInheritanceInfo(&inheritanceInfo);
    sceneSecondaryCommandBuffer.begin(sceneBeginInfo);
    vk::BufferCopy2 copyRegion{};
    copyRegion.setSize(sizeof(SceneParam)).setSrcOffset(0).setDstOffset(0);
    vk::CopyBufferInfo2 copyBufferInfo{};
    copyBufferInfo.setSrcBuffer(mOffscreenFrameResource->SceneStagingBuffer)
        .setDstBuffer(mOffscreenFrameResource->SceneSSBO)
        .setRegions(copyRegion);
    sceneSecondaryCommandBuffer.copyBuffer2(copyBufferInfo);
    vk::BufferMemoryBarrier2 sceneBufferBarrier{};
    sceneBufferBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
        .setDstStageMask(vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setBuffer(mOffscreenFrameResource->SceneSSBO)
        .setOffset(0)
        .setSize(sizeof(SceneParam));
    vk::DependencyInfo sceneDepInfo{};
    sceneDepInfo.setBufferMemoryBarriers({sceneBufferBarrier});
    sceneSecondaryCommandBuffer.pipelineBarrier2(sceneDepInfo);
    sceneSecondaryCommandBuffer.end();
    mOffscreenFrameResource->SecondaryTransferCommandBuffers.push_back(sceneSecondaryCommandBuffer);

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
    if (mRenderQueues.contains(DefaultGraphicPipelineType::ForwardOpaquePBR))
    {
        auto &entities = mRenderQueues.at(DefaultGraphicPipelineType::ForwardOpaquePBR);
        auto pipelineAsset = mAssetManager->GetByName<GraphicPipeline>(DefaultGraphicPipelineType::ForwardOpaquePBR);
        auto graphicPipelineGBufferPipeline = pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipeline();
        auto graphicPipelineGBufferPipelineLayout =
            pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipelineLayout();
        currentGraphicCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPipelineGBufferPipeline);
        currentGraphicCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                       graphicPipelineGBufferPipelineLayout, 0,
                                                       mContext->TextureBindlessDescriptorSet.get(), {});
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
            pbrPushConstants.SceneSSBOAddress = frameResource->SceneSSBOAddress;
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
void RenderSystem::ForwardTransparent(OffscreenFrameResource *frameResource)
{
}
void RenderSystem::GBuffer(OffscreenFrameResource *frameResource)
{
    auto currentGraphicCommandBuffer = frameResource->GraphicsCommandBuffer;
    auto colorAttachment = frameResource->ColorTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto albedoAttachment = frameResource->AlbedoTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto normalAttachment = frameResource->NormalTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto armAttachment = frameResource->ARMTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto positionAttachment = frameResource->PositionTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto emissiveAttachment = frameResource->EmissiveTexture->GetResourceAs<TextureRenderTarget2DResource>();
    auto depthStencilAttachment = frameResource->DepthStencilTexture->GetResourceAs<TextureRenderTarget2DResource>();
    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos{
        // Color
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->ColorClearValue)
            .setImageView(colorAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Albedo
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->AlbedoClearValue)
            .setImageView(albedoAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Normal
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->NormalClearValue)
            .setImageView(normalAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // ARM
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->ARMClearValue)
            .setImageView(armAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Position
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->PositionClearValue)
            .setImageView(positionAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
        // Emissive
        vk::RenderingAttachmentInfo()
            .setClearValue(frameResource->EmissiveClearValue)
            .setImageView(emissiveAttachment->GetImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore),
    };
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
    if (mRenderQueues.contains(DefaultGraphicPipelineType::GBufferOpaquePBR))
    {
        auto &entities = mRenderQueues.at(DefaultGraphicPipelineType::GBufferOpaquePBR);
        auto pipelineAsset = mAssetManager->GetByName<GraphicPipeline>(DefaultGraphicPipelineType::GBufferOpaquePBR);
        auto graphicPipelineGBufferPipeline = pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipeline();
        auto graphicPipelineGBufferPipelineLayout =
            pipelineAsset->GetResourceAs<GraphicPipelineResource>()->GetPipelineLayout();
        currentGraphicCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPipelineGBufferPipeline);
        currentGraphicCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                       graphicPipelineGBufferPipelineLayout, 0,
                                                       mContext->TextureBindlessDescriptorSet.get(), {});
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
            pbrPushConstants.SceneSSBOAddress = frameResource->SceneSSBOAddress;
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
void RenderSystem::Lighting(OffscreenFrameResource *frameResource)
{
}

} // namespace MEngine::Function