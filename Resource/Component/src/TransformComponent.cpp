#include "TransformComponent.hpp"

namespace MEngine::Resource
{
void TransformComponent::Translate(const Vector3 &delta)
{
    localPosition += delta;
    Dirty = true;
}
void TransformComponent::Rotate(float angle, const Vector3 &axis)
{
    Quaternion deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    localRotation = glm::normalize(deltaRotation * localRotation);
    Dirty = true;
}
void TransformComponent::Scale(const Vector3 &scale)
{
    localScale *= scale;
    Dirty = true;
}
} // namespace MEngine::Resource