#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "ColorAttachmentBuilder.hpp"
#include "DepthStencilAttachmentBuilder.hpp"
#include "MaterialComponent.hpp"
#include "MeshComponent.hpp"
#include "TextureRenderTarget2D.hpp"
#include "TextureRenderTarget2DDirector.hpp"
#include "TransformComponent.hpp"

namespace MEngine::Function
{

void RenderSystem::PrePareRenderQueues()
{
    mRenderQueues.clear();
    auto registry = mScene->GetRegistry();
    auto view = registry->view<TransformComponent, MeshComponent, MaterialComponent>();
    for (auto entity : view)
    {
        auto &transformComponent = view.get<TransformComponent>(entity);
        auto &meshComponent = view.get<MeshComponent>(entity);
        auto &materialComponent = view.get<MaterialComponent>(entity);
        // 检查是否加载Vulkan资源
        // 是否需要更新资源
        // 加入渲染队列
        mRenderQueues[materialComponent.material->GetName()].push_back(entity);
    }
}

} // namespace MEngine::Function