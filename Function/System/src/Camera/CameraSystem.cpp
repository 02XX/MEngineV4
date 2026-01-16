#include "CameraSystem.hpp"
#include "CameraComponent.hpp"
#include "Math.hpp"
#include "TransformComponent.hpp"
namespace MEngine::Function
{
void CameraSystem::Init()
{
}
void CameraSystem::Update(double deltaTime)
{
    auto view = mScene->mRegistry->view<CameraComponent, TransformComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<CameraComponent>(entity);
        auto &transformComponent = view.get<TransformComponent>(entity);
        auto &camera = view.get<CameraComponent>(entity);
        glm::mat4 viewMatrix = glm::lookAtLH(transformComponent.worldPosition, camera.target, camera.up);
        auto rotationMatrix = glm::mat4_cast(transformComponent.worldRotation);
        camera.viewMatrix = rotationMatrix * viewMatrix;
        camera.projectionMatrix = glm::perspectiveLH(glm::radians(camera.fovY / camera.zoom), camera.aspectRatio,
                                                     camera.nearPlane, camera.farPlane);
    }
}
void CameraSystem::Shutdown()
{
}
} // namespace MEngine::Function
