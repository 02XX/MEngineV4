#pragma once
#include "Component.hpp"
#include "ECS.hpp"
#include "Math.hpp"
#include <string>
namespace MEngine::Function
{

class TransformComponent : public Component
{
  public:
    ~TransformComponent() override = default;
    std::string name = "None";

    Vector3 localPosition{0.0f, 0.0f, 0.0f};
    Quaternion localRotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    Vector3 localScale{1.0f, 1.0f, 1.0f};
    Vector3 worldPosition{0.0f, 0.0f, 0.0f};
    Quaternion worldRotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    Vector3 worldScale{1.0f, 1.0f, 1.0f};

    Matrix4 modelMatrix = Matrix4(1.0f);
    bool dirty = true;

    Entity parent = Null;
    std::vector<Entity> children;
};

} // namespace MEngine::Function