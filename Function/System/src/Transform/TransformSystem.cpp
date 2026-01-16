#include "TransformSystem.hpp"

namespace MEngine::Function
{
void TransformSystem::Init()
{
}
void TransformSystem::Update(double deltaTime)
{
    auto view = mScene->mRegistry->view<TransformComponent>();
    for (auto entity : view)
    {
        auto &transformComponent = view.get<TransformComponent>(entity);
        CalculateMatrix(entity);
    }
}
void TransformSystem::Shutdown()
{
}
void TransformSystem::CalculateMatrix(entt::entity entity)
{
    // 获取实体的TransformComponent
    auto &transformComponent = mScene->mRegistry->get<TransformComponent>(entity);
    // local
    glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), transformComponent.localPosition) *
                            glm::mat4_cast(transformComponent.localRotation) *
                            glm::scale(glm::mat4(1.0f), transformComponent.localScale);
    if (transformComponent.parent != entt::null)
    {
        auto &parentTransform = mScene->mRegistry->get<TransformComponent>(transformComponent.parent);
        transformComponent.modelMatrix = parentTransform.modelMatrix * localMatrix;
    }
    else
    {
        transformComponent.modelMatrix = localMatrix;
    }

    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(transformComponent.modelMatrix, transformComponent.worldScale, transformComponent.worldRotation,
                   transformComponent.worldPosition, skew, perspective);

    // 递归更新所有子节点
    for (auto child : transformComponent.children)
    {
        CalculateMatrix(child);
    }
}
} // namespace MEngine::Function