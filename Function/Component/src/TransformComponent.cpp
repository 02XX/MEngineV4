#include "TransformComponent.hpp"

namespace MEngine::Function
{
void TransformComponent::Translate(const Vector3 &delta)
{
    localPosition += delta;
    dirty = true;
}
void TransformComponent::Rotate(float angle, const Vector3 &axis)
{
    Quaternion deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    localRotation = glm::normalize(deltaRotation * localRotation);
    dirty = true;
}
void TransformComponent::Scale(const Vector3 &scale)
{
    localScale *= scale;
    dirty = true;
}
} // namespace MEngine::Function